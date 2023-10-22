#include <gmock/gmock.h>

#include <memory>
#include <string>
#include <thread>
#include <vector>

#include "Chess0x88.h"
#include "ChessData.h"
#include "Sentio.h"

using ::testing::AtLeast;

using eboard::ChessData;
using eboard::Sentio;

class SentioTest : public ::testing::Test {
  protected:
    void givenAnInstance() {
        instance = std::make_unique<Sentio>([this](std::array<eboard::StoneId, 64> board) {
            receivedBoard = board;
        });
    }

    void givenAnInstance(std::string const& fen) {
        chess::Chess0x88 board;
        board.parse_fen(fen.c_str());
        instance = std::make_unique<Sentio>(
            [this](std::array<eboard::StoneId, 64> board) {
                receivedBoard = board;
            },
            board);
    }

    void givenOccupiedSquaresOfInitialPosition() {
        givenOccupiedIsCalledWith("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR");
    }

    void givenOccupiedIsCalledWith(std::string const& shortFen) {
        whenCallingOccupiedSquaresWith(fenToOccupied(shortFen));
    }

    void whenOccupiedIsCalledWith(std::string const& shortFen) {
        whenCallingOccupiedSquaresWith(fenToOccupied(shortFen));
    }

    void whenCallingOccupiedSquaresWith(std::array<bool, 64> const& occupied) {
        instance->occupiedSquares(occupied);
        std::this_thread::sleep_for(std::chrono::milliseconds(Sentio::MIN_TIME_TO_PROCESS_MS));
    }

    void thenLastReceivedBoardShouldBe(std::string const& expectedBoard) {
        EXPECT_EQ(expectedBoard, asShortFen(receivedBoard));
    }

    void thenReceivedBoardShouldBe(std::array<eboard::StoneId, 64> const& expectedBoard) {
        EXPECT_EQ(expectedBoard, receivedBoard);
    }

    static std::array<bool, 64> fenToOccupied(std::string const& shortFen) {
        chess::Chess0x88 board;
        std::string fen = shortFen + " w - - 0 1";
        board.parse_fen(fen.c_str());
        std::array<bool, 64> result{};
        for (uint8_t square : board.getOccupiedSquares()) {
            result[square] = true;
        }
        return result;
    }

    std::map<eboard::StoneId, char> conversionMap{
        {ChessData::WHITE_PAWN, 'P'},   {ChessData::WHITE_ROOK, 'R'},  {ChessData::WHITE_KNIGHT, 'N'},
        {ChessData::WHITE_BISHOP, 'B'}, {ChessData::WHITE_QUEEN, 'Q'}, {ChessData::WHITE_KING, 'K'},
        {ChessData::BLACK_PAWN, 'p'},   {ChessData::BLACK_ROOK, 'r'},  {ChessData::BLACK_KNIGHT, 'n'},
        {ChessData::BLACK_BISHOP, 'b'}, {ChessData::BLACK_QUEEN, 'q'}, {ChessData::BLACK_KING, 'k'},
    };

    char pieceToChar(eboard::StoneId stoneId) {
        return conversionMap[stoneId];
    }

    std::string asShortFen(std::array<eboard::StoneId, 64> board) {
        std::string fen;
        for (int row = 7, rowIndex = 0; row >= 0; row--, rowIndex++) {
            int blanks = 0;
            for (int col = 0; col < 8; col++) {
                if (board[row * 8 + col] == ChessData::NO_STONE) {
                    blanks++;
                    if (col == 7 && blanks > 0) {
                        fen += std::to_string(blanks);
                    }
                } else {
                    if (blanks > 0) {
                        fen += std::to_string(blanks);
                    }
                    blanks = 0;
                    fen += pieceToChar(board[row * 8 + col]);
                }
            }
            if (rowIndex != 7) {
                fen += '/';
            }
        }
        return fen;
    }

  private:
    std::unique_ptr<Sentio> instance;
    std::array<eboard::StoneId, 64> receivedBoard{};
};

TEST_F(SentioTest, initialPosition) {
    givenAnInstance();
    whenOccupiedIsCalledWith("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR");
    thenLastReceivedBoardShouldBe("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR");
}

TEST_F(SentioTest, pickUpRookA1) {
    givenAnInstance();
    givenOccupiedSquaresOfInitialPosition();
    whenOccupiedIsCalledWith("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/1NBQKBNR"); // a1 up
    thenLastReceivedBoardShouldBe("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/1NBQKBNR");
}

TEST_F(SentioTest, nonCaptureMove) {
    givenAnInstance();
    givenOccupiedSquaresOfInitialPosition();
    givenOccupiedIsCalledWith("rnbqkbnr/pppppppp/8/8/8/8/PPPP1PPP/RNBQKBNR");  // e2 up
    whenOccupiedIsCalledWith("rnbqkbnr/pppppppp/8/8/4P3/8/PPPP1PPP/RNBQKBNR"); // e4 down
    thenLastReceivedBoardShouldBe("rnbqkbnr/pppppppp/8/8/4P3/8/PPPP1PPP/RNBQKBNR");
}

TEST_F(SentioTest, captureMove) {
    givenAnInstance("rnbqkbnr/ppp1pppp/8/3p4/4P3/8/PPPP1PPP/RNBQKBNR w KQkq d6 0 2");
    givenOccupiedIsCalledWith("rnbqkbnr/ppp1pppp/8/3p4/4P3/8/PPPP1PPP/RNBQKBNR");
    givenOccupiedIsCalledWith("rnbqkbnr/ppp1pppp/8/3p4/8/8/PPPP1PPP/RNBQKBNR"); // e4 up
    givenOccupiedIsCalledWith("rnbqkbnr/ppp1pppp/8/8/8/8/PPPP1PPP/RNBQKBNR");   // d5 up
    whenOccupiedIsCalledWith("rnbqkbnr/ppp1pppp/8/3p4/8/8/PPPP1PPP/RNBQKBNR");  // d5 down
    thenLastReceivedBoardShouldBe("rnbqkbnr/ppp1pppp/8/3P4/8/8/PPPP1PPP/RNBQKBNR");
}

TEST_F(SentioTest, gameSequenceThreeCaptureMovesApart) {
    givenAnInstance();
    // game sequence: 1. e4 c5 2. Nf3 d6 3. Bb5+ Bd7 4. Bxd7+ Qxd7 5. h4 Qc6 6. h5 Qxe4+
    givenOccupiedIsCalledWith("rnbqkbnr/pppppppp/8/8/8/8/PPPP1PPP/RNBQKBNR");   // e2 up
    givenOccupiedIsCalledWith("rnbqkbnr/pppppppp/8/8/4P3/8/PPPP1PPP/RNBQKBNR"); // e4 down

    givenOccupiedIsCalledWith("rnbqkbnr/pp1ppppp/8/8/4P3/8/PPPP1PPP/RNBQKBNR");   // c7 up
    givenOccupiedIsCalledWith("rnbqkbnr/pp1ppppp/8/2p5/4P3/8/PPPP1PPP/RNBQKBNR"); // c5 down

    givenOccupiedIsCalledWith("rnbqkbnr/pp1ppppp/8/2p5/4P3/8/PPPP1PPP/RNBQKB1R");   // Ng1 up
    givenOccupiedIsCalledWith("rnbqkbnr/pp1ppppp/8/2p5/4P3/5N2/PPPP1PPP/RNBQKB1R"); // Nf3 down

    givenOccupiedIsCalledWith("rnbqkbnr/pp2pppp/8/2p5/4P3/5N2/PPPP1PPP/RNBQKB1R");   // d7 up
    givenOccupiedIsCalledWith("rnbqkbnr/pp2pppp/3p4/2p5/4P3/5N2/PPPP1PPP/RNBQKB1R"); // d6 down

    givenOccupiedIsCalledWith("rnbqkbnr/pp2pppp/3p4/2p5/4P3/5N2/PPPP1PPP/RNBQK2R");  // Bf1 up
    givenOccupiedIsCalledWith("rnbqkbnr/pp2pppp/3p4/1Bp5/4P3/5N2/PPPP1PPP/RNBQK2R"); // Bb5 down

    givenOccupiedIsCalledWith("rn1qkbnr/pp2pppp/3p4/1Bp5/4P3/5N2/PPPP1PPP/RNBQK2R");  // Bc8 up
    givenOccupiedIsCalledWith("rn1qkbnr/pp1bpppp/3p4/1Bp5/4P3/5N2/PPPP1PPP/RNBQK2R"); // Bd7 down

    givenOccupiedIsCalledWith("rn1qkbnr/pp2pppp/3p4/1Bp5/4P3/5N2/PPPP1PPP/RNBQK2R"); // Bd7 up
    givenOccupiedIsCalledWith("rn1qkbnr/pp2pppp/3p4/2p5/4P3/5N2/PPPP1PPP/RNBQK2R");  // Bb5 up
    givenOccupiedIsCalledWith("rn1qkbnr/pp1Bpppp/3p4/2p5/4P3/5N2/PPPP1PPP/RNBQK2R"); // Bd7 down

    givenOccupiedIsCalledWith("rn1qkbnr/pp2pppp/3p4/2p5/4P3/5N2/PPPP1PPP/RNBQK2R"); // Bd7 up
    givenOccupiedIsCalledWith("rn2kbnr/pp2pppp/3p4/2p5/4P3/5N2/PPPP1PPP/RNBQK2R");  // Qd8 up
    givenOccupiedIsCalledWith("rn2kbnr/pp1qpppp/3p4/2p5/4P3/5N2/PPPP1PPP/RNBQK2R"); // Qd7 down

    givenOccupiedIsCalledWith("rn2kbnr/pp1qpppp/3p4/2p5/4P3/5N2/PPPP1PP1/RNBQK2R");  // h2 up
    givenOccupiedIsCalledWith("rn2kbnr/pp1qpppp/3p4/2p5/4P2P/5N2/PPPP1PP1/RNBQK2R"); // h4 down

    givenOccupiedIsCalledWith("rn2kbnr/pp2pppp/3p4/2p5/4P2P/5N2/PPPP1PP1/RNBQK2R");  // Qd7 up
    givenOccupiedIsCalledWith("rn2kbnr/pp2pppp/2qp4/2p5/4P2P/5N2/PPPP1PP1/RNBQK2R"); // Qc6 down

    givenOccupiedIsCalledWith("rn2kbnr/pp2pppp/2qp4/2p5/4P3/5N2/PPPP1PP1/RNBQK2R");  // h4 up
    givenOccupiedIsCalledWith("rn2kbnr/pp2pppp/2qp4/2p4P/4P3/5N2/PPPP1PP1/RNBQK2R"); // h5 down

    givenOccupiedIsCalledWith("rn2kbnr/pp2pppp/2qp4/2p4P/8/5N2/PPPP1PP1/RNBQK2R"); // e4 up
    givenOccupiedIsCalledWith("rn2kbnr/pp2pppp/3p4/2p4P/8/5N2/PPPP1PP1/RNBQK2R");  // Qc6 up
    whenOccupiedIsCalledWith("rn2kbnr/pp2pppp/3p4/2p4P/4q3/5N2/PPPP1PP1/RNBQK2R"); // Qe4 down

    thenLastReceivedBoardShouldBe("rn2kbnr/pp2pppp/3p4/2p4P/4q3/5N2/PPPP1PP1/RNBQK2R");
}

TEST_F(SentioTest, takeBack_A) {
    givenAnInstance();
    givenOccupiedSquaresOfInitialPosition();
    givenOccupiedIsCalledWith("rnbqkbnr/pppppppp/8/8/8/8/PPPP1PPP/RNBQKBNR");       // e2 up
    givenOccupiedIsCalledWith("rnbqkbnr/pppppppp/8/8/4P3/8/PPPP1PPP/RNBQKBNR");     // e4 down
    givenOccupiedIsCalledWith("rnbqkbnr/pppp1ppp/8/8/4P3/8/PPPP1PPP/RNBQKBNR");     // e7 up
    givenOccupiedIsCalledWith("rnbqkbnr/pppp1ppp/8/4p3/4P3/8/PPPP1PPP/RNBQKBNR");   // e5 down
    givenOccupiedIsCalledWith("rnbqkbnr/pppp1ppp/8/4p3/4P3/8/PPPP1PPP/RNBQKB1R");   // Ng1 up
    givenOccupiedIsCalledWith("rnbqkbnr/pppp1ppp/8/4p3/4P3/5N2/PPPP1PPP/RNBQKB1R"); // Nf3 down

    // take back Ng1Nf3
    givenOccupiedIsCalledWith("rnbqkbnr/pppp1ppp/8/4p3/4P3/8/PPPP1PPP/RNBQKB1R"); // Nf3 up
    givenOccupiedIsCalledWith("rnbqkbnr/pppp1ppp/8/4p3/4P3/8/PPPP1PPP/RNBQKBNR"); // Ng1 down
    // take back e7e5
    givenOccupiedIsCalledWith("rnbqkbnr/pppp1ppp/8/8/4P3/8/PPPP1PPP/RNBQKBNR"); // e5 up
    givenOccupiedIsCalledWith("rnbqkbnr/pppppppp/8/8/4P3/8/PPPP1PPP/RNBQKBNR"); // e7 down

    // make other moves d7d5, e4xd5, c7c6
    givenOccupiedIsCalledWith("rnbqkbnr/ppp1pppp/8/8/4P3/8/PPPP1PPP/RNBQKBNR");   // d7 up
    givenOccupiedIsCalledWith("rnbqkbnr/ppp1pppp/8/3p4/4P3/8/PPPP1PPP/RNBQKBNR"); // d5 down
    givenOccupiedIsCalledWith("rnbqkbnr/ppp1pppp/8/8/4P3/8/PPPP1PPP/RNBQKBNR");   // d5 up
    givenOccupiedIsCalledWith("rnbqkbnr/ppp1pppp/8/8/8/8/PPPP1PPP/RNBQKBNR");     // e4 up
    whenOccupiedIsCalledWith("rnbqkbnr/ppp1pppp/8/3P4/8/8/PPPP1PPP/RNBQKBNR");    // d5 down
    givenOccupiedIsCalledWith("rnbqkbnr/pp2pppp/8/3P4/8/8/PPPP1PPP/RNBQKBNR");    // c7 up
    whenOccupiedIsCalledWith("rnbqkbnr/pp2pppp/2p5/3P4/8/8/PPPP1PPP/RNBQKBNR");   // c6 down

    thenLastReceivedBoardShouldBe("rnbqkbnr/pp2pppp/2p5/3P4/8/8/PPPP1PPP/RNBQKBNR");
}

TEST_F(SentioTest, takeBack_B) {
    givenAnInstance();
    givenOccupiedSquaresOfInitialPosition();
    givenOccupiedIsCalledWith("rnbqkbnr/pppppppp/8/8/8/8/PPPP1PPP/RNBQKBNR");       // e2 up
    givenOccupiedIsCalledWith("rnbqkbnr/pppppppp/8/8/4P3/8/PPPP1PPP/RNBQKBNR");     // e4 down
    givenOccupiedIsCalledWith("rnbqkbnr/pppp1ppp/8/8/4P3/8/PPPP1PPP/RNBQKBNR");     // e7 up
    givenOccupiedIsCalledWith("rnbqkbnr/pppp1ppp/8/4p3/4P3/8/PPPP1PPP/RNBQKBNR");   // e5 down
    givenOccupiedIsCalledWith("rnbqkbnr/pppp1ppp/8/4p3/4P3/8/PPPP1PPP/RNBQKB1R");   // Ng1 up
    givenOccupiedIsCalledWith("rnbqkbnr/pppp1ppp/8/4p3/4P3/5N2/PPPP1PPP/RNBQKB1R"); // Nf3 down

    // take back Ng1Nf3
    givenOccupiedIsCalledWith("rnbqkbnr/pppp1ppp/8/4p3/4P3/8/PPPP1PPP/RNBQKB1R"); // Nf3 up
    givenOccupiedIsCalledWith("rnbqkbnr/pppp1ppp/8/4p3/4P3/8/PPPP1PPP/RNBQKBNR"); // Ng1 down
    // take back e7e5
    givenOccupiedIsCalledWith("rnbqkbnr/pppp1ppp/8/8/4P3/8/PPPP1PPP/RNBQKBNR"); // e5 up
    givenOccupiedIsCalledWith("rnbqkbnr/pppppppp/8/8/4P3/8/PPPP1PPP/RNBQKBNR"); // e7 down

    // make other moves d7d5, e4xd5, c7c6
    givenOccupiedIsCalledWith("rnbqkbnr/ppp1pppp/8/8/4P3/8/PPPP1PPP/RNBQKBNR");   // d7 up
    givenOccupiedIsCalledWith("rnbqkbnr/ppp1pppp/8/3p4/4P3/8/PPPP1PPP/RNBQKBNR"); // d5 down
    givenOccupiedIsCalledWith("rnbqkbnr/ppp1pppp/8/3p4/4P3/8/1PPP1PPP/RNBQKBNR"); // a2 up
    whenOccupiedIsCalledWith("rnbqkbnr/ppp1pppp/8/3p4/P3P3/8/1PPP1PPP/RNBQKBNR"); // a4 down
    thenLastReceivedBoardShouldBe("rnbqkbnr/ppp1pppp/8/3p4/P3P3/8/1PPP1PPP/RNBQKBNR");
}

TEST_F(SentioTest, takeBack_C) {
    givenAnInstance();
    givenOccupiedSquaresOfInitialPosition();
    givenOccupiedIsCalledWith("rnbqkbnr/pppppppp/8/8/8/8/PPPP1PPP/RNBQKBNR");     // e2 up
    givenOccupiedIsCalledWith("rnbqkbnr/pppppppp/8/8/4P3/8/PPPP1PPP/RNBQKBNR");   // e4 down
    givenOccupiedIsCalledWith("rnbqkbnr/ppp1pppp/8/8/4P3/8/PPPP1PPP/RNBQKBNR");   // d7 up
    givenOccupiedIsCalledWith("rnbqkbnr/ppp1pppp/8/3p4/4P3/8/PPPP1PPP/RNBQKBNR"); // d5 down
    givenOccupiedIsCalledWith("rnbqkbnr/ppp1pppp/8/8/4P3/8/PPPP1PPP/RNBQKBNR");   // d5 up
    givenOccupiedIsCalledWith("rnbqkbnr/ppp1pppp/8/8/8/8/PPPP1PPP/RNBQKBNR");     // e4 up
    givenOccupiedIsCalledWith("rnbqkbnr/ppp1pppp/8/3P4/8/8/PPPP1PPP/RNBQKBNR"); // d5 down

    // take back exd5
    givenOccupiedIsCalledWith("rnbqkbnr/ppp1pppp/8/8/8/8/PPPP1PPP/RNBQKBNR"); // d5 up
    givenOccupiedIsCalledWith("rnbqkbnr/ppp1pppp/8/8/4P3/8/PPPP1PPP/RNBQKBNR"); // d5 moved to e4
    thenLastReceivedBoardShouldBe("rnbqkbnr/ppp1pppp/8/8/4P3/8/PPPP1PPP/RNBQKBNR");
    whenOccupiedIsCalledWith("rnbqkbnr/ppp1pppp/8/3p4/4P3/8/PPPP1PPP/RNBQKBNR"); // d5 down

    thenLastReceivedBoardShouldBe("rnbqkbnr/ppp1pppp/8/3p4/4P3/8/PPPP1PPP/RNBQKBNR");
}