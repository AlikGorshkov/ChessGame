#include "ActionManager.h"

#include <QApplication>
#include <QPointer>

namespace ChessProj
{

CActionManager::CActionManager()
{
    AddAction(CommonAction::NewGameAsWhite,   ":/UIRes/WhiteKing");
    AddAction(CommonAction::NewGameAsBlack,   ":/UIRes/BlackKing");
    AddAction(CommonAction::EvaluatePosition, ":/UIRes/Evaluate");

    ReTranslate();
}

CActionManager & CActionManager::Instance()
{
    static CActionManager self;
    return self;
}

void CActionManager::AddAction(const CommonAction act, const QString & iconID)
{
    QPointer<QAction> action(new QAction);

    action->setIcon(QIcon(iconID));

    m_Actions[act] = std::move(action);
}

QAction * CActionManager::GetAction(const CommonAction act) const
{
    auto it = m_Actions.find(act);
    if (it == m_Actions.end())
    {
        assert(false);

        return nullptr;
    }

    return it->second;
}

void CActionManager::ReTranslate()
{
    GetAction(CommonAction::NewGameAsWhite)->setText(QApplication::tr("New game as White"));
    GetAction(CommonAction::NewGameAsBlack)->setText(QApplication::tr("New game as Black"));
    GetAction(CommonAction::EvaluatePosition)->setText(QApplication::tr("Evaluate Position"));
}

} // namespace ChessProj