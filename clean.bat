@echo off

rmdir /s /q "%~dp0bin"
rmdir /s /q "%~dp0build"
del   /s /q "%~dp0*.sln"
del   /s /q "%~dp0*.vcxproj*"
