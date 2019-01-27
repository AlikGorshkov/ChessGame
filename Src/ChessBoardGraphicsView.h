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

    void StartNewGame(const bool asWhite);

    void UpdateBoardItems();

    void EvaluatePosition();

protected:
    void resizeEvent(QResizeEvent * event) override;
    void mousePressEvent(QMouseEvent * event) override;
    void mouseReleaseEvent(QMouseEvent * event) override;
    void mouseMoveEvent(QMouseEvent * event) override;

private:
    void UpdateBoardGeometry();

    void UpdateBoardItemsPositions();

    void AddSquaresNumbersLetters();

    void AddPiecesItems();

    void ResetBoardPiecesCache();

    void ReturnLastMovedPieceBack();

    void SetZValueForItemToMove(const float zValue);

    CSquare GetSquareForPoint(const QPointF & pt) const;

    QPointF GetPosForSquare(const CSquare & square) const;

    QGraphicsScene *                    m_Scene;
    QGraphicsRectItem *                 m_BackgroundItem;
    std::vector<QGraphicsRectItem *>    m_SquaresItems;
    std::vector<QGraphicsTextItem *>    m_SquaresNumbers;
    std::vector<QGraphicsTextItem *>    m_SquaresLetters;
    CChessGame                          m_Game;

    std::vector<QGraphicsPixmapItem *>  m_AllPiecesItems;

    QGraphicsPixmapItem *               m_BoardPiecesCache[8][8];

    QFont                               m_Font;

    QRectF                              m_BoardRect;
    qreal                               m_SquareSide = 0.0;

    QPointF                             m_MousePosToPieceOffset;

    CSquare                             m_LastMousePressSquare;
};

} // namespace ChessProj