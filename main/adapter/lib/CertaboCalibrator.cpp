#include "CertaboCalibrator.h"
#include "CertaboParser.h"
#include "ChessData.h"

using eboard::CertaboCalibrator;

CertaboCalibrator::CertaboCalibrator(CalibrationCompleteFunction completeFunction,
                                     CalibrationCompleteForSquareFunction completeForSquareFunction,
                                     LedsDetectedFunction ledsDetectedFunction)
    : completeFunction(std::move(completeFunction)), completeForSquareFunction(std::move(completeForSquareFunction)),
      ledsDetectedFunction(std::move(ledsDetectedFunction)), parser(*this) {
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

void CertaboCalibrator::calibrate(const uint8_t* data, size_t data_len) {
    parser.parse(data, data_len);
}

void CertaboCalibrator::hasPieceRecognition(bool) {
    // ignore
}

void CertaboCalibrator::translate(std::vector<CertaboPiece> const& board) {
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

void CertaboCalibrator::ledsDetected(bool hasRgbLeds) {
    ledsDetectedFunction(hasRgbLeds);
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
