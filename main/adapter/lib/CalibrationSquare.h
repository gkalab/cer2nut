#pragma once

#include <functional>
#include <vector>

#include "CertaboPiece.h"

namespace eboard {

using CalibrationCompleteForSquareFunction = std::function<void(int square)>;

class CalibrationSquare {
  public:
    explicit CalibrationSquare(int square);

    bool isCalibrated();
    void calibratePiece(std::vector<std::vector<CertaboPiece>>& receivedBoards,
                        CalibrationCompleteForSquareFunction const& completeForSquareFunction);
    int getStone();
    int getSquare() const;

    eboard::CertaboPiece getPiece();

  private:
    bool pieceOrExtraQueenSquare(CertaboPiece& pc) const;
    int noStoneOr(int stone);

    int square;
    CertaboPiece piece;
};

} // namespace eboard