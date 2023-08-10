#pragma once

#include <array>
#include <string>

namespace eboard {

using PieceId = std::array<uint8_t, 5>;

/**
 * CertaboPiece represents raw piece data from a Certabo position for one piece.
 */
class CertaboPiece {
  public:
    explicit CertaboPiece(std::array<uint8_t, 5> pieceId);

    PieceId& getId();

    friend bool operator<(const CertaboPiece& c1, const CertaboPiece& c2) {
        std::string c1id;
        for (int i = 0; i < 5; i++) {
            c1id += std::to_string(c1.pieceId[i]) + " ";
        }
        std::string c2id;
        for (int i = 0; i < 5; i++) {
            c2id += std::to_string(c2.pieceId[i]) + " ";
        }
        return c1id < c2id;
    }

  private:
    PieceId pieceId;
};

} // namespace eboard