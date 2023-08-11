#include <gmock/gmock.h>

#include <string>
#include <vector>

#include "ChessnutAdapter.h"

using ::testing::AtLeast;

using eboard::ChessnutAdapter;

static std::string boardDataWithQueens(
    ":48 0 248 71 99 48 0 248 85 159 48 0 177 203 192 48 0 177 215 17 48 0 177 117 59 48 0 177 43 7 48 0 248 "
    "222 81 48 0 247 200 86 48 0 248 114 180 48 0 248 155 251 48 0 248 48 74 48 0 177 236 131 48 0 177 230 12 "
    "48 0 177 187 36 48 0 248 146 97 48 0 248 89 231 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 16 218 88 139 184 0 0 0 0 0 0 0 0 "
    "0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 "
    "0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 "
    "0 0 0 3 1 84 252 15 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 48 0 248 85 122 48 0 248 68 117 48 0 248 201 109 "
    "48 0 248 144 65 48 0 177 231 217 48 0 248 76 179 48 0 248 161 89 48 0 94 124 14 48 0 248 98 180 48 0 248 "
    "233 43 48 0 248 86 247 48 0 248 145 6 48 0 248 104 144 48 0 248 79 194 48 0 248 134 85 48 0 177 81 "
    "73\r\n");

static std::string boardDataWithoutQueens(
    ":48 0 248 71 99 48 0 248 85 159 48 0 177 203 192 48 0 177 215 17 48 0 177 117 59 48 0 177 43 7 48 0 248 "
    "222 81 48 0 247 200 86 48 0 248 114 180 48 0 248 155 251 48 0 248 48 74 48 0 177 236 131 48 0 177 230 12 "
    "48 0 177 187 36 48 0 248 146 97 48 0 248 89 231 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 "
    "0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 "
    "0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 "
    "0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 48 0 248 85 122 48 0 248 68 117 48 0 248 201 109 "
    "48 0 248 144 65 48 0 177 231 217 48 0 248 76 179 48 0 248 161 89 48 0 94 124 14 48 0 248 98 180 48 0 248 "
    "233 43 48 0 248 86 247 48 0 248 145 6 48 0 248 104 144 48 0 248 79 194 48 0 248 134 85 48 0 177 81 "
    "73\r\n");

class ChessnutAdapterTest : public ::testing::Test {
  protected:
    void SetUp() override {
        adapter = std::make_unique<ChessnutAdapter>(
            [](uint8_t* data, size_t data_len) {
                // toUsb
            },
            [this](uint8_t* data, size_t data_len, bool) {
                // toBle
                toBleData = std::vector<uint8_t>(&data[0], &data[data_len]);
            });
        // init with real time mode
        std::vector<uint8_t> realTimeMode{0x21, 0x01, 0x00};
        adapter->fromBle(&realTimeMode.front(), realTimeMode.size());
        toBleData.clear();
    }

    void givenCalibrationDataIsReceived() {
        std::vector<uint8_t> data(boardDataWithQueens.begin(), boardDataWithQueens.end());
        for (int i = 0; i < 7; i++) {
            adapter->fromUsb(&data.front(), data.size());
        }
    }

    void whenBoardDataWithoutQueensIsReceivedOnce() {
        std::vector<uint8_t> data(boardDataWithoutQueens.begin(), boardDataWithoutQueens.end());
        adapter->fromUsb(&data.front(), data.size());
    }

    void whenCalibrationPositionWithQueensIsReceivedOnce() {
        std::vector<uint8_t> data(boardDataWithQueens.begin(), boardDataWithQueens.end());
        adapter->fromUsb(&data.front(), data.size());
    }

    void thenToBleShouldBeCalledStartingWith(std::vector<uint8_t> const& expected) {
        ASSERT_GT(toBleData.size(), 0);
        std::vector<uint8_t> converted(toBleData.begin(), toBleData.begin() + expected.size());
        EXPECT_EQ(converted, expected) << "expected:  " << toHex(expected.data(), expected.size()) << std::endl
                                       << "toBleData: " << toHex(converted.data(), converted.size());
    }

    void thenToBleShouldNotBeCalled() {
        EXPECT_EQ(toBleData.size(), 0);
    }

  private:
    std::unique_ptr<ChessnutAdapter> adapter;
    std::vector<uint8_t> toBleData;

    std::string toHex(unsigned const char* data, int len) {
        std::stringstream ss;
        ss << std::hex;
        for (int i = 0; i < len; ++i) {
            ss << std::setw(2) << std::setfill('0') << (int)data[i];
        }
        return ss.str();
    }
};

TEST_F(ChessnutAdapterTest, initialPositionFromUsb) {
    givenCalibrationDataIsReceived();
    whenBoardDataWithoutQueensIsReceivedOnce();
    thenToBleShouldBeCalledStartingWith({
        0x01, 0x24,                                     //
        0x58, 0x23, 0x31, 0x85, 0x44, 0x44, 0x44, 0x44, //
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, //
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, //
        0x77, 0x77, 0x77, 0x77, 0xA6, 0xC9, 0x9B, 0x6A, //
    });
}

TEST_F(ChessnutAdapterTest, calibrationPositionFromUsb) {
    givenCalibrationDataIsReceived();
    whenCalibrationPositionWithQueensIsReceivedOnce();
    thenToBleShouldNotBeCalled();
}
