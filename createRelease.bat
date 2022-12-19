rem @echo off
set /p "build=Enter Version: "
mkdir releases
cd releases

rem PC RELEASE

mkdir Mad64Win_%build%
xcopy ..\x64\release\mad64.exe Mad64Win_%build%\
xcopy ..\readme.txt Mad64Win_%build%\
xcopy ..\font.otf Mad64Win_%build%\
xcopy ..\font.ttf Mad64Win_%build%\
xcopy ..\fontc64.ttf Mad64Win_%build%\
xcopy ..\*.dll Mad64Win_%build%\
xcopy ..\samples Mad64Win_%build%\samples\
del Mad64Win_%build%.zip
7z a Mad64Win_%build%.zip Mad64Win_%build%

pause