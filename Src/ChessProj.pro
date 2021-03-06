QT      += core gui widgets
TARGET   = ChessProj
TEMPLATE = app

CONFIG(debug, debug|release) {
        DESTDIR = ../bin/debug/
} else {
        DESTDIR = ../bin/release/
}

SOURCES     +=  ChessProj.cpp                   \
                MainWindow.cpp                  \
                MainToolBar.cpp                 \
                ActionManager.cpp               \
                ChessBoardGraphicsView.cpp      \
                ChessBoard.cpp                  \
                ChessGame.cpp                   \
                ChessMove.cpp                   \
                ChessPiece.cpp

HEADERS     +=  MainWindow.h                    \
                MainToolBar.h                   \
                ActionManager.h                 \
                ChessBoardGraphicsView.h        \
                ChessBoard.h                    \
                ChessGame.h                     \
                ChessMove.h                     \
                ChessPiece.h

RESOURCES   =   ChessProj.qrc

QMAKE_CXXFLAGS += /MP