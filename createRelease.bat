@echo off
set /p "build=Enter Version: "
mkdir releases
mkdir releases\Mad64_%build%
xcopy x64\release\mad64.exe releases\Mad64_%build%\
xcopy *.dll releases\Mad64_%build%\
xcopy readme.txt releases\Mad64_%build%\
xcopy data releases\Mad64_%build%\data\
7z a releases\Mad64_%build%.zip releases\Mad64_%build%

