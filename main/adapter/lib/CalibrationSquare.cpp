#include <functional>
#include <map>
#include <vector>

#include "CalibrationSquare.h"
#include "ChessData.h"

eboard::CalibrationSquare::CalibrationSquare(int square)
    : square(square), piece(CertaboPiece(PieceId{255, 255, 255, 255, 255})) {}

bool eboard::CalibrationSquare::isCalibrated() {
    return piece.getId() != PieceId{255, 255, 255, 255, 255};
}

static int toSquare(int b) {
    int row = 7 - (b / 8);
    int col = b % 8;
    return row * 8 + col;
}

void eboard::CalibrationSquare::calibratePiece(std::vector<std::vector<CertaboPiece>>& receivedBoards,
                                               CalibrationCompleteForSquareFunction const& completeForSquareFunction) {
    std::map<CertaboPiece, int> pieceCount;
    for (auto& board : receivedBoards) {
        CertaboPiece& pc = board[square];
        pieceCount[pc]++;
    }
    for (auto& entry : pieceCount) {
        int count = entry.second;
        auto& pc = const_cast<CertaboPiece&>(entry.first);
        if (count > receivedBoards.size() / 2 && pieceOrExtraQueenSquare(pc)) {
            piece = CertaboPiece(pc.getId());
            if (pc.getId() != PieceId{}) {
                completeForSquareFunction(toSquare(square));
                //printf("calibration complete for square %d\n", toSquare(square));
            }
        }
    }
}

bool eboard::CalibrationSquare::pieceOrExtraQueenSquare(CertaboPiece& pc) const {
    return pc.getId() != PieceId{} || square == 19 || square == 43;
}

int eboard::CalibrationSquare::noStoneOr(int stone) {
    if (piece.getId() != PieceId{}) {
        return stone;
    } else {
        return ChessData::NO_STONE;
    }
}

int eboard::CalibrationSquare::getStone() {
    if (square > 7 && square < 16) {
        return ChessData::BLACK_PAWN;
    }
    if (square > 47 && square < 56) {
        return ChessData::WHITE_PAWN;
    }
    switch (square) {
    case 0:
    case 7:
        return noStoneOr(ChessData::BLACK_ROOK);
    case 1:
    case 6:
        return noStoneOr(ChessData::BLACK_KNIGHT);
    case 2:
    case 5:
        return noStoneOr(ChessData::BLACK_BISHOP);
    case 3:
    case 19: // black extra queen square
        return noStoneOr(ChessData::BLACK_QUEEN);
    case 4:
        return noStoneOr(ChessData::BLACK_KING);
    case 56:
    case 63:
        return noStoneOr(ChessData::WHITE_ROOK);
    case 57:
    case 62:
        return noStoneOr(ChessData::WHITE_KNIGHT);
    case 58:
    case 61:
        return noStoneOr(ChessData::WHITE_BISHOP);
    case 59:
    case 43: // white extra queen square
        return noStoneOr(ChessData::WHITE_QUEEN);
    case 60:
        return noStoneOr(ChessData::WHITE_KING);
    default:
        return ChessData::NO_STONE;
    }
}

eboard::CertaboPiece eboard::CalibrationSquare::getPiece() {
    return piece;
}

int eboard::CalibrationSquare::getSquare() const {
    return square;
}
