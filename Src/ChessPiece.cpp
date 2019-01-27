#include "ChessPiece.h"

namespace ChessProj
{

CChessPiece::CChessPiece(const Type type /*= Type::None*/, const Color color /*= Color::White*/)
    : m_Type(type)
    , m_Color(color)
{
}

bool CChessPiece::IsValid() const
{
    return m_Type != Type::None;
}

char CChessPiece::GetFENChar() const
{
    const bool isWhite = m_Color == Color::White;

    switch (m_Type)
    {
    case Type::Pawn :   return isWhite ? 'P' : 'p';
    case Type::Knight : return isWhite ? 'N' : 'n';
    case Type::Bishop : return isWhite ? 'B' : 'b';
    case Type::Queen :  return isWhite ? 'Q' : 'q';
    case Type::Rook :   return isWhite ? 'R' : 'r';
    }

    return isWhite ? 'K' : 'k';
}

CChessPiece::Type CChessPiece::GetType() const
{
    return m_Type;
}

void CChessPiece::SetType(const Type type)
{
    m_Type = type;
}

CChessPiece::Color CChessPiece::GetColor() const
{
    return m_Color;
}

void CChessPiece::SetColor(const Color color)
{
    m_Color = color;
}

CChessPiece::Color CChessPiece::GetOppositeColor(const Color color)
{
    return (color == Color::White) ? Color::Black : Color::White;
}

} // namespace ChessProj