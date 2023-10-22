#include <gmock/gmock.h>

#include <vector>

#include "Chess0x88.h"

using ::testing::AtLeast;

using chess::Chess0x88;

class Chess0x88Test : public ::testing::Test {
  protected:
    void givenAnInstance() {
        instance = std::make_unique<Chess0x88>();
    }

    void thenOccupiedSquaresShouldBe(std::vector<uint8_t> const& expected) {
        EXPECT_EQ(expected, instance->getOccupiedSquares());
    }

    void thenPieceAtSquareShouldBe(uint8_t rank, uint8_t file, chess::pieces expectedPiece) {
        EXPECT_EQ(expectedPiece, instance->getPiece(rank, file));
    }

    void doPerftFor(int depth) {
        if (depth == 0) {
            nodes++;
            return;
        }
        auto move_list = instance->generate_moves();
        for (uint32_t move : move_list) {
            if (!instance->make_move(move)) {
                continue;
            }
            doPerftFor(depth - 1);
            instance->unmake_move(move);
        }
    }

    void testPerft(std::string const& fen, int depth, long expectedNodes) {
        nodes = 0;
        instance->parse_fen(fen.c_str());
        doPerftFor(depth);
        EXPECT_EQ(expectedNodes, nodes);
    }

  private:
    std::unique_ptr<Chess0x88> instance;
    long nodes = 0;
};

TEST_F(Chess0x88Test, getOccupiedSquaresForInitialPosition) {
    givenAnInstance();
    std::vector<uint8_t> expected{
        0,  1,  2,  3,  4,  5,  6,  7,  //
        8,  9,  10, 11, 12, 13, 14, 15, //
        48, 49, 50, 51, 52, 53, 54, 55, //
        56, 57, 58, 59, 60, 61, 62, 63, //
    };
    thenOccupiedSquaresShouldBe(expected);
}

TEST_F(Chess0x88Test, getPieceSquare0) {
    givenAnInstance();
    thenPieceAtSquareShouldBe(0, 0, chess::pieces::r);
    thenPieceAtSquareShouldBe(0, 1, chess::pieces::n);
    thenPieceAtSquareShouldBe(7, 0, chess::pieces::R);
}

TEST_F(Chess0x88Test, testPerft) {
    givenAnInstance();
    // Positions taken from https://www.chessprogramming.net/perfect-perft/
    testPerft("1k6/1b6/8/8/7R/8/8/4K2R b K - 0 1", 5, 1063513);
    // Illegal ep move #1
    testPerft("3k4/3p4/8/K1P4r/8/8/8/8 b - - 0 1", 6, 1134888);
    //--Illegal ep move #2
    testPerft("8/8/4k3/8/2p5/8/B2P2K1/8 w - - 0 1", 6, 1015133);
    // EP Capture Checks Opponent
    testPerft("8/8/1k6/2b5/2pP4/8/5K2/8 b - d3 0 1", 6, 1440467);
    // Short Castling Gives Check
    testPerft("5k2/8/8/8/8/8/8/4K2R w K - 0 1", 6, 661072);
    // Long Castling Gives Check
    testPerft("3k4/8/8/8/8/8/8/R3K3 w Q - 0 1", 6, 803711);
    // Castle Rights
    testPerft("r3k2r/1b4bq/8/8/8/8/7B/R3K2R w KQkq - 0 1", 4, 1274206);
    // Castling Prevented
    testPerft("r3k2r/8/3Q4/8/8/5q2/8/R3K2R b KQkq - 0 1", 4, 1720476);
    // Promote out of Check
    testPerft("2K2r2/4P3/8/8/8/8/8/3k4 w - - 0 1", 6, 3821001);
    // Discovered Check
    testPerft("8/8/1P2K3/8/2n5/1q6/8/5k2 b - - 0 1", 5, 1004658);
    // Promote to give check
    testPerft("4k3/1P6/8/8/8/8/K7/8 w - - 0 1", 6, 217342);
    // Under Promote to give check
    testPerft("8/P1k5/K7/8/8/8/8/8 w - - 0 1", 6, 92683);
    // Self Stalemate
    testPerft("K1k5/8/P7/8/8/8/8/8 w - - 0 1", 6, 2217);
    // Stalemate & Checkmate
    testPerft("8/k1P5/8/1K6/8/8/8/8 w - - 0 1", 7, 567584);
    // Stalemate & Checkmate
    testPerft("8/8/2k5/5q2/5n2/8/5K2/8 b - - 0 1", 4, 23527);
}
