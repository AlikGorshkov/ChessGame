#include "ChessBoardGraphicsView.h"
#include "ChessBoard.h"

#include <QGraphicsScene>
#include <QGraphicsRectItem>
#include <QGraphicsPixmapItem>

namespace ChessProj
{

static const QColor         s_ColorWhite        = QColor(237, 237, 209);
static const QColor         s_ColorBlack        = QColor(117, 149, 87);
static const QColor         s_ColorBackground   = QColor(52, 49, 47);
static const qreal          s_BoardMarginLeft   = 40.0;
static const qreal          s_BoardMarginRight  = 20.0;
static const qreal          s_BoardMarginTop    = 20.0;
static const qreal          s_BoardMarginBottom = 40.0;
static const int            s_FontSize          = 12;
static const std::size_t    s_MaxPiecesOnBoard  = 32;

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
    throw 1;
}

static const QPixmap & GetPixmapForPiece(const CChessPiece & piece)
{
    return GetPixmapForTypeColor(piece.GetType(), piece.GetColor());
}

CBoardGraphicsView::CBoardGraphicsView()
    : m_Board(new CChessBoard)
{
    m_Font.setPointSize(s_FontSize);
    m_Font.setBold(true);

    m_Scene = new QGraphicsScene();

    setScene(m_Scene);

    AddSquaresNumbersLetters();

    AddPiecesItems();

    UpdateBoardItems();
}

void CBoardGraphicsView::UpdateBoardItems()
{
    ResetBoardPiecesCache();

    auto * pieces = m_Board->GetPieces();

    std::size_t pieceItemIdx = 0;

    for (int r = 0; r < 8; ++r)
        for (int c = 0; c < 8; ++c)
        {
            auto & piece = pieces[r * 8 + c];
            if (piece.GetType() == CChessPiece::Type::None)
                continue;

            auto * item = m_AllPiecesItems[pieceItemIdx++];

            m_BoardPiecesCache[r][c] = item;

            item->setPixmap(GetPixmapForPiece(piece));

            item->setVisible(true);
        }

    for (std::size_t t = pieceItemIdx; t < s_MaxPiecesOnBoard; ++t)
        m_AllPiecesItems[t]->setVisible(false);
}

void CBoardGraphicsView::resizeEvent(QResizeEvent * event)
{
    __super::resizeEvent(event);

    if (m_SquaresItems.empty())
        return;

    assert(m_SquaresItems.size() == 64);

    const auto viewSize = size();

    const auto viewWidth  = static_cast<qreal>(viewSize.width());
    const auto viewHeight = static_cast<qreal>(viewSize.height());

    const auto availableWidth  = viewWidth  - s_BoardMarginLeft - s_BoardMarginRight;
    const auto availableHeight = viewHeight - s_BoardMarginTop  - s_BoardMarginBottom;

    const auto squareSide = std::min(availableWidth, availableHeight) / 8.0;

    const auto graphicsScale = squareSide / 333.0;

    const QFontMetrics fontMetrics(m_Font);

    const auto numberXPos = s_BoardMarginLeft / 2.0;

    const auto yOffset = (squareSide - static_cast<qreal>(fontMetrics.height())) / 2.0;

    const auto letterYPos = viewHeight - s_BoardMarginBottom;

    // squares

    for (std::size_t r = 0; r < 8; ++r)
    {
        const auto rowOffset = s_BoardMarginTop + squareSide * static_cast<qreal>(r);

        for (std::size_t c = 0; c < 8; ++c)
        {
            const auto colOffset = s_BoardMarginLeft + squareSide * static_cast<qreal>(c);

            m_SquaresItems[r * 8 + c]->setRect(colOffset, rowOffset, squareSide, squareSide);

            if (auto * item = m_BoardPiecesCache[r][c])
            {
                item->setPos(colOffset, rowOffset);
                item->setScale(graphicsScale);
            }
        }
    }

    m_BackgroundItem->setRect(0.0, 0.0, viewWidth, viewHeight);

    // numbers

    for (std::size_t i = 0; i < 8; ++i)
    {
        const auto rowOffset = s_BoardMarginTop + squareSide * static_cast<qreal>(i);

        m_SquaresNumbers[i]->setPos(numberXPos, rowOffset + yOffset);
    }

    // letters

    for (std::size_t i = 0; i < 8; ++i)
    {
        auto * letterText = m_SquaresLetters[i];

        const auto ww = fontMetrics.width(letterText->toPlainText());

        const auto colOffset = s_BoardMarginLeft + squareSide * static_cast<qreal>(i);

        const auto xOffset = (squareSide - static_cast<qreal>(ww)) / 2.0;

        letterText->setPos(colOffset + xOffset, letterYPos);
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
        m_AllPiecesItems.push_back(m_Scene->addPixmap(whiteQueenPixmap));

    ResetBoardPiecesCache();
}

void CBoardGraphicsView::ResetBoardPiecesCache()
{
    for (int r = 0; r < 8; ++r)
        for (int c = 0; c < 8; ++c)
            m_BoardPiecesCache[r][c] = nullptr;
}

} // namespace ChessProj