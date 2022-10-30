rem @echo off
set /p "build=Enter Version: "
mkdir releases
cd releases

rem PC RELEASE

mkdir Mad64Win_%build%
xcopy ..\x64\release\mad64.exe Mad64Win_%build%\
xcopy ..\readme.txt Mad64Win_%build%\
xcopy ..\*.dll Mad64Win_%build%\
xcopy ..\samples Mad64Win_%build%\samples\
7z a Mad64Win_%build%.zip Mad64Win_%build%

rem LINUX RELEASE

mkdir Mad64Linux_%build%
xcopy E:\UbuntuShared\mad64 Mad64Linux_%build%\
xcopy ..\readme.txt Mad64Linux_%build%\
xcopy ..\samples Mad64Linux_%build%\samples\
7z a Mad64Linux_%build%.zip Mad64Linux_%build%

pause