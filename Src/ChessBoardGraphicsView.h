#pragma once

#include "ChessGame.h"

#include <QGraphicsView>

class QGraphicsRectItem;
class QGraphicsTextItem;
class QGraphicsPixmapItem;

namespace ChessProj
{

class CBoardGraphicsView : public QGraphicsView
{
Q_OBJECT

public:
    CBoardGraphicsView();

    void UpdateBoardItems();

protected:
    void resizeEvent(QResizeEvent * event) override;
    void mousePressEvent(QMouseEvent * event) override;
    void mouseReleaseEvent(QMouseEvent * event) override;
    void dragEnterEvent(QDragEnterEvent * event) override;
    void dragLeaveEvent(QDragLeaveEvent * event) override;
    void dragMoveEvent(QDragMoveEvent * event) override;

private:
    void UpdateBoardItemsPositions();

    void AddSquaresNumbersLetters();

    void AddPiecesItems();

    void ResetBoardPiecesCache();

    CSquare GetSquareForPoint(const QPointF & pt) const;

    QPointF GetPosForSquare(const CSquare & square) const;

    QGraphicsScene *                    m_Scene;
    QGraphicsRectItem *                 m_BackgroundItem;
    QGraphicsRectItem *                 m_SelectionItem;
    std::vector<QGraphicsRectItem *>    m_SquaresItems;
    std::vector<QGraphicsTextItem *>    m_SquaresNumbers;
    std::vector<QGraphicsTextItem *>    m_SquaresLetters;
    CChessGame                          m_Game;

    std::vector<QGraphicsPixmapItem *>  m_AllPiecesItems;

    QGraphicsPixmapItem *               m_BoardPiecesCache[8][8];

    QFont                               m_Font;

    QRectF                              m_BoardRect;
    qreal                               m_SquareSide = 0.0;

    CSquare                             m_LastMousePressSquare;
    CSquare                             m_SelectedSquare;
};

} // namespace ChessProj