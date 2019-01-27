#pragma once

#include <QMainWindow>

namespace ChessProj
{

class CMainToolBar;
class CBoardGraphicsView;

class CMainWindow : public QMainWindow
{
    Q_OBJECT

public:
    CMainWindow(QWidget * parent = nullptr);

private slots:
    void StartNewGameAsWhite();
    void StartNewGameAsBlack();
    void EvaluatePosition();

private:
    CMainToolBar *       m_ToolBar;
    CBoardGraphicsView * m_View;
};

} // namespace ChessProj