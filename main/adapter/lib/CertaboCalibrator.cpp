#include "CertaboCalibrator.h"
#include "CertaboParser.h"
#include "ChessData.h"

using eboard::CertaboCalibrator;

CertaboCalibrator::CertaboCalibrator(eboard::CalibrationCompleteFunction completeFunction,
                                     eboard::CalibrationCompleteForSquareFunction completeForSquareFunction)
    : completeFunction(std::move(completeFunction)), completeForSquareFunction(std::move(completeForSquareFunction)),
      parser(*this) {
    for (int i = 0; i < 16; i++) {
        // black squares
        calibrationSquares.emplace_back(i);
    }
    for (int i = 48; i < 64; i++) {
        // white squares
        calibrationSquares.emplace_back(i);
    }
    calibrationSquares.emplace_back(19); // black extra queen square
    calibrationSquares.emplace_back(43); // white extra queen square
}

void CertaboCalibrator::calibrate(uint8_t* data, size_t data_len) {
    parser.parse(data, data_len);
}

void CertaboCalibrator::hasPieceRecognition(bool) {
    // ignore
}

void CertaboCalibrator::translate(std::vector<CertaboPiece> board) {
    receivedBoards.push_back(board);
    if (receivedBoards.size() >= 7 && !calibrationComplete) {
        if (checkPieces()) {
            receivedBoards.clear();
            Stones stones;
            for (CalibrationSquare square : calibrationSquares) {
                int stone = square.getStone();
                if (stone != ChessData::NO_STONE) {
                    stones[square.getPiece()] = stone;
                }
            }
            calibrationComplete = true;
            completeFunction(stones);
        } else if (receivedBoards.size() > 15) {
            receivedBoards.erase(receivedBoards.begin(), receivedBoards.end() - 10);
        }
    }
}

bool CertaboCalibrator::checkPieces() {
    bool allCalibrated = true;
    for (CalibrationSquare& square : calibrationSquares) {
        if (!square.isCalibrated()) {
            square.calibratePiece(receivedBoards, completeForSquareFunction);
            if (!square.isCalibrated()) {
                allCalibrated = false;
            }
        }
    }
    return allCalibrated;
}
