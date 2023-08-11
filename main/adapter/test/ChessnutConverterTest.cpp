#include <iomanip>
#include <sstream>

#include <gmock/gmock.h>

#include "CertaboCalibrator.h"
#include "ChessnutConverter.h"

using eboard::ChessnutConverter;

class ChessnutConverterTest : public ::testing::Test {
  protected:
    void SetUp() override {
        converter = std::make_unique<ChessnutConverter>(
            [this](uint8_t* data, size_t data_len) {
                convertedBoard = std::vector<uint8_t>(&data[0], &data[data_len]);
            },
            [this](uint8_t* data, size_t data_len) {
                convertedInfo = std::vector<uint8_t>(&data[0], &data[data_len]);
            });
        whenChessnutToCertaboCommandIsCalledWith({0x21, 0x01, 0x00}); // enable real-time mode
    }

    void whenConvertingBoard(std::array<eboard::StoneId, 64> const& board) {
        converter->process(board);
    }

    void whenChessnutToCertaboCommandIsCalledWith(std::vector<uint8_t> data) {
        convertedCommand = converter->chessnutToCertaboCommand(&data.front(), data.size());
    }

    void thenInfoCallbackShouldBeCalledWith(std::vector<uint8_t> const& expected) {
        EXPECT_EQ(convertedInfo, expected) << "expected: " << toHex(expected.data(), expected.size()) << std::endl
                                           << "converted:  " << toHex(convertedInfo.data(), convertedInfo.size());
    }

    void thenSizeOfBoardCallbackShouldBe(int expectedSize) {
        EXPECT_EQ(convertedBoard.size(), expectedSize)
            << "converted:  " << toHex(convertedBoard.data(), convertedBoard.size());
    }

    void thenSizeOfInfoCallbackShouldBe(int expectedSize) {
        EXPECT_EQ(convertedInfo.size(), expectedSize)
            << "converted:  " << toHex(convertedInfo.data(), convertedInfo.size());
    }

    void thenConvertedCommandShouldBe(std::vector<uint8_t> const& expected) {
        EXPECT_EQ(convertedCommand, expected)
            << "expected: " << toHex(expected.data(), expected.size()) << std::endl
            << "command:  " << toHex(convertedCommand.data(), convertedCommand.size());
    }

    void thenBoardCallbackShouldBeCalledStartingWith(std::vector<uint8_t> const& expected) {
        std::vector<uint8_t> converted(convertedBoard.begin(), convertedBoard.begin() + expected.size());
        EXPECT_EQ(converted, expected) << "expected:  " << toHex(expected.data(), expected.size()) << std::endl
                                       << "converted: " << toHex(converted.data(), converted.size());
    }

    void thenInfoCallbackShouldBeCalledStartingWith(std::vector<uint8_t> const& expected) {
        std::vector<uint8_t> converted(convertedInfo.begin(), convertedInfo.begin() + expected.size());
        EXPECT_EQ(converted, expected) << "expected:  " << toHex(expected.data(), expected.size()) << std::endl
                                       << "converted: " << toHex(converted.data(), converted.size());
    }

  private:
    std::string toHex(unsigned const char* data, int len) {
        std::stringstream ss;
        ss << std::hex;
        for (int i = 0; i < len; ++i) {
            ss << std::setw(2) << std::setfill('0') << (int)data[i];
        }
        return ss.str();
    }

    std::unique_ptr<ChessnutConverter> converter;
    std::vector<uint8_t> convertedCommand;
    std::vector<uint8_t> convertedBoard;
    std::vector<uint8_t> convertedInfo;
};

TEST_F(ChessnutConverterTest, convertInitialPosition) {
    whenConvertingBoard({                                        //
                         2,   3,   4,   5,   6,   4,   3,   2,   //
                         1,   1,   1,   1,   1,   1,   1,   1,   //
                         0,   0,   0,   0,   0,   0,   0,   0,   //
                         0,   0,   0,   0,   0,   0,   0,   0,   //
                         0,   0,   0,   0,   0,   0,   0,   0,   //
                         0,   0,   0,   0,   0,   0,   0,   0,   //
                         129, 129, 129, 129, 129, 129, 129, 129, //
                         130, 131, 132, 133, 134, 132, 131, 130});
    std::vector<uint8_t> expected{
        0x01, 0x24,                                     //
        0x58, 0x23, 0x31, 0x85, 0x44, 0x44, 0x44, 0x44, //
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, //
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, //
        0x77, 0x77, 0x77, 0x77, 0xA6, 0xC9, 0x9B, 0x6A, //
    };
    thenBoardCallbackShouldBeCalledStartingWith(expected);
    thenSizeOfBoardCallbackShouldBe(38);
}

TEST_F(ChessnutConverterTest, realTimeMode) {
    whenChessnutToCertaboCommandIsCalledWith({0x21, 0x01, 0x00});
    thenInfoCallbackShouldBeCalledWith({0x23, 0x01, 0x00}); // ack
}

TEST_F(ChessnutConverterTest, uploadMode) {
    whenChessnutToCertaboCommandIsCalledWith({0x21, 0x01, 0x01});
    thenInfoCallbackShouldBeCalledWith({0x23, 0x01, 0x00}); // ack
}

TEST_F(ChessnutConverterTest, batteryStatus) {
    whenChessnutToCertaboCommandIsCalledWith({0x29, 0x01, 0x00});
    thenInfoCallbackShouldBeCalledWith({0x2a, 0x02, 0x64, 0x00}); // battery full, not loading
}

TEST_F(ChessnutConverterTest, filesCount) {
    whenChessnutToCertaboCommandIsCalledWith({0x31, 0x01, 0x00});
    thenInfoCallbackShouldBeCalledWith({0x32, 0x01, 0x00}); // zero files
}

TEST_F(ChessnutConverterTest, ledCommand_e2e4) {
    whenChessnutToCertaboCommandIsCalledWith({0x0A, 0x08, 0x00, 0x00, 0x00, 0x00, 0x08, 0x00, 0x08, 0x00});
    thenConvertedCommandShouldBe({0x00, 0x00, 0x00, 0x00, 0x10, 0x00, 0x10, 0x00}); // reversed bits for Certabo
}

TEST_F(ChessnutConverterTest, callbackForLedCommand_e2e4) {
    whenChessnutToCertaboCommandIsCalledWith({0x0A, 0x08, 0x00, 0x00, 0x00, 0x00, 0x08, 0x00, 0x08, 0x00});
    thenInfoCallbackShouldBeCalledWith({0x23, 0x01, 0x00}); // ack
}

TEST_F(ChessnutConverterTest, requestDateTime) {
    whenChessnutToCertaboCommandIsCalledWith({0x26, 0x01, 0x00});
    thenInfoCallbackShouldBeCalledStartingWith({0x2d, 0x04});
    thenSizeOfInfoCallbackShouldBe(6);
}
