#include <cstring> // for memcpy

#include "RgbLedCommandTranslator.h"

using eboard::RgbLedCommandTranslator;

const int BOARD_SIZE = 8;
const int LEDS_PER_ROW = 9;
const int TOTAL_LEDS = 81;
const int COLORS_PER_LED = 3;

RgbLedCommandTranslator::RgbLedCommandTranslator() : brightness(0x40) {}

std::vector<uint8_t> RgbLedCommandTranslator::translate(std::vector<uint8_t> const& command) {
    std::vector<uint8_t> result = std::vector<uint8_t>(TOTAL_LEDS * COLORS_PER_LED);
    // Process each bit of the command
    for (int byteIndex = 0; byteIndex < command.size(); ++byteIndex) {
        uint8_t currentByte = command[byteIndex];
        for (int bitIndex = 0; bitIndex < 8; ++bitIndex) {
            if (currentByte & (1 << bitIndex)) {
                setColorForSquare(result, byteIndex * 8 + bitIndex, 0, 0, brightness);
            }
        }
    }
    return addStartEndBytes(result);
}

void RgbLedCommandTranslator::setColorForSquare(std::vector<uint8_t>& ledValues, int squareIndex, uint8_t r, uint8_t g,
                                                uint8_t b) {
    int row = 7 - squareIndex / BOARD_SIZE;
    int col = 7 - squareIndex % BOARD_SIZE;
    // Determine the indices of the LEDs for the given square
    int baseIndex = (row * LEDS_PER_ROW + col) * COLORS_PER_LED;
    int ledIndices[] = {baseIndex, baseIndex + COLORS_PER_LED, baseIndex + LEDS_PER_ROW * COLORS_PER_LED,
                        baseIndex + LEDS_PER_ROW * COLORS_PER_LED + COLORS_PER_LED};
    // Set color for the four LEDs of the specified square
    for (int i : ledIndices) {
        ledValues[i] = r;
        ledValues[i + 1] = g;
        ledValues[i + 2] = b;
    }
}

std::vector<uint8_t> RgbLedCommandTranslator::addStartEndBytes(const std::vector<uint8_t>& ledValues) {
    std::vector<uint8_t> result(ledValues.size() + 4);
    result[0] = 255;
    result[1] = 85;
    std::memcpy(&result[2], ledValues.data(), ledValues.size());
    result[result.size() - 2] = 13;
    result[result.size() - 1] = 10;
    return result;
}

void RgbLedCommandTranslator::setBrightness(int brightnessValue) {
    brightness = brightnessValue;
}
