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