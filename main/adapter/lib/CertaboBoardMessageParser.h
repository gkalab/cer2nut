#pragma once

#include <array>
#include <cstdint>
#include <list>
#include <vector>

#include "BoardTranslator.h"
#include "CertaboCalibrator.h"
#include "CertaboParser.h"

namespace eboard {

using CallbackFunction = std::function<void(std::array<StoneId, 64>)>;

/**
 * CertaboBoardMessageParser translates raw board data to the internal board representation.
 * It uses the stone IDs from a calibrator to map the raw IDs to stones.
 */
class CertaboBoardMessageParser : public BoardTranslator {
  public:
    CertaboBoardMessageParser(Stones, CallbackFunction callbackFunction);
    ~CertaboBoardMessageParser() override = default;

  public:
    void hasPieceRecognition(bool canRecognize) override;
    void translate(std::vector<CertaboPiece> board) override;

    void parse(uint8_t* msg, size_t data_len);

  private:
    void averageLastBoards(std::array<StoneId, 64> newBoard);
    /**
     * Returns the element with the highest frequency in a vector.
     * implementation from https://devptr.com/find-most-frequent-element-in-vector-in-c/
     * @param entries
     * @return
     */
    static int mostFrequent(std::vector<StoneId>& entries);
    static int toSquare(int index);

    bool pieceRecognition = false;
    CertaboParser parser;
    Stones stones;
    CallbackFunction callback;
    std::list<std::array<StoneId, 64>> boardHistory;
};

} // namespace eboard