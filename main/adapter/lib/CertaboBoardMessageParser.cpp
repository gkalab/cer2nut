#include <algorithm>
#include <unordered_map>

#include "CertaboBoardMessageParser.h"
#include "ChessData.h"

using eboard::CertaboBoardMessageParser;

CertaboBoardMessageParser::CertaboBoardMessageParser(Stones stones, CallbackFunction callbackFunction)
    : parser(CertaboParser(*this)), stones(std::move(stones)), callback(std::move(callbackFunction)) {}

void CertaboBoardMessageParser::parse(uint8_t* msg, size_t data_len) {
    parser.parse(msg, data_len);
}

void eboard::CertaboBoardMessageParser::hasPieceRecognition(bool canRecognize) {
    pieceRecognition = canRecognize;
}

void eboard::CertaboBoardMessageParser::translate(std::vector<CertaboPiece> board) {
    std::array<StoneId, 64> newBoard = {};
    int i = 0;
    for (auto& piece : board) {
        int square = toSquare(i);
        if (stones.find(piece) != stones.end()) {
            newBoard[square] = stones[piece];
        } else {
            newBoard[square] = ChessData::NO_STONE;
        }
        i++;
    }
    averageLastBoards(newBoard);
}

void CertaboBoardMessageParser::averageLastBoards(std::array<StoneId, 64> newBoard) {
    while (boardHistory.size() > 2) {
        boardHistory.pop_front();
    }
    boardHistory.push_back(newBoard);
    std::array<StoneId, 64> avg_board{};
    for (int i = 0; i < 64; i++) {
        std::vector<StoneId> counter;
        for (const auto& board : boardHistory) {
            counter.push_back(board[i]);
        }
        avg_board[i] = mostFrequent(counter);
    }
    callback(avg_board);
}

using MapEntry = std::pair<uint8_t, std::size_t>;
int CertaboBoardMessageParser::mostFrequent(std::vector<StoneId>& entries) {
    std::unordered_map<StoneId, std::size_t> freqMap;
    std::for_each(entries.begin(), entries.end(), [&](StoneId& elem) {
        freqMap[elem]++;
    });
    auto result = std::max_element(freqMap.begin(), freqMap.end(), [](MapEntry left, MapEntry right) {
        return left.second < right.second;
    });
    return result->first;
}

int CertaboBoardMessageParser::toSquare(int index) {
    int row = 7 - (index / 8);
    int col = index % 8;
    return row * 8 + col;
}
