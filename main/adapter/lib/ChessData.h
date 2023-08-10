#pragma once

#include <cstdint>

namespace eboard {

class ChessData {
  public:
    static const uint8_t NO_STONE = 0;
    static const uint8_t WHITE_PAWN = 0x01;
    static const uint8_t WHITE_ROOK = 0x02;
    static const uint8_t WHITE_KNIGHT = 0x03;
    static const uint8_t WHITE_BISHOP = 0x04;
    static const uint8_t WHITE_QUEEN = 0x05;
    static const uint8_t WHITE_KING = 0x06;
    static const uint8_t BLACK_PAWN = 0x81;
    static const uint8_t BLACK_ROOK = 0x82;
    static const uint8_t BLACK_KNIGHT = 0x83;
    static const uint8_t BLACK_BISHOP = 0x84;
    static const uint8_t BLACK_QUEEN = 0x85;
    static const uint8_t BLACK_KING = 0x86;
};

} // namespace eboard