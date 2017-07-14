@echo off
cd %~dp0

REM find which supported VS version is installed (14, or 15)
set VSVersion=14
set CMakeGenerator=Visual Studio 14 2015 Win64
for %%a in (14 15) do (  
  reg query HKLM\SOFTWARE\Microsoft\VisualStudio\%%a.0\Setup\VS\community /reg:32 > %TEMP%\VSSetupInfo.txt 2>&1
  for /f "usebackq tokens=1,2,3* delims= " %%i in (%TEMP%\VSSetupInfo.txt) do (
    if "%%i"=="ProductDir" set VSVersion=%%a
  )
)

if "%VSVersion%" == "15" set CMakeGenerator=Visual Studio 15 2017 Win64
echo Found VS version %CMakeGenerator%

external\nuget\nuget.exe restore external/packages.config -PackagesDirectory external

if not EXIST build goto :mkbuild

rd /s /q build
if ERRORLEVEL 1 goto :nodelete

:mkbuild
mkdir build
if ERRORLEVEL 1 goto :nodelete

cd build
cmake -G "%CMakeGenerator%" ..
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