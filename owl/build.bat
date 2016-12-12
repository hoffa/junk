@echo off

set output=owl.lua

echo Building Owl... (%output%)
call :append main.lua begin
call :append input.lua
call :append screen.lua
call :append area.lua
call :append tile.lua
call :append player.lua
call :append object.lua
call :append event.lua
call :append path.lua end
echo Build finished.
pause
exit

:append
set file=src\%~1
echo Adding %file%...
if "%~2" equ "begin" (
    if not exist %output% (
        copy %file% %output% > nul
    ) else (
        xcopy /q %file% %output%
    )
) else (
    if exist %file% (
        copy /b %output%+%file% %output% > nul
    ) else (
        echo Cannot find %file%!
    )
)
if "%~2" neq "end" echo. >> %output% && echo. >> %output%
goto :eof