#include "CertaboPiece.h"

using eboard::CertaboPiece;

CertaboPiece::CertaboPiece(eboard::PieceId const &pieceId) : pieceId(pieceId) {}

eboard::PieceId& CertaboPiece::getId() {
    return pieceId;
}
