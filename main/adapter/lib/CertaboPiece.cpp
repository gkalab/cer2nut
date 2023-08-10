#include "CertaboPiece.h"

using eboard::CertaboPiece;

CertaboPiece::CertaboPiece(eboard::PieceId pieceId) : pieceId(pieceId) {}

eboard::PieceId& CertaboPiece::getId() {
    return pieceId;
}
