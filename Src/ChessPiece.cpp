#include "ChessPiece.h"

namespace ChessProj
{

CChessPiece::CChessPiece(const Type type /*= Type::None*/, const Color color /*= Color::White*/)
    : m_Type(type)
    , m_Color(color)
{
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

} // namespace ChessProj