@echo off
cls
echo "Start this Batch - Script from Qt-Console"
echo "R = Release"
echo "D = Debug"

set auswahl=
 set /p auswahl="Option: "




if "%auswahl%" == "R" GOTO RELEASE
if "%auswahl%" == "D" GOTO DEBUG

echo "Abort chose R or D ;)"
pause
exit

:DEBUG
	qmake -win32 I2PChat.pro
	make
	exit
:RELEASE
	qmake I2PChat_release.pro
	make
	exit
