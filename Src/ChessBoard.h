#pragma once

#include "ChessPiece.h"

namespace ChessProj
{

class CChessBoard
{
public:
    CChessBoard();

    const CChessPiece * GetPieces() const;

    void Initialize(const CChessPiece::Color bottomColor);

private:
    CChessPiece         m_Pieces[8][8];
    CChessPiece::Color  m_BottomColor = CChessPiece::Color::White;
};

} // namespace ChessProj