#pragma once

#include <array>

#include "CertaboCalibrator.h"

namespace eboard {

/**
 * Callback function to be called for converted data.
 * Converted data can be board data, battery information, ack bytes or other data.
 * This data should to be sent back to the app controlling the board.
 */
using ConverterCallbackFunction = std::function<void(uint8_t* data, size_t data_len)>;

/**
 * ChessnutConverter converts internal board representation to Chessnut board output.
 * and handles Chessnut commands
 */
class ChessnutConverter {
  public:
    /**
     * Constructor
     * @param boardCallback Callback function for board data, shall be sent via BLE to the controlling app
     * @param infoCallback Callback function for acknowledgement, battery information or other data, shall be sent via BLE
     */
    explicit ChessnutConverter(ConverterCallbackFunction boardCallback, ConverterCallbackFunction infoCallback);

    /**
     * Convert from internal board representation to chessnut board output.
     * The callback function will be called with the converted board and date/time information.
     * Example output of initial position:
     * Example:
     * 012458233185444444440000000000000000000000000000000077777777A6C99B6Ab0915700
     * [
     * 01 24 --> board message via BLE
     * 58 23 31 85 44 44 44 44 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 77 77 77 77 A6 C9 9B 6A
     * timestamp (four bytes with least significant byte first)
     * ]
     * @param board internal board representation
     */
    void process(std::array<eboard::StoneId, 64> const& board);

    /**
     * Convert a Chessnut command to a Certabo command.
     * Commands that don't do anything for Certabo are either ignored or just acknowledged, i.e. the callback
     * is called with the correct ack sequence.
     * @param data
     * @param data_len
     * @return the command sequence
     */
    std::vector<uint8_t> chessnutToCertaboCommand(uint8_t* data, size_t data_len);

  private:
    static uint8_t setLowerNibble(uint8_t orig, uint8_t nibble);
    static uint8_t setUpperNibble(uint8_t orig, uint8_t nibble);
    static uint8_t stoneToChessnutStone(eboard::StoneId stone);

    ConverterCallbackFunction boardCallback;
    ConverterCallbackFunction infoCallback;
    bool realTimeMode = false;
};

} // namespace eboard
