#pragma once

#include <vector>

#include "CertaboPiece.h"

namespace eboard {

class BoardTranslator {
  public:
    BoardTranslator() = default;
    virtual ~BoardTranslator() = default;

  public:
    virtual void hasPieceRecognition(bool canRecognize) = 0;

    virtual void translate(std::vector<CertaboPiece> board) = 0;
};

} // namespace eboard