#include <algorithm>
#include <chrono>
#include <utility>

#include "ChessData.h"
#include "Sentio.h"

using chess::pieces;
using eboard::ChessData;
using eboard::Sentio;
using eboard::StoneId;

const std::vector<uint8_t> Sentio::SQUARES_INITIAL_POSITION = {
    0,  1,  2,  3,  4,  5,  6,  7,  //
    8,  9,  10, 11, 12, 13, 14, 15, //
    48, 49, 50, 51, 52, 53, 54, 55, //
    56, 57, 58, 59, 60, 61, 62, 63, //
};

Sentio::Sentio(BoardCallbackFunction callbackFunction) : callback(std::move(callbackFunction)) {}

Sentio::Sentio(eboard::BoardCallbackFunction callbackFunction, chess::Chess0x88 initialBoard)
    : callback(std::move(callbackFunction)), board(std::move(initialBoard)) {}

const std::map<int, int> Sentio::PIECE_TO_STONE_ID{
    {pieces::P, ChessData::WHITE_PAWN},   //
    {pieces::N, ChessData::WHITE_KNIGHT}, //
    {pieces::B, ChessData::WHITE_BISHOP}, //
    {pieces::R, ChessData::WHITE_ROOK},   //
    {pieces::Q, ChessData::WHITE_QUEEN},  //
    {pieces::K, ChessData::WHITE_KING},   //
    {pieces::p, ChessData::BLACK_PAWN},   //
    {pieces::n, ChessData::BLACK_KNIGHT}, //
    {pieces::b, ChessData::BLACK_BISHOP}, //
    {pieces::r, ChessData::BLACK_ROOK},   //
    {pieces::q, ChessData::BLACK_QUEEN},  //
    {pieces::k, ChessData::BLACK_KING},   //
};

void Sentio::occupiedSquares(std::array<bool, 64> const& occupied) {
    lastReceivedOccupiedSquares = occupied;
    uint64_t currentTime =
        std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now().time_since_epoch())
            .count();
    if (lastProcessedOccupiedSquares == lastReceivedOccupiedSquares) {
        if ((lastBoardSendTime + MIN_TIME_TO_PROCESS_MS) <= currentTime) {
            callback(lastBoardArray);
            lastBoardSendTime = currentTime;
        }
        return;
    }
    if ((lastProcessTime + MIN_TIME_TO_PROCESS_MS) <= currentTime) {
        processOccupiedSquares(occupied);
    }
}

void Sentio::processOccupiedSquares(const std::array<bool, 64>& occupied) {
    lastProcessedOccupiedSquares = occupied;
    lastProcessTime =
        std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now().time_since_epoch())
            .count();
    std::vector<uint8_t> expectedSquares = board.getOccupiedSquares();
    std::vector<uint8_t> occupiedSquares = toSquares(occupied);
    if (expectedSquares == occupiedSquares) {
        callCallback(toBoardArray(board));
    } else if (occupiedSquares == SQUARES_INITIAL_POSITION) {
        if (!board.isStartPosition()) {
            board = chess::Chess0x88();
        }
        callCallback(toBoardArray(board));
    } else if (!takeBackMove(occupiedSquares)) {
        checkValidMove(expectedSquares, occupiedSquares);
    }
}

std::vector<uint8_t> Sentio::toSquares(const std::array<bool, 64>& occupied) {
    std::vector<uint8_t> result;
    for (int i = 0; i < occupied.size(); i++) {
        if (occupied[i]) {
            result.push_back(i);
        }
    }
    return result;
}

void Sentio::callCallback(std::array<StoneId, 64> const& boardArray) {
    lastBoardArray = boardArray;
    callback(boardArray);
}

std::array<StoneId, 64> Sentio::toBoardArray(chess::Chess0x88& chessBoard) {
    std::array<StoneId, 64> result{};
    for (uint8_t rank = 0; rank < 8; rank++) {
        for (uint8_t file = 0; file < 8; file++) {
            uint8_t piece = chessBoard.getPiece(rank, file);
            if (piece != pieces::e && piece != pieces::o) {
                result[(7 - rank) * 8 + file] = PIECE_TO_STONE_ID.at(piece);
            }
        }
    }
    return result;
}

bool eboard::Sentio::takeBackMove(std::vector<uint8_t> const& occupiedSquares) {
    uint32_t previousMove = board.pop();
    if (previousMove != 0) {
        board.unmake_move(previousMove);
        if (board.getOccupiedSquares() == occupiedSquares) {
            callCallback(toBoardArray(board));
            return true;
        } else {
            board.make_move(previousMove);
        }
    }
    return false;
}

void eboard::Sentio::checkValidMove(std::vector<uint8_t> const& expectedSquares,
                                    std::vector<uint8_t> const& occupiedSquares) {
    std::vector<uint8_t> missing;
    std::vector<uint8_t> extra;
    setDifference(expectedSquares, occupiedSquares, missing, extra);
    if (!nonCaptureMove(occupiedSquares, missing, extra)) {
        if (!captureMove(missing, extra)) {
            incompleteMove(missing);
        }
    }
}

void eboard::Sentio::setDifference(const std::vector<uint8_t>& expectedSquares,
                                   const std::vector<uint8_t>& occupiedSquares, std::vector<uint8_t>& missing,
                                   std::vector<uint8_t>& extra) {
    std::set_difference(expectedSquares.begin(), expectedSquares.end(), occupiedSquares.begin(), occupiedSquares.end(),
                        std::inserter(missing, missing.begin()));
    std::set_difference(occupiedSquares.begin(), occupiedSquares.end(), expectedSquares.begin(), expectedSquares.end(),
                        std::inserter(extra, extra.begin()));
}

static uint8_t toBoardArraySquare(uint8_t sq) {
    uint8_t file = sq & 7;
    uint8_t rank = sq / 8;
    return (7 - rank) * 8 + file;
}

bool eboard::Sentio::nonCaptureMove(std::vector<uint8_t> const& occupiedSquares, std::vector<uint8_t> const& missing,
                                    std::vector<uint8_t> const& extra) {
    if (capturePiece == nullptr && missing.size() == 1 && extra.size() == 1) {
        if (makeMove(missing[0], extra[0])) {
            std::vector<uint8_t> expectedSquares = board.getOccupiedSquares();
            if (expectedSquares == occupiedSquares) {
                callCallback(toBoardArray(board));
            } else {
                // special handling for castling - rook needs to move as well
                std::vector<uint8_t> miss;
                std::vector<uint8_t> ex;
                setDifference(expectedSquares, occupiedSquares, miss, ex);
                if (miss.size() == 1 && ex.size() == 1) {
                    std::array<StoneId, 64> boardArray = toBoardArray(board);
                    boardArray[toBoardArraySquare(ex[0])] = PIECE_TO_STONE_ID.at(board.getPiece(miss[0]));
                    boardArray[toBoardArraySquare(miss[0])] = 0;
                    callCallback(boardArray);
                }
            }
        } else {
            std::array<StoneId, 64> boardArray = toBoardArray(board);
            boardArray[toBoardArraySquare(extra[0])] = board.getPiece(missing[0]);
            boardArray[toBoardArraySquare(missing[0])] = 0;
            callCallback(boardArray);
        }
        return true;
    }
    return false;
}

void eboard::Sentio::incompleteMove(std::vector<uint8_t> const& missing) {
    checkForPromotionPieceChange(missing);
    if (!missing.empty()) {
        std::array<StoneId, 64> boardArray = toBoardArray(board);
        for (auto sq : missing) {
            boardArray[toBoardArraySquare(sq)] = 0;
        }
        callCallback(boardArray);
    }
}

void eboard::Sentio::checkForPromotionPieceChange(const std::vector<uint8_t>& missing) {
    if (missing.size() == 2 || missing.size() == 3) {
        uint8_t kingSquare = 128; // invalid square
        uint8_t pawnSquare = 128; // invalid square
        for (auto sq : missing) {
            uint8_t piece = board.getPiece(sq);
            if (piece == chess::K || piece == chess::k) {
                kingSquare = sq;
            }
            if (piece == chess::P || piece == chess::p) {
                pawnSquare = sq;
            }
        }
        if (kingSquare != 128 && pawnSquare != 128 &&
            board.getPieceColor(kingSquare) == board.getPieceColor(pawnSquare) &&
            ((board.getPieceColor(pawnSquare) == chess::white && pawnSquare < 16) ||
             (board.getPieceColor(pawnSquare) == chess::black && pawnSquare > 47))) {
            if (promoteToPieceWhite == chess::Q) {
                promoteToPieceWhite = chess::N;
                promoteToPieceBlack = chess::n;
            } else {
                promoteToPieceWhite++;
                promoteToPieceBlack++;
            }
        }
    }
}

bool eboard::Sentio::captureMove(std::vector<uint8_t> const& missing, std::vector<uint8_t> const& extra) {
    if (missing.size() == 2 && extra.empty()) {
        if (board.getPieceColor(missing[0]) != board.getPieceColor(missing[1])) {
            if (board.getPieceColor(missing[0]) == board.getSideToMove()) {
                capturePiece = std::make_unique<CapturePiece>(board.getPiece(missing[0]), missing[0], missing[1]);
            } else {
                capturePiece = std::make_unique<CapturePiece>(board.getPiece(missing[1]), missing[1], missing[0]);
            }
        } else {
            capturePiece.reset();
        }
        std::array<StoneId, 64> boardArray = toBoardArray(board);
        boardArray[toBoardArraySquare(missing[0])] = 0;
        boardArray[toBoardArraySquare(missing[1])] = 0;
        callCallback(boardArray);
    } else if (capturePiece != nullptr && isPossibleCapture(missing, extra)) {
        uint8_t fromSquare = capturePiece->getFromSquare();
        uint8_t toSquare = capturePiece->getToSquare();
        capturePiece.reset();
        bool result = makeMove(fromSquare, toSquare);
        callCallback(toBoardArray(board));
        return result;
    } else if (capturePiece != nullptr && isPossibleEpCapture(missing, extra)) {
        uint8_t fromSquare = capturePiece->getFromSquare();
        uint8_t toSquare = extra[0];
        capturePiece.reset();
        bool result = makeMove(fromSquare, toSquare);
        callCallback(toBoardArray(board));
        return result;
    }
    return false;
}

bool eboard::Sentio::makeMove(uint8_t fromSquare, uint8_t toSquare) {
    auto moves = board.generate_moves();
    for (uint32_t move : moves) {
        if (fromSquare == get_move_source_64(move) && toSquare == get_move_target_64(move)) {
            uint32_t promoPiece = get_move_piece(move);
            if (!promoPiece || promoPiece == promoteToPieceWhite || promoPiece == promoteToPieceBlack) {
                resetPromoteToPieces();
                return board.make_move(move);
            }
        }
    }
    return false;
}

void eboard::Sentio::resetPromoteToPieces() {
    promoteToPieceWhite = chess::Q;
    promoteToPieceBlack = chess::q;
}

bool eboard::Sentio::isPossibleCapture(std::vector<uint8_t> const& missing, std::vector<uint8_t> const& extra) {
    return missing.size() == 1 && extra.empty();
}

bool eboard::Sentio::isPossibleEpCapture(std::vector<uint8_t> const& missing, std::vector<uint8_t> const& extra) {
    return missing.size() == 2 && extra.size() == 1 &&
           (capturePiece->getPiece() == chess::pieces::P || capturePiece->getPiece() == chess::pieces::p);
}
