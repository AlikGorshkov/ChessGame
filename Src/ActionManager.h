#pragma once

#include <QAction>

namespace ChessProj
{

class CActionManager
{
public:
    enum class CommonAction
    {
        NewGameAsWhite,
        NewGameAsBlack,
        EvaluatePosition
    };

    static CActionManager & Instance();

    QAction * GetAction(const CommonAction act) const;

    void ReTranslate();

private:
    CActionManager();

    void AddAction(const CommonAction act, const QString & iconID);

    std::map<CommonAction, QPointer<QAction>>   m_Actions;
};

} // namespace ChessProj