#include "ChessBoardGraphicsView.h"

#include <QGraphicsScene>
#include <QGraphicsRectItem>
#include <QGraphicsPixmapItem>
#include <QMessageBox>
#include <QMouseEvent>

namespace ChessProj
{

static const QColor         s_ColorWhite        = QColor(237, 237, 209);
static const QColor         s_ColorBlack        = QColor(117, 149, 87);
static const QColor         s_ColorBackground   = QColor(52, 49, 47);
static const QColor         s_ColorSelection    = QColor(255, 247, 74, 200);
static const qreal          s_BoardMarginLeft   = 40.0;
static const qreal          s_BoardMarginRight  = 20.0;
static const qreal          s_BoardMarginTop    = 20.0;
static const qreal          s_BoardMarginBottom = 40.0;
static const int            s_FontSize          = 12;
static const std::size_t    s_MaxPiecesOnBoard  = 32;
static const qreal          s_MovementZValue    = 150.0;
static const qreal          s_PieceItemZValue   = 100.0;

static const QPixmap & GetPixmapForTypeColor(const CChessPiece::Type type, const CChessPiece::Color color)
{
    static QPixmap s_PiecesPixmaps[2][6];

    static bool isInitialized = false;

    if (!isInitialized)
    {
        isInitialized = true;

        s_PiecesPixmaps[0][0].load(":/UIRes/WhiteKing");
        s_PiecesPixmaps[0][1].load(":/UIRes/WhiteQueen");
        s_PiecesPixmaps[0][2].load(":/UIRes/WhiteRook");
        s_PiecesPixmaps[0][3].load(":/UIRes/WhiteBishop");
        s_PiecesPixmaps[0][4].load(":/UIRes/WhiteKnight");
        s_PiecesPixmaps[0][5].load(":/UIRes/WhitePawn");
        s_PiecesPixmaps[1][0].load(":/UIRes/BlackKing");
        s_PiecesPixmaps[1][1].load(":/UIRes/BlackQueen");
        s_PiecesPixmaps[1][2].load(":/UIRes/BlackRook");
        s_PiecesPixmaps[1][3].load(":/UIRes/BlackBishop");
        s_PiecesPixmaps[1][4].load(":/UIRes/BlackKnight");
        s_PiecesPixmaps[1][5].load(":/UIRes/BlackPawn");
    }

    const int row = (color == CChessPiece::Color::White) ? 0 : 1;

    switch (type)
    {
    case CChessPiece::Type::King :   return s_PiecesPixmaps[row][0];
    case CChessPiece::Type::Queen :  return s_PiecesPixmaps[row][1];
    case CChessPiece::Type::Rook :   return s_PiecesPixmaps[row][2];
    case CChessPiece::Type::Bishop : return s_PiecesPixmaps[row][3];
    case CChessPiece::Type::Knight : return s_PiecesPixmaps[row][4];
    case CChessPiece::Type::Pawn :   return s_PiecesPixmaps[row][5];
    }

    assert(false);

    return s_PiecesPixmaps[row][0];
}

static const QPixmap & GetPixmapForPiece(const CChessPiece & piece)
{
    return GetPixmapForTypeColor(piece.GetType(), piece.GetColor());
}

static QString GetGameStateMessage(const CChessGame::State state)
{
    switch (state)
    {
    case CChessGame::State::WhiteWon : return "White won";
    case CChessGame::State::BlackWon : return "Black won";
    case CChessGame::State::Draw :     return "Draw";
    }

    return "Active";
}

CBoardGraphicsView::CBoardGraphicsView()
{
    m_Font.setPointSize(s_FontSize);
    m_Font.setBold(true);

    m_Scene = new QGraphicsScene();

    setScene(m_Scene);

    AddSquaresNumbersLetters();

    AddPiecesItems();

    UpdateBoardItems();
}

void CBoardGraphicsView::StartNewGame(const bool asWhite)
{
    m_Game.StartNew(asWhite ? CChessPiece::Color::White : CChessPiece::Color::Black);

    UpdateBoardGeometry();

    UpdateBoardItems();
}

void CBoardGraphicsView::UpdateBoardItems()
{
    ResetBoardPiecesCache();

    const auto & board = m_Game.GetBoard();

    std::size_t pieceItemIdx = 0;

    for (int r = 0; r < 8; ++r)
        for (int c = 0; c < 8; ++c)
        {
            const auto & piece = board.GetPieceAtSquare(CSquare(r, c));
            if (piece.GetType() == CChessPiece::Type::None)
                continue;

            auto * item = m_AllPiecesItems[pieceItemIdx++];

            m_BoardPiecesCache[r][c] = item;

            item->setPixmap(GetPixmapForPiece(piece));

            item->setVisible(true);
        }

    for (std::size_t t = pieceItemIdx; t < s_MaxPiecesOnBoard; ++t)
        m_AllPiecesItems[t]->setVisible(false);

    UpdateBoardItemsPositions();
}

void CBoardGraphicsView::EvaluatePosition()
{
    QMessageBox::information(this, "FEN", m_Game.GetFEN().c_str(), QMessageBox::Ok);
}

void CBoardGraphicsView::resizeEvent(QResizeEvent * event)
{
    __super::resizeEvent(event);

    UpdateBoardGeometry();
}

void CBoardGraphicsView::mousePressEvent(QMouseEvent * event)
{
    __super::mousePressEvent(event);

    if (event->button() != Qt::LeftButton)
        return;

    m_LastMousePressSquare = GetSquareForPoint(event->pos());

    if (m_LastMousePressSquare.IsValid())
    {
        const auto squarePos = GetPosForSquare(m_LastMousePressSquare);

        m_MousePosToPieceOffset = event->pos() - squarePos;

        SetZValueForItemToMove(s_MovementZValue);
    }
}

void CBoardGraphicsView::mouseReleaseEvent(QMouseEvent * event)
{
    __super::mousePressEvent(event);

    if (event->button() != Qt::LeftButton)
        return;

    SetZValueForItemToMove(s_PieceItemZValue);

    const auto square = GetSquareForPoint(event->pos());

    if (m_Game.IsMoveLegal(CChessMove(m_LastMousePressSquare, square)))
    {
        m_Game.Move(CChessMove(m_LastMousePressSquare, square));

        UpdateBoardItems();

        if (m_Game.GetState() != CChessGame::State::Active)
            QMessageBox::information(this, "Game over", GetGameStateMessage(m_Game.GetState()), QMessageBox::Ok);
    }
    else
        ReturnLastMovedPieceBack();
}

void CBoardGraphicsView::mouseMoveEvent(QMouseEvent * event)
{
    __super::mouseMoveEvent(event);

    if (!(event->buttons() & Qt::LeftButton))
        return;

    if (m_LastMousePressSquare.IsValid())
        if (auto * item = m_BoardPiecesCache[m_LastMousePressSquare.m_Row][m_LastMousePressSquare.m_Col])
            item->setPos(event->pos() - m_MousePosToPieceOffset);
}

void CBoardGraphicsView::UpdateBoardGeometry()
{
    if (m_SquaresItems.empty())
        return;

    const bool isWhiteBottom = m_Game.GetBoard().GetBottomColor() == CChessPiece::Color::White;

    assert(m_SquaresItems.size() == 64);

    const auto viewSize = size();

    const auto viewWidth  = static_cast<qreal>(viewSize.width());
    const auto viewHeight = static_cast<qreal>(viewSize.height());

    const auto availableWidth  = viewWidth  - s_BoardMarginLeft - s_BoardMarginRight;
    const auto availableHeight = viewHeight - s_BoardMarginTop  - s_BoardMarginBottom;

    const auto sideLength = std::min(availableWidth, availableHeight);

    const auto squareSide = sideLength / 8.0;

    const QFontMetrics fontMetrics(m_Font);

    const auto numberXPos = s_BoardMarginLeft / 2.0;

    const auto yOffset = (squareSide - static_cast<qreal>(fontMetrics.height())) / 2.0;

    const auto letterYPos = viewHeight - s_BoardMarginBottom;

    m_BoardRect = QRectF(s_BoardMarginLeft, s_BoardMarginTop, sideLength, sideLength);

    m_SquareSide = squareSide;

    // squares

    for (std::size_t r = 0; r < 8; ++r)
    {
        const auto rowOffset = s_BoardMarginTop + squareSide * static_cast<qreal>(r);

        for (std::size_t c = 0; c < 8; ++c)
        {
            const auto colOffset = s_BoardMarginLeft + squareSide * static_cast<qreal>(c);

            m_SquaresItems[r * 8 + c]->setRect(colOffset, rowOffset, squareSide, squareSide);
        }
    }

    m_BackgroundItem->setRect(0.0, 0.0, viewWidth, viewHeight);

    // numbers 

    for (std::size_t i = 0; i < 8; ++i)
    {
        const auto rowOffset = s_BoardMarginTop + squareSide * static_cast<qreal>(i);

        const auto numberIndex = isWhiteBottom ? i : 7u - i;

        m_SquaresNumbers[numberIndex]->setPos(numberXPos, rowOffset + yOffset);
    }

    // letters

    for (std::size_t i = 0; i < 8; ++i)
    {
        const auto letterIndex = isWhiteBottom ? i : 7u - i;

        auto * letterText = m_SquaresLetters[letterIndex];

        const auto ww = fontMetrics.width(letterText->toPlainText());

        const auto colOffset = s_BoardMarginLeft + squareSide * static_cast<qreal>(i);

        const auto xOffset = (squareSide - static_cast<qreal>(ww)) / 2.0;

        letterText->setPos(colOffset + xOffset, letterYPos);
    }

    UpdateBoardItemsPositions();
}

void CBoardGraphicsView::UpdateBoardItemsPositions()
{
    const auto graphicsScale = m_SquareSide / 333.0;

    for (std::size_t r = 0; r < 8; ++r)
    {
        const auto rowOffset = s_BoardMarginTop + m_SquareSide * static_cast<qreal>(r);

        for (std::size_t c = 0; c < 8; ++c)
            if (auto * item = m_BoardPiecesCache[r][c])
            {
                const auto colOffset = s_BoardMarginLeft + m_SquareSide * static_cast<qreal>(c);

                item->setPos(colOffset, rowOffset);
                item->setScale(graphicsScale);
            }
    }
}

void CBoardGraphicsView::AddSquaresNumbersLetters()
{
    m_BackgroundItem = m_Scene->addRect(QRectF(), QPen(), QBrush(s_ColorBackground));

    for (int i = 0; i < 8; ++i)
    {
        bool isWhite = (i % 2) == 0;

        for (int j = 0; j < 8; ++j)
        {
            m_SquaresItems.push_back(m_Scene->addRect(QRectF(), QPen(Qt::black), QBrush(isWhite ? s_ColorWhite : s_ColorBlack)));

            isWhite = !isWhite;
        }
    }

    for (int i = 0; i < 8; ++i)
    {
        const char number = '8' - i;
        const char letter = 'A' + i;

        auto * numberText = m_Scene->addText(QString(number), m_Font);
        auto * letterText = m_Scene->addText(QString(letter), m_Font);

        numberText->setDefaultTextColor(s_ColorWhite);
        letterText->setDefaultTextColor(s_ColorWhite);

        m_SquaresNumbers.push_back(numberText);
        m_SquaresLetters.push_back(letterText);
    }
}

void CBoardGraphicsView::AddPiecesItems()
{
    m_AllPiecesItems.reserve(s_MaxPiecesOnBoard);

    const auto & whiteQueenPixmap = GetPixmapForTypeColor(CChessPiece::Type::Queen, CChessPiece::Color::White);

    for (int i = 0; i < s_MaxPiecesOnBoard; ++i)
    {
        auto item = m_Scene->addPixmap(whiteQueenPixmap);

        item->setZValue(s_PieceItemZValue);

        m_AllPiecesItems.push_back(item);
    }

    ResetBoardPiecesCache();
}

void CBoardGraphicsView::ResetBoardPiecesCache()
{
    for (int r = 0; r < 8; ++r)
        for (int c = 0; c < 8; ++c)
            m_BoardPiecesCache[r][c] = nullptr;
}

void CBoardGraphicsView::ReturnLastMovedPieceBack()
{
    if (!m_LastMousePressSquare.IsValid())
        return;

    if (auto * item = m_BoardPiecesCache[m_LastMousePressSquare.m_Row][m_LastMousePressSquare.m_Col])
        item->setPos(GetPosForSquare(m_LastMousePressSquare));
}

void CBoardGraphicsView::SetZValueForItemToMove(const float zValue)
{
    if (!m_LastMousePressSquare.IsValid())
        return;

    if (auto * item = m_BoardPiecesCache[m_LastMousePressSquare.m_Row][m_LastMousePressSquare.m_Col])
        item->setZValue(zValue);
}

CSquare CBoardGraphicsView::GetSquareForPoint(const QPointF & pt) const
{
    if (!m_BoardRect.contains(pt))
        return CSquare();

    const QPointF ptBoard(pt.x() - m_BoardRect.x(), pt.y() - m_BoardRect.y());

    CSquare result;
    result.m_Row = static_cast<int>(ptBoard.y() / m_SquareSide);
    result.m_Col = static_cast<int>(ptBoard.x() / m_SquareSide);

    result.m_Row = std::min(result.m_Row, 7);
    result.m_Col = std::min(result.m_Col, 7);

    return result;
}

QPointF CBoardGraphicsView::GetPosForSquare(const CSquare & square) const
{
    assert(square.IsValid());

    return QPointF(m_BoardRect.x() + static_cast<qreal>(square.m_Col) * m_SquareSide,
                   m_BoardRect.y() + static_cast<qreal>(square.m_Row) * m_SquareSide);
}

} // namespace ChessProj