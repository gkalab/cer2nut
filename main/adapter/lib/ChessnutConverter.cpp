#include <chrono>
#include <utility>

#include "ChessData.h"
#include "ChessnutConverter.h"

using eboard::ChessnutConverter;

ChessnutConverter::ChessnutConverter(ConverterCallbackFunction boardCallback, ConverterCallbackFunction infoCallback)
    : boardCallback(std::move(boardCallback)), infoCallback(std::move(infoCallback)) {}

static std::vector<uint8_t> dateTime() {
    std::vector<uint8_t> converted{};
    converted.reserve(4);
    uint32_t secondsSinceEpoch =
        std::chrono::duration_cast<std::chrono::seconds>(std::chrono::steady_clock::now().time_since_epoch()).count();
    converted.push_back(secondsSinceEpoch & 0x000000ff);
    converted.push_back((secondsSinceEpoch & 0x0000ff00) >> 8);
    converted.push_back((secondsSinceEpoch & 0x00ff0000) >> 16);
    converted.push_back((secondsSinceEpoch & 0xff000000) >> 24);
    return converted;
}

void ChessnutConverter::process(std::array<eboard::StoneId, 64> const& board) {
    if (realTimeMode) {
        std::array<uint8_t, 38> converted{};
        converted[0] = 0x01;
        converted[1] = 0x24;
        int j = 33;
        for (int i = 0; i < 64; i += 2) {
            converted[j] = setUpperNibble(converted[j], stoneToChessnutStone(board[i]));
            converted[j] = setLowerNibble(converted[j], stoneToChessnutStone(board[i + 1]));
            j--;
        }
        // add time
        std::vector<uint8_t> convertedSeconds = dateTime();
        converted[34] = convertedSeconds[0];
        converted[35] = convertedSeconds[1];
        converted[36] = convertedSeconds[2];
        converted[37] = convertedSeconds[3];
        boardCallback(converted.data(), 38);
    }
}

uint8_t ChessnutConverter::setLowerNibble(uint8_t orig, uint8_t nibble) {
    return (orig & 0xF0) | (nibble & 0xF);
}

uint8_t ChessnutConverter::setUpperNibble(uint8_t orig, uint8_t nibble) {
    return (orig & 0x0F) | ((nibble & 0xF) << 4);
}

uint8_t ChessnutConverter::stoneToChessnutStone(const eboard::StoneId stone) {
    std::map<eboard::StoneId, uint8_t> conversionMap{
        {ChessData::NO_STONE, 0},      {ChessData::WHITE_PAWN, 7},   {ChessData::WHITE_ROOK, 6},
        {ChessData::WHITE_KNIGHT, 10}, {ChessData::WHITE_BISHOP, 9}, {ChessData::WHITE_QUEEN, 11},
        {ChessData::WHITE_KING, 12},   {ChessData::BLACK_PAWN, 4},   {ChessData::BLACK_ROOK, 8},
        {ChessData::BLACK_KNIGHT, 5},  {ChessData::BLACK_BISHOP, 3}, {ChessData::BLACK_QUEEN, 1},
        {ChessData::BLACK_KING, 2},
    };
    if (conversionMap.find(stone) != conversionMap.end()) {
        return conversionMap[stone];
    } else {
        return 0;
    }
}

/**
 * Reverse the bits of a byte
 * 01000000 becomes 00000010
 * @param b input byte
 * @return byte with its bits reversed
 * Taken from https://graphics.stanford.edu/~seander/bithacks.html#ReverseByteWith64BitsDiv
 */
static uint8_t reverseBits(uint8_t b) {
    return (b * 0x0202020202ULL & 0x010884422010ULL) % 1023;
}

std::vector<uint8_t> eboard::ChessnutConverter::chessnutToCertaboCommand(uint8_t* data, size_t data_len) {
    std::vector<uint8_t> ack = std::vector<uint8_t>{0x23, 0x01, 0x00};

    std::vector<uint8_t> received(&data[0], &data[data_len]);
    if (received.size() >= 3 && received[0] == 0x21 && received[1] == 0x01 && received[2] == 0x00) { // real time mode
        infoCallback(ack.data(), ack.size());
        realTimeMode = true;
    } else if (received.size() >= 3 && received[0] == 0x21 && received[1] == 0x01 &&
               received[2] == 0x01) { // upload mode
        infoCallback(ack.data(), ack.size());
        realTimeMode = false;
    } else if (received.size() >= 3 && received[0] == 0x29 && received[1] == 0x01 &&
               received[2] == 0x00) {                               // battery status
        auto result = std::vector<uint8_t>{0x2a, 0x02, 0x64, 0x00}; // battery full, not loading
        infoCallback(result.data(), result.size());
    } else if (received.size() >= 3 && received[0] == 0x31 && received[1] == 0x01 &&
               received[2] == 0x00) {                         // files count
        auto result = std::vector<uint8_t>{0x32, 0x01, 0x00}; // zero files
        infoCallback(result.data(), result.size());
    } else if (received.size() >= 3 && received[0] == 0x26 && received[1] == 0x01 &&
               received[2] == 0x00) { // request date/time
        std::vector<uint8_t> seconds = dateTime();
        auto result = std::vector<uint8_t>{0x2d, 0x04, seconds[0], seconds[1], seconds[2], seconds[3]};
        infoCallback(result.data(), result.size());
    } else if (received.size() >= 3 && received[0] == 0x27 && received[1] == 0x01 &&
               received[2] == 0x00) { // request FW version
        auto result = std::vector<uint8_t>{0x28, 0x0d, 0x00, 0x43, 0x45, 0x52, 0x54, 0x41,
                                           0x42, 0x4f, 0x5f, 0x56, 0x31, 0x30, 0x30}; // CERTABO_V100
        infoCallback(result.data(), result.size());
    } else if (received.size() >= 6 && received[0] == 0x0b && received[1] == 0x04) { // ignore sound command
        infoCallback(ack.data(), ack.size());
    } else if (received.size() >= 10 && received[0] == 0x0A && received[1] == 0x08) { // LED command
        std::vector<uint8_t> result;
        result.reserve(8);
        for (int i = 0; i < 8; i++) {
            result.push_back(reverseBits(received[i + 2]));
        }
        infoCallback(ack.data(), ack.size());
        return result;
    }
    return {};
}
