#pragma once

#include "ChessPiece.h"

namespace ChessProj
{

struct CSquare
{
    CSquare(const int row = -1, const int col = -1);

    bool IsValid() const;

    bool operator==(const CSquare & other) const;
    bool operator!=(const CSquare & other) const;

    CSquare operator+(const CSquare & other) const;
    CSquare & operator+=(const CSquare & other);

    int m_Row = -1;
    int m_Col = -1;
};

class CChessBoard
{
public:
    CChessBoard();

    void Initialize(const CChessPiece::Color bottomColor);

    CChessPiece::Color GetBottomColor() const;

    const CChessPiece & GetPieceAtSquare(const CSquare & square) const;

    const CSquare & GetWhiteKingPos() const;
    const CSquare & GetBlackKingPos() const;

    void SetPieceAtSquare(const CChessPiece & piece, const CSquare & square);

private:
    CChessPiece         m_Pieces[8][8];
    CChessPiece::Color  m_BottomColor = CChessPiece::Color::White;

    CSquare             m_WhiteKingPos;
    CSquare             m_BlackKingPos;
};

} // namespace ChessProj