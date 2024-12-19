#pragma once

#include <atomic>
#include <cstdint>
#include <vector>

namespace eboard {

class RgbLedCommandTranslator {
  public:
    RgbLedCommandTranslator();
    std::vector<uint8_t> translate(std::vector<uint8_t> const& command);
    void setBrightness(int brightnessValue);

  private:
    /** Function to set color for a specific square */
    void setColorForSquare(std::vector<uint8_t>& ledValues, int squareIndex, uint8_t r, uint8_t g, uint8_t b);

    /** Add bytes at the beginning and end of the vector */
    std::vector<uint8_t> addStartEndBytes(const std::vector<uint8_t>& ledValues);

    std::atomic_int brightness;
};

} // namespace eboard
