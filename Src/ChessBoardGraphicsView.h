#pragma once

#include <memory>

#include <QGraphicsView>

class QGraphicsRectItem;
class QGraphicsTextItem;
class QGraphicsPixmapItem;

namespace ChessProj
{

class CChessBoard;

class CBoardGraphicsView : public QGraphicsView
{
Q_OBJECT

public:
    CBoardGraphicsView();

    void UpdateBoardItems();

protected:
    void resizeEvent(QResizeEvent * event);

private:
    void AddSquaresNumbersLetters();

    void AddPiecesItems();

    void ResetBoardPiecesCache();

    QGraphicsScene *                    m_Scene;
    QGraphicsRectItem *                 m_BackgroundItem;
    std::vector<QGraphicsRectItem *>    m_SquaresItems;
    std::vector<QGraphicsTextItem *>    m_SquaresNumbers;
    std::vector<QGraphicsTextItem *>    m_SquaresLetters;
    std::unique_ptr<CChessBoard>        m_Board;

    std::vector<QGraphicsPixmapItem *>  m_AllPiecesItems;

    QGraphicsPixmapItem *               m_BoardPiecesCache[8][8];

    QFont                               m_Font;
};

} // namespace ChessProj