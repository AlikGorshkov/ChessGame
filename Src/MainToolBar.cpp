#include "MainToolBar.h"
#include "ActionManager.h"

namespace ChessProj
{

CMainToolBar::CMainToolBar()
    : QToolBar()
{
    CActionManager & actMgr = CActionManager::Instance();

    addAction(actMgr.GetAction(CActionManager::CommonAction::NewGameAsWhite));
    addAction(actMgr.GetAction(CActionManager::CommonAction::NewGameAsBlack));

    setFloatable(false);
    setMovable(false);
}

} // namespace ChessProj