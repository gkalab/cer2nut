#pragma once

/**
 * Original code: https://github.com/maksimKorzh/chess_programming/blob/master/src/chess_0x88/chess_0x88.c
 * by Code Monkey King
 */

#include <array>
#include <cstdint>
#include <map>
#include <vector>

/* Move formatting
0000 0000 0000 0000 0000 0000 0011 1111       source square
0000 0000 0000 0000 0000 1111 1100 0000       target square
0000 0000 0000 0000 1111 0000 0000 0000       promoted piece
0000 0000 0000 1111 0000 0000 0000 0000       captured piece
0000 0000 0001 0000 0000 0000 0000 0000       double pawn flag
0000 0000 0010 0000 0000 0000 0000 0000       en passant flag
0000 0011 1100 0000 0000 0000 0000 0000       castle flags
1111 1100 0000 0000 0000 0000 0000 0000       en passant square of position before move
*/

#define to_64(square) ((square >> 4) * 8 + (square & 7))

// encode move
#define encode_move(source, target, piece, capture, pawn, enpassant, castling, epsquare)                               \
    ((to_64(source)) | (to_64(target) << 6) | (piece << 12) | (capture << 16) | (pawn << 20) | (enpassant << 21) |     \
     (castling << 22) | (to_64(epsquare) << 26))

// decode move's source square
#define get_move_source_64(move) (move & 0x3f)
#define get_move_source(move) (((get_move_source_64(move) / 8) * 16) + (get_move_source_64(move) & 7))

// decode move's target square
#define get_move_target_64(move) ((move >> 6) & 0x3f)
#define get_move_target(move) (((get_move_target_64(move) / 8) * 16) + (get_move_target_64(move) & 7))

// decode move's promoted piece
#define get_move_piece(move) ((move >> 12) & 0xf)

// decode move's captured piece
#define get_move_capture(move) ((move >> 16) & 0xf)

// decode move's double pawn push flag
#define get_move_pawn(move) ((move >> 20) & 0x1)

// decode move's en passant flag
#define get_move_enpassant(move) ((move >> 21) & 0x1)

// decode move's castling flag
#define get_move_castling(move) ((move >> 22) & 0xf)

// decode ep square
#define get_move_epsquare_64(move) ((move >> 26) & 0x3f)
#define get_move_epsquare(move) (((get_move_epsquare_64(move) / 8) * 16) + (get_move_epsquare_64(move) & 7))

namespace chess {

// piece encoding
enum pieces { e, P, N, B, R, Q, K, p, n, b, r, q, k, o };

// clang-format off
// square encoding
enum squares {
    a8 = 0,   b8, c8, d8, e8, f8, g8, h8,
    a7 = 16,  b7, c7, d7, e7, f7, g7, h7,
    a6 = 32,  b6, c6, d6, e6, f6, g6, h6,
    a5 = 48,  b5, c5, d5, e5, f5, g5, h5,
    a4 = 64,  b4, c4, d4, e4, f4, g4, h4,
    a3 = 80,  b3, c3, d3, e3, f3, g3, h3,
    a2 = 96,  b2, c2, d2, e2, f2, g2, h2,
    a1 = 112, b1, c1, d1, e1, f1, g1, h1, no_sq
};
// clang-format on

// castling writes
enum castling { KC = 1, QC = 2, kc = 4, qc = 8 };

// sides to move
enum sides { white, black };

// piece move offsets
static int knight_offsets[8] = {33, 31, 18, 14, -33, -31, -18, -14};
static int bishop_offsets[4] = {15, 17, -15, -17};
static int rook_offsets[4] = {16, -16, 1, -1};
static int king_offsets[8] = {16, -16, 1, -1, 15, 17, -15, -17};

class Chess0x88 {
  public:
    Chess0x88();

    void reset_board();
    void parse_fen(const char* fen);
    std::vector<uint32_t> generate_moves();
    bool make_move(uint32_t move);
    void unmake_move(uint32_t move);
    uint32_t pop();

    int getSideToMove() const;

    std::vector<uint8_t> getOccupiedSquares() const;

    uint8_t getPiece(uint8_t rank, uint8_t file) const;
    uint8_t getPiece(uint8_t square);
    uint8_t getPieceColor(uint8_t square);

    bool isStartPosition();

  private:
    int is_square_attacked(int square, int side);

    static uint8_t castling_rights[128];
    static std::array<uint8_t, 128> const START_POSITION;
    static std::map<const char, const uint8_t> const CHAR_PIECES;
    std::array<uint8_t, 128> board;
    int sideToMove = white;
    int enpassant = no_sq;
    // castling rights (dec 15 => bin 1111 => both kings can castle to both sides)
    int castle = 15;
    // kings' squares
    int king_square[2] = {e1, e8};
    std::vector<uint32_t> moves;
};

} // namespace chess
