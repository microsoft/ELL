@echo off

if not EXIST build goto :mkbuild

rd /s /q build
if ERRORLEVEL 1 goto :nodelete

:mkbuild
mkdir build
if ERRORLEVEL 1 goto :nodelete

cd build
cmake -G "Visual Studio 14 2015 Win64" ..
if ERRORLEVEL 1 goto :nocmake

cmake --build . --config Release
if ERRORLEVEL 1 goto :builderror

cmake --build . --target _ELL_python --config Release

goto :eof

:nodelete
echo Error deleting 'build' folder, is it locked?
goto :eof

:nocmake
echo cmake step failed
goto :eof

:builderror