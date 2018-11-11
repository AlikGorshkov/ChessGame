#include "ChessMove.h"

#include <algorithm>

namespace ChessProj
{

const std::vector<CSquare> s_KnightOffsets =
{
    CSquare(-2, -1),
    CSquare(-2,  1),
    CSquare(-1, -2),
    CSquare(-1,  2),
    CSquare( 1, -2),
    CSquare( 1,  2),
    CSquare( 2, -1),
    CSquare( 2,  1)
};

const std::vector<CSquare> s_DiagonalOffsets =
{
    CSquare(-1, -1),
    CSquare(-1,  1),
    CSquare( 1,  1),
    CSquare( 1, -1)
};

const std::vector<CSquare> s_OrthogonalOffsets =
{
    CSquare(-1,  0),
    CSquare( 1,  0),
    CSquare( 0,  1),
    CSquare( 0, -1)
};

// CChessMove

CChessMove::CChessMove(const CSquare from /*= CSquare()*/, const CSquare to /*= CSquare()*/)
    : m_From(from)
    , m_To(to)
{
}

bool CChessMove::IsValid() const
{
    return m_From.IsValid() && m_To.IsValid();
}

int CChessMove::GetNumRanks() const
{
    return std::abs(m_To.m_Row - m_From.m_Row);
}

int CChessMove::GetNumFiles() const
{
    return std::abs(m_To.m_Col - m_From.m_Col);
}

int CChessMove::GetRankIncrement() const
{
    if (m_To.m_Row == m_From.m_Row)
        return 0;

    return (m_To.m_Row > m_From.m_Row) ? 1 : -1;
}

int CChessMove::GetFileIncrement() const
{
    if (m_To.m_Col == m_From.m_Col)
        return 0;

    return (m_To.m_Col > m_From.m_Col) ? 1 : -1;
}

// CTempMove

CTempMove::CTempMove(const CChessMove & mv, CChessBoard & board)
    : m_Mv(mv)
    , m_PieceFrom(board.GetPieceAtSquare(mv.m_From))
    , m_PieceTo(board.GetPieceAtSquare(mv.m_To))
    , m_Board(board)
{
    m_Board.SetPieceAtSquare(m_PieceFrom,   m_Mv.m_To);
    m_Board.SetPieceAtSquare(CChessPiece(), m_Mv.m_From);
}

CTempMove::~CTempMove()
{
    m_Board.SetPieceAtSquare(m_PieceTo,   m_Mv.m_To);
    m_Board.SetPieceAtSquare(m_PieceFrom, m_Mv.m_From);
}

} // namespace ChessProj