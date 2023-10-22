#pragma once

#include <array>
#include <vector>

#include "CertaboPiece.h"

namespace eboard {

class BoardTranslator {
  public:
    BoardTranslator() = default;
    virtual ~BoardTranslator() = default;

  public:
    virtual void hasPieceRecognition(bool canRecognize) = 0;

    virtual void translate(std::vector<CertaboPiece> const& board) = 0;

    virtual void translateOccupiedSquares(std::array<bool, 64> const& occupied) = 0;
};

} // namespace eboard