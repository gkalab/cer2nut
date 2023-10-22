#include <utility>

#include "ChessData.h"
#include "ChessnutAdapter.h"

using eboard::ChessnutAdapter;

std::array<eboard::StoneId, 64> const ChessnutAdapter::STANDARD_POSITION{2,   3,   4,   5,   6,   4,   3,   2,   //
                                                                         1,   1,   1,   1,   1,   1,   1,   1,   //
                                                                         0,   0,   0,   0,   0,   0,   0,   0,   //
                                                                         0,   0,   0,   0,   0,   0,   0,   0,   //
                                                                         0,   0,   0,   0,   0,   0,   0,   0,   //
                                                                         0,   0,   0,   0,   0,   0,   0,   0,   //
                                                                         129, 129, 129, 129, 129, 129, 129, 129, //
                                                                         130, 131, 132, 133, 134, 132, 131, 130};

ChessnutAdapter::ChessnutAdapter(ToUsbFunction toUsb, ToBleFunction toBle)
    : calibrationLeds({0xff, 0xff, 0x08, 0, 0, 0x08, 0xff, 0xff}), ledControl(std::move(toUsb)), toBle(std::move(toBle)),
      boardMessageParser(
          [this](std::array<eboard::StoneId, 64> const &board) {
              if (!initialPositionReceived && board == STANDARD_POSITION) {
                  initialPositionReceived = true;
                  if (!pieceRecognition) {
                      lightCenterLeds();
                  }
              }
              if (initialPositionReceived) {
                  converter.process(board);
              } else if (!pieceRecognition) {
                  calibrationLeds = {0xff, 0xff, 0, 0, 0, 0, 0xff, 0xff};
                  for (int square = 0; square < 64; square++) {
                      if (board.at(square) != ChessData::NO_STONE) {
                          clearBitForSquare(calibrationLeds, square);
                      }
                  }
                  ledCommand(calibrationLeds);
              }
          },
          [this](bool hasPieceRecognition) {
              pieceRecognition = hasPieceRecognition;
              if (hasPieceRecognition) {
                  calibrationLeds = {0xff, 0xff, 0x08, 0, 0, 0x08, 0xff, 0xff};
                  ledCommand(calibrationLeds);
              } else {
                  ledControl.setProcessingTime(200);
              }
          }),
      calibrator(
          [this](eboard::Stones const& stones) {
              boardMessageParser.updateStones(stones);
              calibrationComplete = true;
              lightCenterLeds();
          },
          [this](int square) {
              clearBitForSquare(calibrationLeds, square);
              ledCommand(calibrationLeds);
          }),
      converter(
          [this](uint8_t* data, size_t data_len) {
              this->toBle(data, data_len, true);
          },
          [this](uint8_t* data, size_t data_len) {
              this->toBle(data, data_len, false);
          }) {
    ledCommand(calibrationLeds);
}

void ChessnutAdapter::lightCenterLeds() {
    std::vector<uint8_t> centerLeds{0, 0, 0, 0x18, 0x18, 0, 0, 0};
    ledCommand(centerLeds);
}

void ChessnutAdapter::clearBitForSquare(std::vector<uint8_t>& data, int square) {
    int row = 7 - square / 8;
    int col = 7 - square % 8;
    int pos = (row * 8 + col);
    int posByte = pos / 8;
    int posBit = pos % 8;
    uint8_t oldByte = data[posByte];
    oldByte = (uint8_t)(((0xFF7F >> posBit) & oldByte) & 0x00FF);
    uint8_t newByte = ((0 << (8 - (posBit + 1))) | oldByte);
    data[posByte] = newByte;
}

void ChessnutAdapter::fromUsb(uint8_t* data, size_t data_len) {
    if (!calibrationComplete && pieceRecognition) {
        calibrator.calibrate(data, data_len);
    } else {
        boardMessageParser.parse(data, data_len);
    }
}

void ChessnutAdapter::fromBle(uint8_t* data, size_t data_len) {
    std::vector<uint8_t> result = converter.chessnutToCertaboCommand(data, data_len);
    if (!result.empty() && isReady()) {
        ledCommand(result);
    }
}

void eboard::ChessnutAdapter::ledCommand(std::vector<uint8_t> const& command) {
    ledControl.ledCommand(command);
}

bool ChessnutAdapter::isReady() const {
    return calibrationComplete || initialPositionReceived;
}
