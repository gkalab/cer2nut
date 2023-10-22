#pragma once

#include <functional>
#include <map>

#include "BoardTranslator.h"
#include "CalibrationSquare.h"
#include "CertaboParser.h"

namespace eboard {

using StoneId = uint8_t;
/**
 * Maps raw Certabo piece IDs to individual chess stones.
 */
using Stones = std::map<CertaboPiece, StoneId>;
/**
 * Function to be called when calibration is complete.
 */
using CalibrationCompleteFunction = std::function<void(Stones&)>;

/**
 * CertaboCalibrator calibrates the board.
 * It uses parsed, raw piece information to extracts the piece IDs and calls complete functions for when
 * a square or the whole board is calibrated.
 */
class CertaboCalibrator : public BoardTranslator {
  public:
    CertaboCalibrator(CalibrationCompleteFunction completeFunction,
                      CalibrationCompleteForSquareFunction completeForSquareFunction);
    ~CertaboCalibrator() override = default;

  public:
    /**
     * Called when the parser determines if the board supports piece recognition or not.
     * @param canRecognize
     */
    void hasPieceRecognition(bool canRecognize) override;

    /**
     * Function called by the used parser to translate the raw piece information.
     * @param board parsed board with raw piece information
     */
    void translate(std::vector<CertaboPiece> const& board) override;

    void translateOccupiedSquares(std::array<bool, 64> const& board) override{
        // ignore
    };

    /**
     * Calibrate raw board data.
     * This method must be called several times for the calibration to be complete.
     * @param data raw Certabo board data
     * @param data_len length of the raw Certabo board data
     */
    void calibrate(uint8_t* data, size_t data_len);

  private:
    bool checkPieces();

    CalibrationCompleteFunction completeFunction;
    CalibrationCompleteForSquareFunction completeForSquareFunction;
    CertaboParser parser;
    std::vector<std::vector<CertaboPiece>> receivedBoards;
    bool calibrationComplete = false;
    std::vector<CalibrationSquare> calibrationSquares;
};

} // namespace eboard