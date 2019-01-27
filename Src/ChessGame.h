#pragma once

#include "ChessMove.h"

#include <string>

namespace ChessProj
{

class CChessGame
{
public:
    enum class State
    {
        Active,
        Draw,
        WhiteWon,
        BlackWon
    };

    CChessGame();

    void StartNew(const CChessPiece::Color color);

    CChessPiece::Color GetCurrentMoveColor() const;

    const CChessBoard & GetBoard() const;

    State GetState() const;

    std::string GetFEN() const;

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

    bool IsMoveAvailable() const;
    bool IsMoveAvailableIncremental(const CSquare & square, const std::vector<CSquare> & offsets) const;
    bool IsMoveAvailableStatic(const CSquare & square, const std::vector<CSquare> & offsets) const;

    void UpdateState();

    std::string GetCastleFEN() const;
    std::string GetEnPassantSquare() const;

    mutable CChessBoard     m_Board;
    CChessMove              m_LastMove;
    CChessPiece::Color      m_CurrentMoveColor        = CChessPiece::Color::White;
    State                   m_State                   = State::Active;

    bool                    m_WhiteCanCastleKingSide  = true;
    bool                    m_WhiteCanCastleQueenSide = true;
    bool                    m_BlackCanCastleKingSide  = true;
    bool                    m_BlackCanCastleQueenSide = true;
};

} // namespace ChessProj