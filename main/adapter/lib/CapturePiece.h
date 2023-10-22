#pragma once

#include <cstdint>

namespace eboard {

class CapturePiece {

  public:
    CapturePiece(uint8_t piece, uint8_t fromSquare, uint8_t toSquare)
        : piece(piece), fromSquare(fromSquare), toSquare(toSquare){};

    uint8_t getPiece() {
        return piece;
    }
    uint8_t getFromSquare() {
        return fromSquare;
    }
    uint8_t getToSquare() {
        return toSquare;
    }

  private:
    uint8_t piece;
    uint8_t fromSquare;
    uint8_t toSquare;
};

} // namespace eboard
