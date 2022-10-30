@echo off
set /p "build=Enter Version: "
mkdir releases

rem PC RELEASE

mkdir releases\Mad64Win_%build%
xcopy x64\release\mad64.exe releases\Mad64Win_%build%\
xcopy *.dll releases\Mad64Win_%build%\
xcopy readme.txt releases\Mad64Win_%build%\
xcopy data releases\Mad64Win_%build%\data\
7z a releases\Mad64Win_%build%.zip releases\Mad64_%build%

rem LINUX RELEASE

mkdir releases\Mad64Linux_%build%
xcopy x64\release\mad64 releases\Mad64Linux_%build%\
xcopy readme.txt releases\Mad64Linux_%build%\
xcopy data releases\Mad64Linux_%build%\data\
7z a releases\Mad64Linux_%build%.zip releases\Mad64_%build%
