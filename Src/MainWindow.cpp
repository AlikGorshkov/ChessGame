#include "MainWindow.h"
#include "ActionManager.h"
#include "MainToolBar.h"
#include "ChessBoardGraphicsView.h"

namespace ChessProj
{

CMainWindow::CMainWindow(QWidget * pParent)
    : QMainWindow(pParent)
{
    m_ToolBar = new CMainToolBar;

    addToolBar(m_ToolBar);

    setWindowIcon(QIcon(":/UIRes/BlackKnight"));

    setMinimumSize(QSize(700, 500));

    m_View = new CBoardGraphicsView();

    m_View->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_View->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    setCentralWidget(m_View);

    CActionManager & actMgr = CActionManager::Instance();

    connect(actMgr.GetAction(CActionManager::CommonAction::NewGameAsWhite), SIGNAL(triggered()), this, SLOT(StartNewGameAsWhite()));
    connect(actMgr.GetAction(CActionManager::CommonAction::NewGameAsBlack), SIGNAL(triggered()), this, SLOT(StartNewGameAsBlack()));
}

void CMainWindow::StartNewGameAsWhite()
{
    m_View->StartNewGame(true);
}

void CMainWindow::StartNewGameAsBlack()
{
    m_View->StartNewGame(false);
}

} // namespace ChessProj