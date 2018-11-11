@echo off

SETLOCAL

cd ..
set ChessProjRoot=%CD%

SET QT_BIN_FOLDER=c:\Qt\5.11.2\msvc2017_64\bin
SET BIN_REL_FOLDER=%ChessProjRoot%\bin\release\
SET BIN_DBG_FOLDER=%ChessProjRoot%\bin\debug\

xcopy /E /Y %QT_BIN_FOLDER%\Qt5Core.dll %BIN_REL_FOLDER%
xcopy /E /Y %QT_BIN_FOLDER%\Qt5Gui.dll %BIN_REL_FOLDER%
xcopy /E /Y %QT_BIN_FOLDER%\Qt5Widgets.dll %BIN_REL_FOLDER%

xcopy /E /Y %QT_BIN_FOLDER%\Qt5Cored.dll %BIN_DBG_FOLDER%
xcopy /E /Y %QT_BIN_FOLDER%\Qt5Guid.dll %BIN_DBG_FOLDER%
xcopy /E /Y %QT_BIN_FOLDER%\Qt5Widgetsd.dll %BIN_DBG_FOLDER%

ENDLOCAL