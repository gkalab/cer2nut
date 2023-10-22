#include <gmock/gmock.h>

#include <array>
#include <vector>

#include "BoardTranslator.h"
#include "CertaboParser.h"

using ::testing::_;
using ::testing::AtLeast;

using eboard::BoardTranslator;
using eboard::CertaboParser;
using eboard::CertaboPiece;

class CertaboParserTest : public ::testing::Test {
  public:
    class MockBoardTranslator : public BoardTranslator {
      public:
        MOCK_METHOD(void, hasPieceRecognition, (bool pieceRecognition), (override));
        MOCK_METHOD(void, translate, (std::vector<CertaboPiece> const& board), (override));
        MOCK_METHOD(void, translateOccupiedSquares, ((std::array<bool, 64> const& occupied)), (override));
    };

  protected:
    void SetUp() override {
        translator.hasPieceRecognition(true);
        parser = std::make_unique<CertaboParser>(translator);
    }

    void givenParseIsCalledWith(std::string const& str) {
        whenParseIsCalledWith(str);
    }

    void whenParseIsCalledWith(std::string const& str) {
        std::vector<uint8_t> data(str.begin(), str.end());
        parser->parse(&data.front(), data.size());
    }

    void expectHasPieceRecognitionIsCalled() {
        EXPECT_CALL(translator, hasPieceRecognition(true)).Times(1);
    }

    void expectTranslateToBeCalled(int times = 1) {
        EXPECT_CALL(translator, translate(_)).Times(times);
    }

    void expectTranslateOccupiedSquaresToBeCalled(int times = 1) {
        EXPECT_CALL(translator, translateOccupiedSquares(_)).Times(times);
    }

    void expectTranslateOccupiedSquaresToBeCalledWith(std::array<bool, 64> const& expected) {
        EXPECT_CALL(translator, translateOccupiedSquares(expected)).Times(1);
    }

    void expectNoCallsToTranslate() {
        expectTranslateToBeCalled(0);
    }

  private:
    testing::NiceMock<MockBoardTranslator> translator;
    std::unique_ptr<CertaboParser> parser;
};

TEST_F(CertaboParserTest, hasPieceRecognitionAndTranslateAreCalledOnce) {
    expectHasPieceRecognitionIsCalled();
    expectTranslateToBeCalled();
    whenParseIsCalledWith(":3 0 84 252 153 251 196 0 47 128 3 0 84 240 106 3 0 83 177 224 3\n"
                          " 0 84 107 52 3 0 84 2 3 152 11 175 70 64 3 0 84 255 174 3 0 84 4\n"
                          "4 81 3 0 84 121 210 3 0 84 242 13 3 0 84 78 193 0 0 0 0 0 3 0 84\n"
                          " 240 84 3 0 84 240 65 3 0 84 68 134 0 0 0 0 0 0 0 0 0 0 3 0 85 0\n"
                          " 104 134 15 159 4 129 0 0 0 0 0 3 0 85 0 107 0 0 0 0 0 8 12 216 \n"
                          "32 32 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 3 0 84 107 56 176 \n"
                          "49 208 124 0 80 31 33 199 0 0 0 0 0 0 239 26 167 167 192 220 198\n"
                          " 252 184 16 246 252 243 126 4 0 0 0 0 0 3 0 84 242 11 32 0 0 60 \n"
                          "207 0 0 0 0 0 3 0 84 252 170 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0\n"
                          " 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 3 0 84 44 165 3 0 8\n"
                          "4 68 112 3 0 85 1 184 3 0 84 78 209 0 0 0 0 0 3 0 84 78 216 3 0 \n"
                          "84 237 98 0 0 0 0 0 3 0 85 0 16 3 0 83 229 13 3 0 85 0 67 3 0 84\n"
                          " 121 142 3 0 84 105 128 3 0 84 106 231 3 0 84 247 87 3 0 84 252 \n"
                          "15 \r\n");
}

TEST_F(CertaboParserTest, parseInvalid) {
    expectNoCallsToTranslate();
    whenParseIsCalledWith(":3 0 3XXXX8 252 153 196 3 166 0 65 3 0 84 2 3 3 0 83 177 224 3 0 84 \n"
                          "107 52 3 0 84 240 106 14 142 7 225 207 3 0 84 255 174 3 0 84 44 \n"
                          "81 3 0 84 121 210 3 0 84 242 13 3 0 84 78 193 3 0 84 107 56 3 0 \n"
                          "84 240 84 3 0 84 240 65 3 0 84 68 134 0 0 0 0 0 0 0 0 0 0 3 0 85\n"
                          " 0 104 223 138 7 0 0 0 0 0 0 0 3 0 85 0 107 0 0 0 0 0 128 51 96 \n"
                          "3 4 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 83 4 2 60 \n"
                          "231 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 128 55 111 195 164 1\n"
                          "42 128 160 240 1 0 0 0 0 0 216 16 16 48 212 0 0 0 0 0 3 0 84 252\n"
                          " 15 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 \n"
                          "0 0 0 0 0 0 0 0 0 0 3 0 84 242 11 3 0 84 68 112 3 0 85 1 184 3 0\n"
                          " 84 78 209 3 0 84 44 165 3 0 84 78 216 3 0 84 237 98 0 0 0 0 0 3\n"
                          " 0 85 0 16 3 0 83 229 13 3 0 84 106 231 3 0 84 121 142 3 0 84 10\n"
                          "5 128 3 0 85 0 67 3 0 84 247 87 3 0 84 252 17\n"
                          "0 \r\n");
}

TEST_F(CertaboParserTest, translateIsCalledTwiceForTwoDifferentPositions) {
    expectTranslateToBeCalled(2);
    whenParseIsCalledWith(":3 0 84 252 153 251 196 0 47 128 3 0 84 240 106 3 0 83 177 224 3\n"
                          " 0 84 107 52 3 0 84 2 3 152 11 175 70 64 3 0 84 255 174 3 0 84 4\n"
                          "4 81 3 0 84 121 210 3 0 84 242 13 3 0 84 78 193 0 0 0 0 0 3 0 84\n"
                          " 240 84 3 0 84 240 65 3 0 84 68 134 0 0 0 0 0 0 0 0 0 0 3 0 85 0\n"
                          " 104 134 15 159 4 129 0 0 0 0 0 3 0 85 0 107 0 0 0 0 0 8 12 216 \n"
                          "32 32 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 3 0 84 107 56 176 \n"
                          "49 208 124 0 80 31 33 199 0 0 0 0 0 0 239 26 167 167 192 220 198\n"
                          " 252 184 16 246 252 243 126 4 0 0 0 0 0 3 0 84 242 11 32 0 0 60 \n"
                          "207 0 0 0 0 0 3 0 84 252 170 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0\n"
                          " 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 3 0 84 44 165 3 0 8\n"
                          "4 68 112 3 0 85 1 184 3 0 84 78 209 0 0 0 0 0 3 0 84 78 216 3 0 \n"
                          "84 237 98 0 0 0 0 0 3 0 85 0 16 3 0 83 229 13 3 0 85 0 67 3 0 84\n"
                          " 121 142 3 0 84 105 128 3 0 84 106 231 3 0 84 247 87 3 0 84 252 \n"
                          "15 \r\n"
                          "L\r\n"
                          "\r\n"
                          ":3 0 84 252 153 12 6 128 231 0 3 0 84 240 106 3 0 83 177 224 3 0\n"
                          " 84 107 52 3 0 84 2 3 0 0 0 0 0 3 0 84 255 174 3 0 84 44 81 3 0 \n"
                          "84 121 210 3 0 84 242 13 3 0 84 78 193 229 151 161 128 64 3 0 84\n"
                          " 240 84 3 0 84 240 65 3 0 84 68 134 223 162 32 187 67 47 230 96 \n"
                          "55 4 3 0 85 0 104 0 0 0 0 0 0 0 0 0 0 3 0 85 0 107 0 0 0 0 0 248\n"
                          " 188 143 128 0 221 151 217 128 243 111 27 163 4 1 197 97 181 0 0\n"
                          " 171 190 226 0 132 3 0 84 107 56 247 197 97 6 136 123 50 40 32 2\n"
                          "1 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 232 31 4 76 0 0 0 0 0 0 3 0 84 2\n"
                          "42 11 243 32 0 128 1 0 0 0 0 0 3 0 84 252 170 0 0 0 0 0 0 0 0 0 \n"
                          "0 94 240 0 11 210 188 218 14 112 7 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0\n"
                          " 0 0 0 0 0 3 0 84 44 165 3 0 84 68 112 3 0 85 1 184 3 0 84 78 20\n"
                          "9 254 152 1 224 80 3 0 84 78 216 3 0 84 237 98 131 48 0 0 0 3 0 \n"
                          "85 0 16 3 0 83 229 13 3 0 85 0 67 3 0 84 121 142 3 0 84 105 128 \n"
                          "3 0 84 106 231 3 0 84 247 87 3 0 84 252 15 \n");
}

TEST_F(CertaboParserTest, parsePositionWithJunkInFront) {
    expectTranslateToBeCalled();
    whenParseIsCalledWith(
        "xxxxxxxxxxxxxxxxxx:3 0 84 252 153 3 0 85 0 104 3 0 84 2 3 3 0 83 177 224 3 0 84 107 52 3 0 84 240 106 "
        "3 0 85 0 107 3 0 84 255 174 3 0 84 44 81 3 0 84 121 210 3 0 84 242 13 3 0 84 107 56 3 0 84 78 193 "
        "3 0 84 240 84 3 0 84 240 65 3 0 84 68 134 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 160 80 "
        "7 140 126 32 250 15 0 0 254 7 118 237 181 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 "
        "0 160 225 80 192 121 0 0 0 0 0 207 224 74 7 172 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 3 0 85 1 184 0 0 0 0 0 "
        "0 0 0 0 0 100 115 213 250 161 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 186 10 56 165 201 0 0 0 0 0 168 "
        "94 211 7 40 74 124 195 174 25 3 0 84 44 165 3 0 84 68 112 3 0 84 237 98 3 0 84 252 170 0 0 0 0 0 3 0 "
        "84 78 209 3 0 84 242 11 3 0 84 78 216 3 0 85 0 16 3 0 83 229 13 3 0 85 0 67 3 0 84 121 142 3 0 84 105 "
        "128 3 0 84 106 231 3 0 84 247 87 3 0 84 252 15\r\n");
}

TEST_F(CertaboParserTest, parsePositionInTwoParts) {
    expectTranslateToBeCalled();
    givenParseIsCalledWith(":3 0 84 252 153 3 0 85 0 104 3 0 84 240 106 3 0 83 177 224 3 0 8\n"
                           "4 107 52 3 0 84 2 3 3 0 85 0 107 3 0 84 255 174 3 0 84 44 81 3 0\n"
                           " 84 121 210 3 0 84 242 13 3 0 84 78 193 3 0 84 107 56 3 0 84 240\n"
                           " 84 3 0 84 240 65 3 0 84 68 134 252 64 88 0 32 0 0 0 0 0 0 0 0 0\n"
                           " 0 0 0 0 0 0 239 108 5 136 0 243 32 12 0 240 0 0 0 0 0 251 13 24\n"
                           "1 4 192 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 221 12\n"
                           " 87 1 12 255 194 176 204 0 0 0 0 0 0 226 2 224 8 96 143 182 230 \n"
                           "16 5 120 132 14 0 16 0 0 0 0 0 204 146 3 24 0 231 0 20 3 3 0 0 0\n"
                           " 0 0 121 129 0 5 0 0 0 0 0 0 0 0 0 0 0 112 177 230 48 0 0 0 0 0 \n"
                           "0 0 0 0 0 0 0 0 0 0 0 239 134 225 66 240 0 0 0 0 0 3 0 84 44 165\n"
                           " 3 0 84 68 112 3 0 85 1 184 3 0 84 78 209 3 0 84 242 11 3 0 84 7\n"
                           "8 216 3 0 84 237 98 3 0 84 252 15 3 0 85 0 16 3 0 83 229 13 3 0 \n"
                           "85 0 67 3 0 84 121 142 3 0 84 105 128 3 0 84 106 231 3 0 84 247 \n"
                           "87 3 0 84 252 17");
    whenParseIsCalledWith("0\nL\r\n");
}

TEST_F(CertaboParserTest, parsePositionTabutronic) {
    expectTranslateOccupiedSquaresToBeCalled();
    whenParseIsCalledWith(":255 255 0 0 0 0 255 255\r\n");
}

TEST_F(CertaboParserTest, parsePositionTabutronicOnePieceMissing) {
    std::array<bool, 64> board{
        true,  true,  true,  true,  true,  true,  true,  true,  //
        true,  true,  true,  true,  true,  true,  true,  true,  //
        false, false, false, false, false, false, false, false, //
        false, false, false, false, false, false, false, false, //
        false, false, false, false, false, false, false, false, //
        false, false, false, false, false, false, false, false, //
        true,  true,  true,  true,  true,  true,  true,  true,  //
        true,  true,  true,  true,  true,  true,  true,  false, //
    };
    expectTranslateOccupiedSquaresToBeCalledWith(board);
    whenParseIsCalledWith(":255 255 0 0 0 0 255 254\nL\r\n");
}
