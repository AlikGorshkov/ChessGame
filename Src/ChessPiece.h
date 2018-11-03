#pragma once

namespace ChessProj
{

class CChessPiece
{
public:
    enum class Type
    {
        None,
        King,
        Queen,
        Rook,
        Bishop,
        Knight,
        Pawn
    };

    enum class Color
    {
        White,
        Black
    };

    explicit CChessPiece(const Type type = Type::None, const Color color = Color::White);

    Type GetType() const;
    void SetType(const Type type);

    Color GetColor() const;
    void SetColor(const Color color);

private:
    Type    m_Type;
    Color   m_Color;
};

} // namespace ChessProj