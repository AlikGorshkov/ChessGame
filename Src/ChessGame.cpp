#include "ChessGame.h"

#include <cassert>

namespace ChessProj
{

CChessGame::CChessGame()
{
    StartNew(CChessPiece::Color::White);
}

void CChessGame::StartNew(const CChessPiece::Color color)
{
    m_CurrentMoveColor = CChessPiece::Color::White;

    m_Board.Initialize(color);

    m_LastMove = CChessMove();

    m_WhiteCanCastleKingSide  = true;
    m_WhiteCanCastleQueenSide = true;
    m_BlackCanCastleKingSide  = true;
    m_BlackCanCastleQueenSide = true;
}

CChessPiece::Color CChessGame::GetCurrentMoveColor() const
{
    return m_CurrentMoveColor;
}

const CChessBoard & CChessGame::GetBoard() const
{
    return m_Board;
}

void CChessGame::Move(const CChessMove & mv, const CChessPiece::Type promoteType /*= CChessPiece::Type::Queen*/)
{
    if (!IsMoveLegal(mv))
        return;

    const auto piece = m_Board.GetPieceAtSquare(mv.m_From);

    m_Board.SetPieceAtSquare(piece, mv.m_To);

    m_Board.SetPieceAtSquare(CChessPiece(), mv.m_From);

    // additional piece movements (castling and en passant)
    switch (piece.GetType())
    {
    case CChessPiece::Type::Pawn: HandlePawnMove(mv, promoteType); break;
    case CChessPiece::Type::Rook: HandleRookMove(mv); break;
    case CChessPiece::Type::King: HandleKingMove(mv); break;
    }

    m_LastMove = mv;

    m_CurrentMoveColor = CChessPiece::GetOppositeColor(piece.GetColor());
}

bool CChessGame::IsMoveLegal(const CChessMove & mv) const
{
    if (!mv.IsValid())
        return false;

    const auto & pieceFrom = m_Board.GetPieceAtSquare(mv.m_From);
    if (!pieceFrom.IsValid())
        return false;

    if (pieceFrom.GetColor() != m_CurrentMoveColor)
        return false;

    const auto & pieceTo = m_Board.GetPieceAtSquare(mv.m_To);
    if (pieceTo.IsValid() && pieceTo.GetColor() == pieceFrom.GetColor())
        return false; // can't capture a valid piece of the same color. Also covers the case with (from == to)

    switch (pieceFrom.GetType())
    {
    case CChessPiece::Type::Pawn :   return IsPawnMoveLegal(mv);
    case CChessPiece::Type::Knight : return IsKnightMoveLegal(mv);
    case CChessPiece::Type::Bishop : return IsBishopMoveLegal(mv);
    case CChessPiece::Type::Rook :   return IsRookMoveLegal(mv);
    case CChessPiece::Type::Queen :  return IsQueenMoveLegal(mv);
    case CChessPiece::Type::King :   return IsKingMoveLegal(mv);
    }

    assert(false);

    return false;
}

bool CChessGame::IsPawnMoveLegal(const CChessMove & mv) const
{
    const bool isMovableUp = m_Board.GetBottomColor() == m_CurrentMoveColor; // up the board, but the opposite direction in the container

    if ( isMovableUp && mv.m_To.m_Row >= mv.m_From.m_Row ||
        !isMovableUp && mv.m_To.m_Row <= mv.m_From.m_Row)
        return false; // can't move backwards or stay on the same rank

    const int numRanks = mv.GetNumRanks();
    if (numRanks > 2)
        return false;

    const int numFiles = mv.GetNumFiles();
    if (numFiles > 1)
        return false;

    const auto & pieceTo = m_Board.GetPieceAtSquare(mv.m_To);

    if (numRanks == 2) // only from starting position
    {
        if (numFiles != 0)
            return false; // pawn must stay on the same file

        if (pieceTo.IsValid())
            return false; // blocked by some other piece

        if ( isMovableUp && mv.m_From.m_Row != 6 ||
            !isMovableUp && mv.m_From.m_Row != 1)
            return false;

        const int nextRank = (mv.m_From.m_Row + mv.m_To.m_Row) / 2;

        const auto & pieceNextRank = m_Board.GetPieceAtSquare(CSquare(nextRank, mv.m_To.m_Col));
        if (pieceNextRank.IsValid())
            return false; // blocked by some other piece
    }
    else // numRanks == 1
    {
        if (numFiles == 0 && pieceTo.IsValid())
            return false; // blocked by some other piece

        if (numFiles == 1 && !pieceTo.IsValid()) // en passant
        {
            // there must be a previous 2-rank pawn move
            if (m_LastMove.GetNumRanks() != 2)
                return false;

            const auto & piecePrevMove = m_Board.GetPieceAtSquare(m_LastMove.m_To);
            if (piecePrevMove.GetType() != CChessPiece::Type::Pawn)
                return false;

            if (m_LastMove.m_To.m_Row != mv.m_From.m_Row)
                return false;

            if (m_LastMove.m_To.m_Col != mv.m_To.m_Col)
                return false;
        }
    }

    CTempMove tempMove(mv, m_Board);

    if (IsKingUnderCheck())
        return false;

    return true;
}

bool CChessGame::IsKnightMoveLegal(const CChessMove & mv) const
{
    const int numRanks = mv.GetNumRanks();
    const int numFiles = mv.GetNumFiles();

    if (!(numRanks == 2 && numFiles == 1 || numRanks == 1 && numFiles == 2))
        return false;

    CTempMove tempMove(mv, m_Board);

    if (IsKingUnderCheck())
        return false;

    return true;
}

bool CChessGame::IsBishopMoveLegal(const CChessMove & mv) const
{
    if (!IsDiagonalMoveLegal(mv))
        return false;

    CTempMove tempMove(mv, m_Board);

    if (IsKingUnderCheck())
        return false;

    return true;
}

bool CChessGame::IsRookMoveLegal(const CChessMove & mv) const
{
    if (!IsOrthogonalMoveLegal(mv))
        return false;

    CTempMove tempMove(mv, m_Board);

    if (IsKingUnderCheck())
        return false;

    return true;
}

bool CChessGame::IsQueenMoveLegal(const CChessMove & mv) const
{
    const int numRanks = mv.GetNumRanks();
    const int numFiles = mv.GetNumFiles();

    if (numRanks == numFiles)
    {
        if (!IsDiagonalMoveLegal(mv))
            return false;
    }
    else
    if (numRanks == 0 && numFiles != 0 ||
        numRanks != 0 && numFiles == 0)
    {
        if (!IsOrthogonalMoveLegal(mv))
            return false;
    }
    else
        return false;

    CTempMove tempMove(mv, m_Board);

    if (IsKingUnderCheck())
        return false;

    return true;
}

bool CChessGame::IsKingMoveLegal(const CChessMove & mv) const
{
    const int numRanks = mv.GetNumRanks();
    const int numFiles = mv.GetNumFiles();

    if (numRanks > 1 || numFiles > 2)
        return false;

    if (numFiles == 2) // potential castling
    {
        if (numRanks != 0)
            return false;

        if (IsKingUnderCheck())
            return false; // can't castle, when under the check

        const int fileInc = mv.GetFileIncrement();

        const bool isKingSide = (m_Board.GetBottomColor() == CChessPiece::Color::White) ? (fileInc > 0) : (fileInc < 0);

        const bool isCastlingAvailable = (m_CurrentMoveColor == CChessPiece::Color::White) ?
                                         (isKingSide ? m_WhiteCanCastleKingSide : m_WhiteCanCastleQueenSide) :
                                         (isKingSide ? m_BlackCanCastleKingSide : m_BlackCanCastleQueenSide);

        if (!isCastlingAvailable)
            return false;

        const int emptySquares = isKingSide ? 2 : 3;

        for (int i = 1; i <= emptySquares; ++i)
            if (m_Board.GetPieceAtSquare(mv.m_From + CSquare(0, fileInc * i)).IsValid())
                return false;

        CTempMove tempMove(CChessMove(mv.m_From, mv.m_From + CSquare(0, fileInc)), m_Board);

        if (IsKingUnderCheck())
            return false; // intermediate square is under attack
    }

    CTempMove tempMove(mv, m_Board);

    if (IsKingUnderCheck())
        return false;

    return true;
}

bool CChessGame::IsDiagonalMoveLegal(const CChessMove & mv) const
{
    const int numRanks = mv.GetNumRanks();
    const int numFiles = mv.GetNumFiles();

    if (numRanks != numFiles)
        return false;

    const int rankInc = mv.GetRankIncrement();
    const int fileInc = mv.GetFileIncrement();

    int rankToCheck = mv.m_From.m_Row;
    int fileToCheck = mv.m_From.m_Col;

    for (int i = 1; i < numRanks; ++i)
    {
        rankToCheck += rankInc;
        fileToCheck += fileInc;

        if (m_Board.GetPieceAtSquare(CSquare(rankToCheck, fileToCheck)).IsValid())
            return false;
    }

    return true;
}

bool CChessGame::IsOrthogonalMoveLegal(const CChessMove & mv) const
{
    const int numRanks = mv.GetNumRanks();
    const int numFiles = mv.GetNumFiles();

    if (numRanks != 0 && numFiles != 0)
        return false;

    if (numRanks != 0)
    {
        const int rankInc = mv.GetRankIncrement();

        int rankToCheck = mv.m_From.m_Row;

        for (int i = 1; i < numRanks; ++i)
        {
            rankToCheck += rankInc;

            if (m_Board.GetPieceAtSquare(CSquare(rankToCheck, mv.m_From.m_Col)).IsValid())
                return false;
        }
    }
    else
    {
        const int fileInc = mv.GetFileIncrement();

        int fileToCheck = mv.m_From.m_Col;

        for (int i = 1; i < numFiles; ++i)
        {
            fileToCheck += fileInc;

            if (m_Board.GetPieceAtSquare(CSquare(mv.m_From.m_Row, fileToCheck)).IsValid())
                return false;
        }
    }

    return true;
}

bool CChessGame::IsKingUnderCheck() const
{
    const auto & kingSquare = (m_CurrentMoveColor == CChessPiece::Color::White) ?
                              m_Board.GetWhiteKingPos() :
                              m_Board.GetBlackKingPos();

    const auto opponentColor = CChessPiece::GetOppositeColor(m_CurrentMoveColor);

    // from Pawn
    {
        const auto bottomColor = m_Board.GetBottomColor();

        const int pawnRank = kingSquare.m_Row + (bottomColor == m_CurrentMoveColor ? -1 : 1);

        const auto & pawn1 = m_Board.GetPieceAtSquare(CSquare(pawnRank, kingSquare.m_Col + 1));
        if (pawn1.GetColor() == opponentColor && pawn1.GetType() == CChessPiece::Type::Pawn)
            return true;

        const auto & pawn2 = m_Board.GetPieceAtSquare(CSquare(pawnRank, kingSquare.m_Col - 1));
        if (pawn2.GetColor() == opponentColor && pawn2.GetType() == CChessPiece::Type::Pawn)
            return true;
    }

    // from Knight
    for (const auto & offset : s_KnightOffsets)
    {
        const auto & knight = m_Board.GetPieceAtSquare(kingSquare + offset);
        if (knight.GetColor() == opponentColor && knight.GetType() == CChessPiece::Type::Knight)
            return true;
    }

    auto CheckIncrementalOffsets = [&](const std::vector<CSquare> & offsets, const CChessPiece::Type type2)
    {
        for (const auto & offset : offsets)
        {
            auto square = kingSquare + offset;

            while (square.IsValid())
            {
                const auto & piece = m_Board.GetPieceAtSquare(square);
                if (piece.IsValid())
                {
                    if (piece.GetColor() == opponentColor)
                        if (piece.GetType() == CChessPiece::Type::Queen || piece.GetType() == type2)
                            return true;

                    break; // blocked by a piece of the same color or non-threatening piece of the opposite color => check the next offset
                }

                square += offset;
            }
        }

        return false;
    };

    // from diagonal (Queen or Bishop)
    if (CheckIncrementalOffsets(s_DiagonalOffsets, CChessPiece::Type::Bishop))
        return true;

    // from orthogonal (Queen or Rook)
    if (CheckIncrementalOffsets(s_OrthogonalOffsets, CChessPiece::Type::Rook))
        return true;

    // from King
    for (int row = -1; row <= 1; ++row)
        for (int col = -1; col <= 1; ++col)
        {
            const auto square = kingSquare + CSquare(row, col);
            if (!square.IsValid())
                continue;

            const auto & piece = m_Board.GetPieceAtSquare(square);
            if (piece.GetColor() == opponentColor && piece.GetType() == CChessPiece::Type::King)
                return true;
        }

    return false;
}

void CChessGame::HandleKingMove(const CChessMove & mv)
{
    const auto & king = m_Board.GetPieceAtSquare(mv.m_To);

    assert(king.GetType() == CChessPiece::Type::King);

    bool & canCastleKingSide  = (king.GetColor() == CChessPiece::Color::White) ? m_WhiteCanCastleKingSide  : m_BlackCanCastleKingSide;
    bool & canCastleQueenSide = (king.GetColor() == CChessPiece::Color::White) ? m_WhiteCanCastleQueenSide : m_BlackCanCastleQueenSide;

    canCastleKingSide  = false;
    canCastleQueenSide = false;

    if (mv.GetNumFiles() == 2)
    {
        const int fileInc = mv.GetFileIncrement();

        const int rookOldCol = (fileInc > 0) ? 7 : 0;

        const int rookNewCol = (mv.m_From.m_Col + mv.m_To.m_Col) / 2;

        const auto rookOldSquare = CSquare(mv.m_To.m_Row, rookOldCol);

        const auto rookNewSquare = CSquare(mv.m_To.m_Row, rookNewCol);

        const auto rook = m_Board.GetPieceAtSquare(rookOldSquare);

        assert(rook.GetType() == CChessPiece::Type::Rook);

        assert(!m_Board.GetPieceAtSquare(rookNewSquare).IsValid());

        m_Board.SetPieceAtSquare(rook, rookNewSquare);

        m_Board.SetPieceAtSquare(CChessPiece(), rookOldSquare);
    }
}

void CChessGame::HandleRookMove(const CChessMove & mv)
{
    if (mv.m_From.m_Col != 0 && mv.m_From.m_Col != 7)
        return;

    const auto & rook = m_Board.GetPieceAtSquare(mv.m_To);

    assert(rook.GetType() == CChessPiece::Type::Rook);

    bool & canCastleKingSide  = (rook.GetColor() == CChessPiece::Color::White) ? m_WhiteCanCastleKingSide  : m_BlackCanCastleKingSide;
    bool & canCastleQueenSide = (rook.GetColor() == CChessPiece::Color::White) ? m_WhiteCanCastleQueenSide : m_BlackCanCastleQueenSide;

    if (!canCastleKingSide && !canCastleQueenSide)
        return;

    const bool isKingSide = (m_Board.GetBottomColor() == CChessPiece::Color::White) ? (mv.m_From.m_Col == 7) : (mv.m_From.m_Col == 0);

    if (isKingSide)
        canCastleKingSide  = false;
    else
        canCastleQueenSide = false;
}

void CChessGame::HandlePawnMove(const CChessMove & mv, const CChessPiece::Type promoteType)
{
    if (mv.m_To.m_Row == 0 || mv.m_To.m_Row == 7)
    {
        auto pawnPromoted = m_Board.GetPieceAtSquare(mv.m_To);

        pawnPromoted.SetType(promoteType);

        m_Board.SetPieceAtSquare(pawnPromoted, mv.m_To);

        return;
    }

    if (m_LastMove.GetNumRanks() == 2            &&
        m_LastMove.m_To.m_Row == mv.m_From.m_Row &&
        m_LastMove.m_To.m_Col == mv.m_To.m_Col)
    {
        const auto & pawn = m_Board.GetPieceAtSquare(m_LastMove.m_To);
        if (pawn.GetType() == CChessPiece::Type::Pawn)
            m_Board.SetPieceAtSquare(CChessPiece(), m_LastMove.m_To);
    }
}

} // namespace ChessProj