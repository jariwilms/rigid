@echo off

setlocal

set "PREMAKE_FILE=premake5.lua"
if /i "%~1"=="-s" (set "PREMAKE_FILE=premake5-sandbox.lua")

CALL tools\premake5\premake5.exe --file=%PREMAKE_FILE% vs2022

endlocal