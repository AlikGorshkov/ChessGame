#include "MainWindow.h"
#include "ChessBoardGraphicsView.h"

namespace ChessProj
{

CMainWindow::CMainWindow(QWidget * pParent)
    : QMainWindow(pParent)
{
    setWindowIcon(QIcon(":/UIRes/BlackKnight"));

    setMinimumSize(QSize(700, 500));

    m_View = new CBoardGraphicsView();

    m_View->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_View->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    setCentralWidget(m_View);
}

} // namespace ChessProj