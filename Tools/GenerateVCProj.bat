@echo off

SETLOCAL

cd ..
set ChessProjRoot=%CD%

call "c:\Program Files (x86)\Microsoft Visual Studio\2017\Community\VC\Auxiliary\Build\vcvars64.bat"
call "c:\Qt\5.11.2\msvc2017_64\bin\qtenv2.bat"

cd %ChessProjRoot%/src

qmake -t vcapp ChessProj.pro

ENDLOCAL
