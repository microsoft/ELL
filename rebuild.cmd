@echo off

if not EXIST build goto :mkbuild

rd /s /q build
if ERRORLEVEL 1 goto :nodelete

:mkbuild
mkdir build
if ERRORLEVEL 1 goto :nodelete

cd build

set /p vs="Enter your Visual Studio version (2015/2017): "
if %vs% == 2017 goto :2017

if not %vs% == 2015 goto :builderror
cmake -G "Visual Studio 14 2015 Win64" -DPROCESSOR_HINT=haswell ..
if ERRORLEVEL 1 goto :nocmake
goto :build

:2017
cmake -G "Visual Studio 15 2017 Win64" -DPROCESSOR_HINT=haswell ..

:build
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