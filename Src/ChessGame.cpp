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
    m_State            = State::Active;

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

CChessGame::State CChessGame::GetState() const
{
    return m_State;
}

std::string CChessGame::GetFEN() const
{
    std::string fen;
    fen.reserve(100);

    const bool isWhiteSide = m_Board.GetBottomColor() == CChessPiece::Color::White;

    const int start = isWhiteSide ? 0 :  7;
    const int end   = isWhiteSide ? 8 : -1;
    const int inc   = isWhiteSide ? 1 : -1;

    for (int rank = start; rank != end; rank += inc)
    {
        int emptyFiles = 0;

        for (int file = start; file != end; file += inc)
        {
            const auto & piece = m_Board.GetPieceAtSquare(CSquare(rank, file));
            if (piece.IsValid())
            {
                if (emptyFiles > 0)
                    fen += '0' + emptyFiles;

                fen += piece.GetFENChar();

                emptyFiles = 0;
            }
            else
                ++emptyFiles;
        }

        if (emptyFiles > 0)
            fen += '0' + emptyFiles;

        fen += '/';
    }

    fen.back() = ' '; // replace the last '/' with a space

    fen += (m_CurrentMoveColor == CChessPiece::Color::White) ? 'w' : 'b';

    fen += ' ';

    fen += GetCastleFEN();

    fen += ' ';

    fen += GetEnPassantSquare();

    fen += " 0 1";

    return fen;
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

    UpdateState();
}

bool CChessGame::IsMoveLegal(const CChessMove & mv) const
{
    if (m_State != State::Active)
        return false;

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
    for (const auto & offset : s_KingOffsets)
    {
        const auto & king = m_Board.GetPieceAtSquare(kingSquare + offset);
        if (king.GetColor() == opponentColor && king.GetType() == CChessPiece::Type::King)
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

bool CChessGame::IsMoveAvailable() const
{
    const auto piecesSquares = m_Board.GetPieces(m_CurrentMoveColor);

    for (const auto & square : piecesSquares)
    {
        const auto piece = m_Board.GetPieceAtSquare(square);

        switch (piece.GetType())
        {
        case CChessPiece::Type::Pawn:
        {
            const int rankInc = (m_Board.GetBottomColor() == m_CurrentMoveColor) ? -1 : 1;

            const std::vector<CSquare> offsets =
            {
                CSquare(rankInc,  0),
                CSquare(rankInc, -1),
                CSquare(rankInc,  1),
                CSquare(2 * rankInc, 0)
            };

            for (const auto & offset : offsets)
            {
                const auto squareTo = square + offset;

                const auto & pieceTo = m_Board.GetPieceAtSquare(squareTo);
                if (pieceTo.IsValid() && pieceTo.GetColor() == m_CurrentMoveColor)
                    continue;

                if (IsPawnMoveLegal(CChessMove(square, squareTo)))
                    return true;
            }

            break;
        }
        case CChessPiece::Type::Bishop:
        {
            if (IsMoveAvailableIncremental(square, s_DiagonalOffsets))
                return true;

            break;
        }
        case CChessPiece::Type::Knight:
        {
            if (IsMoveAvailableStatic(square, s_KnightOffsets))
                return true;

            break;
        }
        case CChessPiece::Type::King:
        {
            if (IsMoveAvailableStatic(square, s_KingOffsets))
                return true;

            break;
        }
        case CChessPiece::Type::Rook:
        {
            if (IsMoveAvailableIncremental(square, s_OrthogonalOffsets))
                return true;

            break;
        }
        case CChessPiece::Type::Queen:
        {
            if (IsMoveAvailableIncremental(square, s_DiagonalOffsets))
                return true;

            if (IsMoveAvailableIncremental(square, s_OrthogonalOffsets))
                return true;

            break;
        }
        }
    }

    return false;
}

bool CChessGame::IsMoveAvailableIncremental(const CSquare & square, const std::vector<CSquare> & offsets) const
{
    const auto pieceType = m_Board.GetPieceAtSquare(square).GetType();

    assert(pieceType == CChessPiece::Type::Bishop || pieceType == CChessPiece::Type::Rook || pieceType == CChessPiece::Type::Queen);

    for (const auto & offset : offsets)
    {
        auto squareTo = square + offset;

        while (squareTo.IsValid())
        {
            const auto & piece = m_Board.GetPieceAtSquare(squareTo);
            if (piece.IsValid() && piece.GetColor() == m_CurrentMoveColor)
                break; // blocked by a piece of the same color

            switch (pieceType)
            {
            case CChessPiece::Type::Bishop:
            {
                if (IsBishopMoveLegal(CChessMove(square, squareTo)))
                    return true;

                break;
            }
            case CChessPiece::Type::Rook:
            {
                if (IsRookMoveLegal(CChessMove(square, squareTo)))
                    return true;

                break;
            }
            case CChessPiece::Type::Queen:
            {
                if (IsQueenMoveLegal(CChessMove(square, squareTo)))
                    return true;

                break;
            }
            }

            if (piece.IsValid())
                break; // blocked by a piece of the opposite color that cannot be taken => stop iterating this offset

            squareTo += offset;
        }
    }

    return false;
}

bool CChessGame::IsMoveAvailableStatic(const CSquare & square, const std::vector<CSquare> & offsets) const
{
    const auto pieceType = m_Board.GetPieceAtSquare(square).GetType();

    assert(pieceType == CChessPiece::Type::Knight || pieceType == CChessPiece::Type::King);

    for (const auto & offset : offsets)
    {
        const auto squareTo = square + offset;
        if (!squareTo.IsValid())
            continue;

        const auto & piece = m_Board.GetPieceAtSquare(squareTo);
        if (piece.IsValid() && piece.GetColor() == m_CurrentMoveColor)
            continue; // can't capture a piece of the same color

        switch (pieceType)
        {
        case CChessPiece::Type::Knight:
        {
            if (IsKnightMoveLegal(CChessMove(square, squareTo)))
                return true;

            break;
        }
        case CChessPiece::Type::King:
        {
            if (IsKingMoveLegal(CChessMove(square, squareTo)))
                return true;

            break;
        }
        }
    }

    return false;
}

void CChessGame::UpdateState()
{
    if (IsMoveAvailable())
        return;

    if (IsKingUnderCheck())
        m_State = (m_CurrentMoveColor == CChessPiece::Color::White) ? State::BlackWon : State::WhiteWon;
    else
        m_State = State::Draw;
}

std::string CChessGame::GetCastleFEN() const
{
    if (!m_WhiteCanCastleKingSide && !m_WhiteCanCastleQueenSide && !m_BlackCanCastleKingSide && !m_BlackCanCastleQueenSide)
        return "-";

    std::string fen;

    if (m_WhiteCanCastleKingSide)
        fen += 'K';

    if (m_WhiteCanCastleQueenSide)
        fen += 'Q';

    if (m_BlackCanCastleKingSide)
        fen += 'k';

    if (m_BlackCanCastleQueenSide)
        fen += 'q';

    return fen;
}

std::string CChessGame::GetEnPassantSquare() const
{
    if (!m_LastMove.IsValid()         ||
        m_LastMove.GetNumFiles() != 0 ||
        m_LastMove.GetNumRanks() != 2)
        return "-";

    const auto & pawn = m_Board.GetPieceAtSquare(m_LastMove.m_To);
    if (pawn.GetType() != CChessPiece::Type::Pawn)
        return "-";

    const auto behindPawnSquare = CSquare(m_LastMove.m_From.m_Row + m_LastMove.GetRankIncrement(), m_LastMove.m_From.m_Col);

    return m_Board.GetSquareName(behindPawnSquare);
}

} // namespace ChessProj