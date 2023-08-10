#pragma once

#include <array>
#include <memory>
#include <vector>

#include "BoardTranslator.h"
#include "CertaboBoardMessageParser.h"
#include "CertaboCalibrator.h"
#include "CertaboLedControl.h"
#include "ChessnutConverter.h"

namespace eboard {

/** Callback function for sending data via BLE. */
using ToBleFunction = std::function<void(uint8_t* data, size_t data_len, bool isBoardData)>;

/**
 * ChessnutAdapter adapts board data from Certabo to Chessnut and commands from Chessnut to Certabo.
 */
class ChessnutAdapter {
  public:
    ChessnutAdapter(ToUsbFunction toUsb, ToBleFunction toBle);

    /**
     * fromUsb is called when data is received via USB.
     * @param data
     * @param data_len
     */
    void fromUsb(uint8_t* data, size_t data_len);

    /**
     * fromBle is called when data is received via BLE.
     * @param data
     * @param data_len
     */
    void fromBle(uint8_t* data, size_t data_len);

    /**
     * A direct LED command for Certabo, without any conversion
     * @param command the LED command
     */
    void ledCommand(std::vector<uint8_t> const& command);

    /**
     * @return whether the Certabo board is calibrated
     */
    bool isCalibrated() const;

  private:
    static void clearBitForSquare(std::vector<uint8_t>& data, int square);

    std::vector<uint8_t> calibrationLeds;
    eboard::CertaboLedControl ledControl;
    ToBleFunction toBle;
    CertaboCalibrator calibrator;
    ChessnutConverter converter;
    eboard::Stones stones;
    bool calibrationComplete = false;
    bool initialPositionReceived = false;
    std::unique_ptr<CertaboBoardMessageParser> boardMessageParser;
    static std::array<eboard::StoneId, 64> const standardPosition;
};

} // namespace eboard
