#pragma once

#include <QMainWindow>

namespace ChessProj
{

class CBoardGraphicsView;

class CMainWindow : public QMainWindow
{
    Q_OBJECT

public:
    CMainWindow(QWidget * parent = nullptr);

private:
    CBoardGraphicsView * m_View;
};

} // namespace ChessProj