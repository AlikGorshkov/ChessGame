#include "ChessBoard.h"

#include <algorithm>
#include <initializer_list>

namespace ChessProj
{

CChessBoard::CChessBoard()
{
    Initialize(CChessPiece::Color::White);
}

const CChessPiece * CChessBoard::GetPieces() const
{
    return &m_Pieces[0][0];
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

    const auto topColor = (bottomColor == CChessPiece::Color::White) ?
                          CChessPiece::Color::Black :
                          CChessPiece::Color::White;

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
}

} // namespace ChessProj