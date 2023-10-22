#pragma once

#include <array>
#include <cstdint>
#include <list>
#include <vector>

#include "BoardTranslator.h"
#include "CertaboCalibrator.h"
#include "CertaboParser.h"
#include "Sentio.h"

namespace eboard {

using CallbackFunction = std::function<void(std::array<StoneId, 64> const&)>;
using PieceRecognitionCallbackFunction = std::function<void(bool hasPieceRecognition)>;

/**
 * CertaboBoardMessageParser translates raw board data to the internal board representation.
 * It uses the stone IDs from a calibrator to map the raw IDs to stones.
 */
class CertaboBoardMessageParser : public BoardTranslator {
  public:
    CertaboBoardMessageParser(CallbackFunction callbackFunction,
                              PieceRecognitionCallbackFunction pieceRecognitionCallbackFunction);
    ~CertaboBoardMessageParser() override = default;

  public:
    void hasPieceRecognition(bool canRecognize) override;
    void translate(std::vector<CertaboPiece> const& board) override;
    void translateOccupiedSquares(std::array<bool, 64> const& occupied) override;

    void parse(uint8_t* msg, size_t data_len);
    void updateStones(Stones const& newStones);

  private:
    void averageLastBoards(std::array<StoneId, 64> const &newBoard);
    /**
     * Returns the element with the highest frequency in a vector.
     * implementation from https://devptr.com/find-most-frequent-element-in-vector-in-c/
     * @param entries
     * @return
     */
    static int mostFrequent(std::vector<StoneId>& entries);
    static int toSquare(int index);

    CertaboParser parser;
    Stones stones;
    CallbackFunction callback;
    Sentio sentio;
    PieceRecognitionCallbackFunction pieceRecognitionCallback;
    std::list<std::array<StoneId, 64>> boardHistory;
};

} // namespace eboard