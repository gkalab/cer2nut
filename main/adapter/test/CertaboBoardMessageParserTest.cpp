#include <gmock/gmock.h>

#include <vector>

#include "BoardTranslator.h"
#include "CertaboBoardMessageParser.h"

using ::testing::AtLeast;

using eboard::BoardTranslator;
using eboard::CertaboBoardMessageParser;
using eboard::CertaboCalibrator;
using eboard::CertaboPiece;

class CertaboBoardMessageParserTest : public ::testing::Test {
  protected:
    void SetUp() override {
        boardMessageParser = std::make_unique<CertaboBoardMessageParser>(
            [this](std::array<eboard::StoneId, 64> board) {
                parsedBoard = board;
            },
            [this](bool pieceRecognition) {
                hasPieceRecognition = pieceRecognition;
            },
            [this](bool hasRgbLeds) {});
        CertaboCalibrator calibrator(
            [this](eboard::Stones const& stones) {
                boardMessageParser->updateStones(stones);
            },
            [](int square) {}, [](bool hasRgbLeds) {});
        std::string boardData(
            ":48 0 248 71 99 48 0 248 85 159 48 0 177 203 192 48 0 177 215 17 48 0 177 117 59 48 0 177 43 7 48 0 248 "
            "222 81 48 0 247 200 86 48 0 248 114 180 48 0 248 155 251 48 0 248 48 74 48 0 177 236 131 48 0 177 230 12 "
            "48 0 177 187 36 48 0 248 146 97 48 0 248 89 231 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 "
            "0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 "
            "0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 "
            "0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 48 0 248 85 122 48 0 248 68 117 48 0 248 201 109 "
            "48 0 248 144 65 48 0 177 231 217 48 0 248 76 179 48 0 248 161 89 48 0 94 124 14 48 0 248 98 180 48 0 248 "
            "233 43 48 0 248 86 247 48 0 248 145 6 48 0 248 104 144 48 0 248 79 194 48 0 248 134 85 48 0 177 81 "
            "73\r\n");
        std::vector<uint8_t> data(boardData.begin(), boardData.end());
        for (int i = 0; i < 7; i++) {
            calibrator.calibrate(&data.front(), data.size());
        }
    }

    void givenParsedInput(std::string const& str) {
        whenParsingInput(str);
    }

    void whenParsingInput(std::string const& str) {
        std::vector<uint8_t> data(str.begin(), str.end());
        boardMessageParser->parse(&data.front(), data.size());
    }

    void thenParsedBoardShouldBe(std::array<eboard::StoneId, 64> const& board) {
        EXPECT_EQ(parsedBoard, board);
    }

    void thenPieceRecognitionShouldBe(bool expected) {
        EXPECT_EQ(expected, hasPieceRecognition);
    }

  private:
    std::unique_ptr<CertaboBoardMessageParser> boardMessageParser;
    std::array<eboard::StoneId, 64> parsedBoard = {};
    bool hasPieceRecognition = false;
};

static std::string INITIAL_POSITION(
    ":48 0 248 71 99 48 0 248 85 159 48 0 177 203 192 48 0 177 215 17 48 0 177 117 59 48 0 177 43 7 48 0 248 "
    "222 81 48 0 247 200 86 48 0 248 114 180 48 0 248 155 251 48 0 248 48 74 48 0 177 236 131 48 0 177 230 12 "
    "48 0 177 187 36 48 0 248 146 97 48 0 248 89 231 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 "
    "0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 "
    "0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 "
    "0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 48 0 248 85 122 48 0 248 68 117 48 0 248 201 109 "
    "48 0 248 144 65 48 0 177 231 217 48 0 248 76 179 48 0 248 161 89 48 0 94 124 14 48 0 248 98 180 48 0 248 "
    "233 43 48 0 248 86 247 48 0 248 145 6 48 0 248 104 144 48 0 248 79 194 48 0 248 134 85 48 0 177 81 73\r\n");

static std::string PAWN_H7_REMOVED(
    ":48 0 248 71 99 48 0 248 85 159 48 0 177 203 192 48 0 177 215 17 48 0 177 117 59 48 0 177 43 7 48 0 248 "
    "222 81 48 0 247 200 86 48 0 248 114 180 48 0 248 155 251 48 0 248 48 74 48 0 177 236 131 48 0 177 230 12 "
    "48 0 177 187 36 48 0 248 146 97 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 "
    "0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 "
    "0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 "
    "0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 48 0 248 85 122 48 0 248 68 117 48 0 248 201 109 "
    "48 0 248 144 65 48 0 177 231 217 48 0 248 76 179 48 0 248 161 89 48 0 94 124 14 48 0 248 98 180 48 0 248 "
    "233 43 48 0 248 86 247 48 0 248 145 6 48 0 248 104 144 48 0 248 79 194 48 0 248 134 85 48 0 177 81 73\r\n");

static std::string INITIAL_POSITION_WITH_NOISE(
    ":48 0 248 71 99 48 0 248 85 159 48 0 177 203 192 48 0 177 23 19 48 0 177 117 59 48 0 177 43 6 48 0 248 "
    "222 81 48 0 239 200 86 48 0 248 114 180 48 0 248 155 251 48 0 248 48 74 48 0 177 236 131 48 0 177 230 12 "
    "48 0 177 187 36 48 0 248 146 97 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 "
    "0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 "
    "0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 "
    "0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 48 0 248 85 122 48 0 248 73 250 48 0 248 201 109 "
    "48 0 248 144 65 48 0 177 231 217 48 0 248 76 179 48 0 248 161 89 48 0 30 124 13 48 0 248 98 180 48 0 248 "
    "233 43 48 0 248 86 247 48 0 248 145 6 48 0 248 104 144 48 0 248 79 194 48 0 248 134 85 48 0 177 81 73\r\n");

TEST_F(CertaboBoardMessageParserTest, parseInitialPositionCallsPieceRecognitionCallback) {
    whenParsingInput(INITIAL_POSITION);
    thenPieceRecognitionShouldBe(true);
}

TEST_F(CertaboBoardMessageParserTest, parseInitialPosition) {
    whenParsingInput(INITIAL_POSITION);
    thenParsedBoardShouldBe({                                        //
                             2,   3,   4,   5,   6,   4,   3,   2,   //
                             1,   1,   1,   1,   1,   1,   1,   1,   //
                             0,   0,   0,   0,   0,   0,   0,   0,   //
                             0,   0,   0,   0,   0,   0,   0,   0,   //
                             0,   0,   0,   0,   0,   0,   0,   0,   //
                             0,   0,   0,   0,   0,   0,   0,   0,   //
                             129, 129, 129, 129, 129, 129, 129, 129, //
                             130, 131, 132, 133, 134, 132, 131, 130});
}

TEST_F(CertaboBoardMessageParserTest, parseInitialPosition_pawnRemovedOn_h7) {
    whenParsingInput(PAWN_H7_REMOVED);
    thenParsedBoardShouldBe({                                      //
                             2,   3,   4,   5,   6,   4,   3,   2, //
                             1,   1,   1,   1,   1,   1,   1,   1, //
                             0,   0,   0,   0,   0,   0,   0,   0, //
                             0,   0,   0,   0,   0,   0,   0,   0, //
                             0,   0,   0,   0,   0,   0,   0,   0, //
                             0,   0,   0,   0,   0,   0,   0,   0, //
                             129, 129, 129, 129, 129, 129, 129, 0, //
                             130, 131, 132, 133, 134, 132, 131, 130});
}

TEST_F(CertaboBoardMessageParserTest, parseInitialPosition_somePiecesRemovedDueToNoise) {
    whenParsingInput(INITIAL_POSITION_WITH_NOISE);
    thenParsedBoardShouldBe({                                      //
                             2,   3,   4,   5,   6,   4,   3,   2, //
                             1,   0,   1,   1,   1,   1,   1,   0, //
                             0,   0,   0,   0,   0,   0,   0,   0, //
                             0,   0,   0,   0,   0,   0,   0,   0, //
                             0,   0,   0,   0,   0,   0,   0,   0, //
                             0,   0,   0,   0,   0,   0,   0,   0, //
                             129, 129, 129, 129, 129, 129, 129, 0, //
                             130, 131, 132, 0,   134, 0,   131, 0});
}

TEST_F(CertaboBoardMessageParserTest, averageLastThreeBoards) {
    givenParsedInput(INITIAL_POSITION);
    givenParsedInput(PAWN_H7_REMOVED);
    whenParsingInput(INITIAL_POSITION_WITH_NOISE);
    thenParsedBoardShouldBe({                                      //
                             2,   3,   4,   5,   6,   4,   3,   2, //
                             1,   1,   1,   1,   1,   1,   1,   1, //
                             0,   0,   0,   0,   0,   0,   0,   0, //
                             0,   0,   0,   0,   0,   0,   0,   0, //
                             0,   0,   0,   0,   0,   0,   0,   0, //
                             0,   0,   0,   0,   0,   0,   0,   0, //
                             129, 129, 129, 129, 129, 129, 129, 0, //
                             130, 131, 132, 133, 134, 132, 131, 130});
}
