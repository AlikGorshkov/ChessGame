#pragma once

#include "ChessMove.h"

namespace ChessProj
{

class CChessGame
{
public:
    CChessGame();

    void StartNew(const CChessPiece::Color color);

    CChessPiece::Color GetCurrentMoveColor() const;

    const CChessBoard & GetBoard() const;

    void Move(const CChessMove & mv, const CChessPiece::Type promoteType = CChessPiece::Type::Queen);

    bool IsMoveLegal(const CChessMove & mv) const;

private:
    bool IsPawnMoveLegal(const CChessMove & mv) const;
    bool IsKnightMoveLegal(const CChessMove & mv) const;
    bool IsBishopMoveLegal(const CChessMove & mv) const;
    bool IsRookMoveLegal(const CChessMove & mv) const;
    bool IsQueenMoveLegal(const CChessMove & mv) const;
    bool IsKingMoveLegal(const CChessMove & mv) const;

    bool IsDiagonalMoveLegal(const CChessMove & mv) const;
    bool IsOrthogonalMoveLegal(const CChessMove & mv) const;

    bool IsKingUnderCheck() const;

    void HandleKingMove(const CChessMove & mv);
    void HandleRookMove(const CChessMove & mv);
    void HandlePawnMove(const CChessMove & mv, const CChessPiece::Type promoteType);

    mutable CChessBoard     m_Board;
    CChessMove              m_LastMove;
    CChessPiece::Color      m_CurrentMoveColor        = CChessPiece::Color::White;

    bool                    m_WhiteCanCastleKingSide  = true;
    bool                    m_WhiteCanCastleQueenSide = true;
    bool                    m_BlackCanCastleKingSide  = true;
    bool                    m_BlackCanCastleQueenSide = true;
};

} // namespace ChessProj