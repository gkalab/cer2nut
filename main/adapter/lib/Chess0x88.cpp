/**
* Original code: https://github.com/maksimKorzh/chess_programming/blob/master/src/chess_0x88/chess_0x88.c
* by Code Monkey King
*/

#include "Chess0x88.h"

using namespace chess;

/* castling rights
                             castle   move     in      in
                              right    map     binary  decimal

        white king  moved:     1111 & 1100  =  1100    12
  white king's rook moved:     1111 & 1110  =  1110    14
 white queen's rook moved:     1111 & 1101  =  1101    13

         black king moved:     1111 & 0011  =  1011    3
  black king's rook moved:     1111 & 1011  =  1011    11
 black queen's rook moved:     1111 & 0111  =  0111    7
*/
// clang-format off
uint8_t Chess0x88::castling_rights[128]{
     7, 15, 15, 15,  3, 15, 15, 11,  o, o, o, o, o, o, o, o,
    15, 15, 15, 15, 15, 15, 15, 15,  o, o, o, o, o, o, o, o,
    15, 15, 15, 15, 15, 15, 15, 15,  o, o, o, o, o, o, o, o,
    15, 15, 15, 15, 15, 15, 15, 15,  o, o, o, o, o, o, o, o,
    15, 15, 15, 15, 15, 15, 15, 15,  o, o, o, o, o, o, o, o,
    15, 15, 15, 15, 15, 15, 15, 15,  o, o, o, o, o, o, o, o,
    15, 15, 15, 15, 15, 15, 15, 15,  o, o, o, o, o, o, o, o,
    13, 15, 15, 15, 12, 15, 15, 14,  o, o, o, o, o, o, o, o
};
// clang-format on

std::array<uint8_t, 128> const Chess0x88::START_POSITION{
    // clang-format off
    r, n, b, q, k, b, n, r,  o, o, o, o, o, o, o, o,
    p, p, p, p, p, p, p, p,  o, o, o, o, o, o, o, o,
    e, e, e, e, e, e, e, e,  o, o, o, o, o, o, o, o,
    e, e, e, e, e, e, e, e,  o, o, o, o, o, o, o, o,
    e, e, e, e, e, e, e, e,  o, o, o, o, o, o, o, o,
    e, e, e, e, e, e, e, e,  o, o, o, o, o, o, o, o,
    P, P, P, P, P, P, P, P,  o, o, o, o, o, o, o, o,
    R, N, B, Q, K, B, N, R,  o, o, o, o, o, o, o, o
    // clang-format on
};

std::map<const char, const uint8_t> const Chess0x88::CHAR_PIECES{
    {'P', P}, {'N', N}, {'B', B}, {'R', R}, {'Q', Q}, {'K', K},
    {'p', p}, {'n', n}, {'b', b}, {'r', r}, {'q', q}, {'k', k},
};

Chess0x88::Chess0x88() : board{START_POSITION} {
    moves.reserve(500);
}

void Chess0x88::reset_board() {
    // loop over board ranks
    for (int rank = 0; rank < 8; rank++) {
        // loop over board files
        for (int file = 0; file < 16; file++) {
            // init square
            int square = rank * 16 + file;
            // if square is on board
            if (!(square & 0x88))
                // reset current board square
                board[square] = e;
        }
    }
    // reset stats
    sideToMove = -1;
    castle = 0;
    enpassant = no_sq;
    moves.clear();
}

void Chess0x88::parse_fen(const char* fen) {
    reset_board();
    for (int rank = 0; rank < 8; rank++) {
        for (int file = 0; file < 16; file++) {
            int square = rank * 16 + file;

            // if square is on board
            if (!(square & 0x88)) {
                // match pieces
                if ((*fen >= 'a' && *fen <= 'z') || (*fen >= 'A' && *fen <= 'Z')) {
                    // set up kings' squares
                    if (*fen == 'K')
                        king_square[white] = square;
                    else if (*fen == 'k')
                        king_square[black] = square;
                    // set the piece on board
                    board[square] = CHAR_PIECES.at(*fen);
                    // increment FEN pointer
                    fen++;
                }

                // match empty squares
                if (*fen >= '0' && *fen <= '9') {
                    // calculate offset
                    int offset = *fen - '0';
                    // decrement file on empty squares
                    if (!(board[square]))
                        file--;
                    // skip empty squares
                    file += offset;
                    fen++;
                }

                // match end of rank
                if (*fen == '/') {
                    fen++;
                }
            }
        }
    }

    // go to side parsing
    fen++;
    // parse side to move
    sideToMove = (*fen == 'w') ? white : black;

    // go to castling rights parsing
    fen += 2;

    // parse castling rights
    while (*fen != ' ') {
        switch (*fen) {
        case 'K':
            castle |= KC;
            break;
        case 'Q':
            castle |= QC;
            break;
        case 'k':
            castle |= kc;
            break;
        case 'q':
            castle |= qc;
            break;
        case '-':
            break;
        }
        // increment pointer
        fen++;
    }
    // got to en passant square
    fen++;
    // parse en passant square
    if (*fen != '-') {
        // parse en passant square's file & rank
        int file = fen[0] - 'a';
        int rank = 8 - (fen[1] - '0');
        // set up en passant square
        enpassant = rank * 16 + file;
    } else {
        enpassant = no_sq;
    }
}

int Chess0x88::is_square_attacked(int square, int side) {
    // pawn attacks
    if (!side) {
        // if target square is on board and is white pawn
        if (!((square + 17) & 0x88) && (board[square + 17] == P))
            return 1;

        // if target square is on board and is white pawn
        if (!((square + 15) & 0x88) && (board[square + 15] == P))
            return 1;
    }

    else {
        // if target square is on board and is black pawn
        if (!((square - 17) & 0x88) && (board[square - 17] == p))
            return 1;

        // if target square is on board and is black pawn
        if (!((square - 15) & 0x88) && (board[square - 15] == p))
            return 1;
    }

    // knight attacks
    for (int knight_offset : knight_offsets) {
        // init target square
        int target_square = square + knight_offset;

        // lookup target piece
        int target_piece = board[target_square];

        // if target square is on board
        if (!(target_square & 0x88)) {
            if (!side ? target_piece == N : target_piece == n)
                return 1;
        }
    }

    // king attacks
    for (int king_offset : king_offsets) {
        // init target square
        int target_square = square + king_offset;

        // lookup target piece
        int target_piece = board[target_square];

        // if target square is on board
        if (!(target_square & 0x88)) {
            // if target piece is either white or black king
            if (!side ? target_piece == K : target_piece == k)
                return 1;
        }
    }

    // bishop & queen attacks
    for (int bishop_offset : bishop_offsets) {
        // init target square
        int target_square = square + bishop_offset;

        // loop over attack ray
        while (!(target_square & 0x88)) {
            // target piece
            int target_piece = board[target_square];

            // if target piece is either white or black bishop or queen
            if (!side ? (target_piece == B || target_piece == Q) : (target_piece == b || target_piece == q))
                return 1;

            // break if hit a piece
            if (target_piece)
                break;

            // increment target square by move offset
            target_square += bishop_offset;
        }
    }

    // rook & queen attacks
    for (int rook_offset : rook_offsets) {
        // init target square
        int target_square = square + rook_offset;

        // loop over attack ray
        while (!(target_square & 0x88)) {
            // target piece
            int target_piece = board[target_square];

            // if target piece is either white or black bishop or queen
            if (!side ? (target_piece == R || target_piece == Q) : (target_piece == r || target_piece == q))
                return 1;

            // break if hit a piece
            if (target_piece)
                break;

            // increment target square by move offset
            target_square += rook_offset;
        }
    }
    return 0;
}

// move generator
std::vector<uint32_t> Chess0x88::generate_moves() {
    std::vector<uint32_t> move_list;

    // loop over all board squares
    for (int square = 0; square < board.size(); square++) {
        // check if the square is on board
        if (!(square & 0x88)) {
            // white pawn and castling moves
            if (!sideToMove) {
                // white pawn moves
                if (board[square] == P) {
                    // init target square
                    int to_square = square - 16;

                    // quite white pawn moves (check if target square is on board)
                    if (!(to_square & 0x88) && !board[to_square]) {
                        // pawn promotions
                        if (square >= a7 && square <= h7) {
                            move_list.push_back(encode_move(square, to_square, Q, 0, 0, 0, castle, enpassant));
                            move_list.push_back(encode_move(square, to_square, R, 0, 0, 0, castle, enpassant));
                            move_list.push_back(encode_move(square, to_square, B, 0, 0, 0, castle, enpassant));
                            move_list.push_back(encode_move(square, to_square, N, 0, 0, 0, castle, enpassant));
                        }

                        else {
                            // one square ahead pawn move
                            move_list.push_back(encode_move(square, to_square, 0, 0, 0, 0, castle, enpassant));

                            // two squares ahead pawn move
                            if ((square >= a2 && square <= h2) && !board[square - 32])
                                move_list.push_back(encode_move(square, (square - 32), 0, 0, 1, 0, castle, enpassant));
                        }
                    }

                    // white pawn capture moves
                    for (int pawn_offset : bishop_offsets) {
                        // white pawn offsets
                        if (pawn_offset < 0) {
                            // init target square
                            int to_square = square + pawn_offset;

                            // check if target square is on board
                            if (!(to_square & 0x88)) {
                                // capture pawn promotion
                                if ((square >= a7 && square <= h7) &&
                                    (board[to_square] >= 7 && board[to_square] <= 12)) {
                                    move_list.push_back(
                                        encode_move(square, to_square, Q, board[to_square], 0, 0, castle, enpassant));
                                    move_list.push_back(
                                        encode_move(square, to_square, R, board[to_square], 0, 0, castle, enpassant));
                                    move_list.push_back(
                                        encode_move(square, to_square, B, board[to_square], 0, 0, castle, enpassant));
                                    move_list.push_back(
                                        encode_move(square, to_square, N, board[to_square], 0, 0, castle, enpassant));
                                }

                                else {
                                    // casual capture
                                    if (board[to_square] >= 7 && board[to_square] <= 12)
                                        move_list.push_back(encode_move(square, to_square, 0, board[to_square], 0, 0,
                                                                        castle, enpassant));

                                    // enpassant capture
                                    if (to_square == enpassant)
                                        move_list.push_back(
                                            encode_move(square, to_square, 0, 7, 0, 1, castle, enpassant));
                                }
                            }
                        }
                    }
                }

                // white king castling
                if (board[square] == K) {
                    // if king side castling is available
                    if (castle & KC) {
                        // make sure there are empty squares between king & rook
                        if (!board[f1] && !board[g1]) {
                            // make sure king & next square are not under attack
                            if (!is_square_attacked(e1, black) && !is_square_attacked(f1, black))
                                move_list.push_back(encode_move(e1, g1, 0, 0, 0, 0, castle, enpassant));
                        }
                    }

                    // if queen side castling is available
                    if (castle & QC) {
                        // make sure there are empty squares between king & rook
                        if (!board[d1] && !board[b1] && !board[c1]) {
                            // make sure king & next square are not under attack
                            if (!is_square_attacked(e1, black) && !is_square_attacked(d1, black))
                                move_list.push_back(encode_move(e1, c1, 0, 0, 0, 0, castle, enpassant));
                        }
                    }
                }
            }

            // black pawn and castling moves
            else {
                // black pawn moves
                if (board[square] == p) {
                    // init target square
                    int to_square = square + 16;

                    // quite black pawn moves (check if target square is on board)
                    if (!(to_square & 0x88) && !board[to_square]) {
                        // pawn promotions
                        if (square >= a2 && square <= h2) {
                            move_list.push_back(encode_move(square, to_square, q, 0, 0, 0, castle, enpassant));
                            move_list.push_back(encode_move(square, to_square, r, 0, 0, 0, castle, enpassant));
                            move_list.push_back(encode_move(square, to_square, b, 0, 0, 0, castle, enpassant));
                            move_list.push_back(encode_move(square, to_square, n, 0, 0, 0, castle, enpassant));
                        }

                        else {
                            // one square ahead pawn move
                            move_list.push_back(encode_move(square, to_square, 0, 0, 0, 0, castle, enpassant));

                            // two squares ahead pawn move
                            if ((square >= a7 && square <= h7) && !board[square + 32])
                                move_list.push_back(encode_move(square, (square + 32), 0, 0, 1, 0, castle, enpassant));
                        }
                    }

                    // black pawn capture moves
                    for (int pawn_offset : bishop_offsets) {
                        // white pawn offsets
                        if (pawn_offset > 0) {
                            // init target square
                            int to_square = square + pawn_offset;

                            // check if target square is on board
                            if (!(to_square & 0x88)) {
                                // capture pawn promotion
                                if ((square >= a2 && square <= h2) &&
                                    (board[to_square] >= 1 && board[to_square] <= 6)) {
                                    move_list.push_back(
                                        encode_move(square, to_square, q, board[to_square], 0, 0, castle, enpassant));
                                    move_list.push_back(
                                        encode_move(square, to_square, r, board[to_square], 0, 0, castle, enpassant));
                                    move_list.push_back(
                                        encode_move(square, to_square, b, board[to_square], 0, 0, castle, enpassant));
                                    move_list.push_back(
                                        encode_move(square, to_square, n, board[to_square], 0, 0, castle, enpassant));
                                }

                                else {
                                    // casual capture
                                    if (board[to_square] >= 1 && board[to_square] <= 6)
                                        move_list.push_back(encode_move(square, to_square, 0, board[to_square], 0, 0,
                                                                        castle, enpassant));

                                    // en passant capture
                                    if (to_square == enpassant)
                                        move_list.push_back(
                                            encode_move(square, to_square, 0, 1, 0, 1, castle, enpassant));
                                }
                            }
                        }
                    }
                }

                // black king castling
                if (board[square] == k) {
                    // if king side castling is available
                    if (castle & kc) {
                        // make sure there are empty squares between king & rook
                        if (!board[f8] && !board[g8]) {
                            // make sure king & next square are not under attack
                            if (!is_square_attacked(e8, white) && !is_square_attacked(f8, white))
                                move_list.push_back(encode_move(e8, g8, 0, 0, 0, 0, castle, enpassant));
                        }
                    }

                    // if queen side castling is available
                    if (castle & qc) {
                        // make sure there are empty squares between king & rook
                        if (!board[d8] && !board[b8] && !board[c8]) {
                            // make sure king & next square are not under attack
                            if (!is_square_attacked(e8, white) && !is_square_attacked(d8, white))
                                move_list.push_back(encode_move(e8, c8, 0, 0, 0, 0, castle, enpassant));
                        }
                    }
                }
            }

            // knight moves
            if (!sideToMove ? board[square] == N : board[square] == n) {
                // loop over knight move offsets
                for (int knight_offset : knight_offsets) {
                    // init target square
                    int to_square = square + knight_offset;

                    // init target piece
                    int piece = board[to_square];

                    // make sure target square is onboard
                    if (!(to_square & 0x88)) {
                        //
                        if (!sideToMove ? (!piece || (piece >= 7 && piece <= 12))
                                        : (!piece || (piece >= 1 && piece <= 6))) {
                            // on capture
                            if (piece)
                                move_list.push_back(encode_move(square, to_square, 0, piece, 0, 0, castle, enpassant));

                            // on empty square
                            else
                                move_list.push_back(encode_move(square, to_square, 0, 0, 0, 0, castle, enpassant));
                        }
                    }
                }
            }

            // king moves
            if (!sideToMove ? board[square] == K : board[square] == k) {
                // loop over king move offsets
                for (int king_offset : king_offsets) {
                    // init target square
                    int to_square = square + king_offset;

                    // init target piece
                    int piece = board[to_square];

                    // make sure target square is onboard
                    if (!(to_square & 0x88)) {
                        //
                        if (!sideToMove ? (!piece || (piece >= 7 && piece <= 12))
                                        : (!piece || (piece >= 1 && piece <= 6))) {
                            // on capture
                            if (piece)
                                move_list.push_back(encode_move(square, to_square, 0, piece, 0, 0, castle, enpassant));

                            // on empty square
                            else
                                move_list.push_back(encode_move(square, to_square, 0, 0, 0, 0, castle, enpassant));
                        }
                    }
                }
            }

            // bishop & queen moves
            if (!sideToMove ? (board[square] == B) || (board[square] == Q)
                            : (board[square] == b) || (board[square] == q)) {
                // loop over bishop & queen offsets
                for (int bishop_offset : bishop_offsets) {
                    // init target square
                    int to_square = square + bishop_offset;

                    // loop over attack ray
                    while (!(to_square & 0x88)) {
                        // init target piece
                        int piece = board[to_square];

                        // if hits own piece
                        if (!sideToMove ? (piece >= 1 && piece <= 6) : ((piece >= 7 && piece <= 12)))
                            break;

                        // if hits opponent's piece
                        if (!sideToMove ? (piece >= 7 && piece <= 12) : ((piece >= 1 && piece <= 6))) {
                            move_list.push_back(encode_move(square, to_square, 0, piece, 0, 0, castle, enpassant));
                            break;
                        }

                        // if steps into an empty square
                        if (!piece)
                            move_list.push_back(encode_move(square, to_square, 0, 0, 0, 0, castle, enpassant));

                        // increment target square
                        to_square += bishop_offset;
                    }
                }
            }

            // rook & queen moves
            if (!sideToMove ? (board[square] == R) || (board[square] == Q)
                            : (board[square] == r) || (board[square] == q)) {
                // loop over bishop & queen offsets
                for (int rook_offset : rook_offsets) {
                    // init target square
                    int to_square = square + rook_offset;

                    // loop over attack ray
                    while (!(to_square & 0x88)) {
                        // init target piece
                        int piece = board[to_square];

                        // if hits own piece
                        if (!sideToMove ? (piece >= 1 && piece <= 6) : ((piece >= 7 && piece <= 12)))
                            break;

                        // if hits opponent's piece
                        if (!sideToMove ? (piece >= 7 && piece <= 12) : ((piece >= 1 && piece <= 6))) {
                            move_list.push_back(encode_move(square, to_square, 0, piece, 0, 0, castle, enpassant));
                            break;
                        }

                        // if steps into an empty square
                        if (!piece)
                            move_list.push_back(encode_move(square, to_square, 0, 0, 0, 0, castle, enpassant));

                        // increment target square
                        to_square += rook_offset;
                    }
                }
            }
        }
    }
    return move_list;
}

bool Chess0x88::make_move(uint32_t move) {
    // parse move
    int from_square = get_move_source(move);
    int to_square = get_move_target(move);
    int promoted_piece = get_move_piece(move);
    int enpass = get_move_enpassant(move);
    int double_push = get_move_pawn(move);
    bool castling = false;
    if ((board[from_square] == K || board[from_square] == k) &&
        ((from_square == e1 && (to_square == g1 || to_square == c1)) ||
         (from_square == e8 && (to_square == g8 || to_square == c8)))) {
        castling = true;
    }

    // move piece
    board[to_square] = board[from_square];
    board[from_square] = e;

    // pawn promotion
    if (promoted_piece)
        board[to_square] = promoted_piece;

    // enpassant capture
    if (enpass)
        !sideToMove ? (board[to_square + 16] = e) : (board[to_square - 16] = e);

    // reset enpassant square
    enpassant = no_sq;

    // double pawn push
    if (double_push)
        !sideToMove ? (enpassant = to_square + 16) : (enpassant = to_square - 16);

    // castling
    if (castling) {
        // switch target square
        switch (to_square) {
        // white castles king side
        case g1:
            board[f1] = board[h1];
            board[h1] = e;
            break;

        // white castles queen side
        case c1:
            board[d1] = board[a1];
            board[a1] = e;
            break;

            // black castles king side
        case g8:
            board[f8] = board[h8];
            board[h8] = e;
            break;

            // black castles queen side
        case c8:
            board[d8] = board[a8];
            board[a8] = e;
            break;
        default:
            break;
        }
    }

    // update king square
    if (board[to_square] == K || board[to_square] == k)
        king_square[sideToMove] = to_square;

    // update castling rights
    castle &= castling_rights[from_square];
    castle &= castling_rights[to_square];

    // change side
    sideToMove ^= 1;

    // take move back if king is under check
    if (is_square_attacked(king_square[sideToMove ^ 1], sideToMove)) {
        unmake_move(move);
        // illegal move
        return false;
    } else {
        moves.push_back(move);
        // legal move
        return true;
    }
}

void Chess0x88::unmake_move(uint32_t move) {
    int from_square = get_move_source(move);
    int to_square = get_move_target(move);
    int promoted_piece = get_move_piece(move);
    int captured_piece = get_move_capture(move);
    int enpass = get_move_enpassant(move);
    int ep_square = get_move_epsquare(move);

    sideToMove ^= 1;

    if (promoted_piece) {
        board[from_square] = sideToMove == white ? P : p;
    } else {
        board[from_square] = board[to_square];
    }

    if (captured_piece) {
        if (enpass) {
            !sideToMove ? (board[to_square + 16] = captured_piece) : (board[to_square - 16] = captured_piece);
            board[to_square] = e;
        } else {
            board[to_square] = captured_piece;
        }
    } else {
        board[to_square] = e;
    }

    enpassant = ep_square;

    bool castling = false;
    if ((board[from_square] == K || board[from_square] == k) &&
        ((from_square == e1 && (to_square == g1 || to_square == c1)) ||
         (from_square == e8 && (to_square == g8 || to_square == c8)))) {
        castling = true;
    }
    if (castling) {
        switch (to_square) {
        case g1:
            board[h1] = board[f1];
            board[f1] = e;
            break;
        case c1:
            board[a1] = board[d1];
            board[d1] = e;
            break;
        case g8:
            board[h8] = board[f8];
            board[f8] = e;
            break;
        case c8:
            board[a8] = board[d8];
            board[d8] = e;
            break;
        default:
            break;
        }
    }
    if (board[from_square] == K || board[from_square] == k) {
        king_square[sideToMove] = from_square;
    }
    castle = get_move_castling(move);
}

uint32_t Chess0x88::pop() {
    if (!moves.empty()) {
        uint32_t move = moves.back();
        moves.pop_back();
        return move;
    } else {
        return 0;
    }
}

int Chess0x88::getSideToMove() const {
    return sideToMove;
}

std::vector<uint8_t> Chess0x88::getOccupiedSquares() const {
    std::vector<uint8_t> result;
    for (int rank = 0; rank < 8; rank++) {
        for (int file = 0; file < 16; file++) {
            uint8_t square = rank * 16 + file;
            // if square is on board and not empty
            if (!(square & 0x88) && (board[square])) {
                result.push_back(rank * 8 + file);
            }
        }
    }
    return result;
}

uint8_t Chess0x88::getPiece(uint8_t rank, uint8_t file) const {
    uint8_t square = rank * 16 + file;
    return board[square];
}

static int offsetFile(int offset) {
    return offset & 7;
}

static int offsetRank(int offset) {
    return (offset / 8) * 16;
}

uint8_t Chess0x88::getPiece(uint8_t square) {
    return board[offsetRank(square) + offsetFile(square)];
}

uint8_t Chess0x88::getPieceColor(uint8_t square) {
    uint8_t piece = getPiece(square);
    if (piece > 0 && piece <= 6) {
        return white;
    } else if (piece > 6 && piece < 13) {
        return black;
    } else {
        return -1;
    }
}

bool Chess0x88::isStartPosition() {
    return board == START_POSITION && sideToMove == white && enpassant == no_sq && castle == 15;
}
