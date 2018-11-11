#include "ChessBoard.h"

#include <algorithm>

namespace ChessProj
{

// CSquare

CSquare::CSquare(const int row /*= -1*/, const int col /*= -1*/)
    : m_Row(row)
    , m_Col(col)
{
}

bool CSquare::IsValid() const
{
    return m_Row > -1 && m_Row < 8 && m_Col > -1 && m_Col < 8;
}

bool CSquare::operator==(const CSquare & other) const
{
    return other.m_Row == m_Row && other.m_Col == m_Col;
}

bool CSquare::operator!=(const CSquare & other) const
{
    return !(*this == other);
}

CSquare CSquare::operator+(const CSquare & other) const
{
    CSquare result;

    result.m_Row = m_Row + other.m_Row;
    result.m_Col = m_Col + other.m_Col;

    return result;
}

CSquare & CSquare::operator+=(const CSquare & other)
{
    m_Row += other.m_Row;
    m_Col += other.m_Col;

    return *this;
}

// CChessBoard

CChessBoard::CChessBoard()
{
    Initialize(CChessPiece::Color::White);
}

void CChessBoard::Initialize(const CChessPiece::Color bottomColor)
{
    m_BottomColor = bottomColor;

    for (int row : {0, 7})
    {
        m_Pieces[row][0].SetType(CChessPiece::Type::Rook);
        m_Pieces[row][1].SetType(CChessPiece::Type::Knight);
        m_Pieces[row][2].SetType(CChessPiece::Type::Bishop);
        m_Pieces[row][3].SetType(CChessPiece::Type::Queen);
        m_Pieces[row][4].SetType(CChessPiece::Type::King);
        m_Pieces[row][5].SetType(CChessPiece::Type::Bishop);
        m_Pieces[row][6].SetType(CChessPiece::Type::Knight);
        m_Pieces[row][7].SetType(CChessPiece::Type::Rook);
    }

    for (int row : {1, 6})
        for (int col = 0; col < 8; ++col)
            m_Pieces[row][col].SetType(CChessPiece::Type::Pawn);

    for (int row = 2; row < 6; ++row)
        for (int col = 0; col < 8; ++col)
            m_Pieces[row][col].SetType(CChessPiece::Type::None);

    const auto topColor = CChessPiece::GetOppositeColor(bottomColor);

    for (int row = 0; row < 2; ++row)
        for (int col = 0; col < 8; ++col)
            m_Pieces[row][col].SetColor(topColor);

    for (int row = 6; row < 8; ++row)
        for (int col = 0; col < 8; ++col)
            m_Pieces[row][col].SetColor(bottomColor);

    if (bottomColor == CChessPiece::Color::Black)
    {
        std::swap(m_Pieces[0][3], m_Pieces[0][4]);
        std::swap(m_Pieces[7][3], m_Pieces[7][4]);
    }

    m_WhiteKingPos = (bottomColor == CChessPiece::Color::White) ? CSquare(7, 4) : CSquare(0, 3);
    m_BlackKingPos = (bottomColor == CChessPiece::Color::White) ? CSquare(0, 4) : CSquare(7, 3);
}

CChessPiece::Color CChessBoard::GetBottomColor() const
{
    return m_BottomColor;
}

const CChessPiece & CChessBoard::GetPieceAtSquare(const CSquare & square) const
{
    static const CChessPiece emptyPiece;

    if (!square.IsValid())
        return emptyPiece;

    return m_Pieces[square.m_Row][square.m_Col];
}

const CSquare & CChessBoard::GetWhiteKingPos() const
{
    return m_WhiteKingPos;
}

const CSquare & CChessBoard::GetBlackKingPos() const
{
    return m_BlackKingPos;
}

void CChessBoard::SetPieceAtSquare(const CChessPiece & piece, const CSquare & square)
{
    if (!square.IsValid())
        return;

    if (piece.GetType() == CChessPiece::Type::King)
    {
        auto & kingPos = (piece.GetColor() == CChessPiece::Color::White) ? m_WhiteKingPos : m_BlackKingPos;

        kingPos = square;
    }

    m_Pieces[square.m_Row][square.m_Col] = piece;
}

} // namespace ChessProj