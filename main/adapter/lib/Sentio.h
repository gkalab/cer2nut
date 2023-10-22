#pragma once

#include <array>
#include <functional>
#include <memory>

#include "CapturePiece.h"
#include "CertaboCalibrator.h"
#include "Chess0x88.h"

namespace eboard {

using BoardCallbackFunction = std::function<void(std::array<StoneId, 64> const&)>;

class Sentio {
  public:
    static uint64_t const MIN_TIME_TO_PROCESS_MS = 300;

    explicit Sentio(BoardCallbackFunction callbackFunction);

    Sentio(BoardCallbackFunction callbackFunction, chess::Chess0x88 initialBoard);

    void occupiedSquares(std::array<bool, 64> const& occupied);

  private:
    static const std::vector<uint8_t> SQUARES_INITIAL_POSITION;
    static const std::map<int, int> PIECE_TO_STONE_ID;

    void processOccupiedSquares(const std::array<bool, 64>& occupied);
    static std::vector<uint8_t> toSquares(const std::array<bool, 64>& occupied);
    void callCallback(std::array<StoneId, 64> const& boardArray);
    static std::array<StoneId, 64> toBoardArray(chess::Chess0x88& chessBoard);
    bool takeBackMove(std::vector<uint8_t> const &occupiedSquares);
    void checkValidMove(std::vector<uint8_t> const& expectedSquares, std::vector<uint8_t> const& occupiedSquares);
    bool nonCaptureMove(std::vector<uint8_t> const& missing, std::vector<uint8_t> const& extra);
    bool captureMove(std::vector<uint8_t> const& missing, std::vector<uint8_t> const& extra);
    void incompleteMove(std::vector<uint8_t> const& missing, std::vector<uint8_t> const& extra);
    bool makeMove(uint8_t fromSquare, uint8_t toSquare);
    static bool isPossibleCapture(std::vector<uint8_t> const& missing, std::vector<uint8_t> const& extra);
    bool isPossibleEpCapture(std::vector<uint8_t> const& missing, std::vector<uint8_t> const& extra);

    BoardCallbackFunction callback;
    chess::Chess0x88 board;
    std::unique_ptr<CapturePiece> capturePiece;
    std::array<bool, 64> lastProcessedOccupiedSquares{};
    std::array<bool, 64> lastReceivedOccupiedSquares{};
    std::array<StoneId, 64> lastBoardArray{};
    uint64_t lastBoardSendTime = 0;
    uint64_t lastProcessTime = 0;
};

} // namespace eboard