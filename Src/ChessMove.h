#pragma once

#include "ChessBoard.h"

namespace ChessProj
{

struct CChessMove
{
    CChessMove(const CSquare from = CSquare(), const CSquare to = CSquare());

    bool IsValid() const;

    int GetNumRanks() const;
    int GetNumFiles() const;

    int GetRankIncrement() const;
    int GetFileIncrement() const;

    CSquare     m_From;
    CSquare     m_To;
};

class CTempMove
{
public:
    CTempMove(const CChessMove & mv, CChessBoard & board);
    ~CTempMove();

private:
    const CChessMove    m_Mv;
    const CChessPiece   m_PieceFrom;
    const CChessPiece   m_PieceTo;

    CChessBoard &       m_Board;
};

extern const std::vector<CSquare> s_KingOffsets;
extern const std::vector<CSquare> s_KnightOffsets;
extern const std::vector<CSquare> s_DiagonalOffsets;
extern const std::vector<CSquare> s_OrthogonalOffsets;

} // namespace ChessProj