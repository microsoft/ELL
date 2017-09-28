@echo off
Setlocal EnableDelayedExpansion 

cd %~dp0

if EXIST external\nuget.exe goto :restore
curl --output external\nuget.exe https://dist.nuget.org/win-x86-commandline/latest/nuget.exe
if ERRORLEVEL 1 goto :nocurl
 
:restore
external\nuget restore external/packages.config -PackagesDirectory external
if ERRORLEVEL 1 goto :norestore

REM find which supported VS version is installed
set Vs14=
set Vs15=

if "%1"=="14" (
  set Vs14=1
  goto :step2
)
if "%1"=="15" (
  set Vs15=1
  goto :step2
)

set InstallationVersion=
for /f "usebackq tokens=1* delims=: " %%i in (`external\vswhere.2.1.3\tools\vswhere.exe -legacy`) do (
  if /i "%%i"=="installationVersion" (
    set VERSION=%%j
    set VER=!VERSION:~0,2!
    if "!VER!"=="14" set Vs14=1
    if "!VER!"=="15" set Vs15=1
  )
)

if "!Vs14! and !Vs15!" == "0 and 0" goto :NoCompatibleVsInstall
:step2
set CMakeGenerator=Visual Studio 14 2015 Win64
if "!Vs14! and !Vs15!" == "0 and 1" (
    set CMakeGenerator=Visual Studio 15 2017 Win64
)
if "!Vs14! and !Vs15!" == "1 and 1" (
    set /p id="Use VS 2017 ? "
    if /i "!id!"=="y" set CMakeGenerator=Visual Studio 15 2017 Win64
    if /i "!id!"=="yes" set CMakeGenerator=Visual Studio 15 2017 Win64
)

echo Using VS version !CMakeGenerator!

:mkbuild
if not exist build mkdir build
if ERRORLEVEL 1 goto :nodelete

cd build
cmake -G "!CMakeGenerator!" ..
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
goto :eof

:nocurl
echo is curl installed?  If you are using anaconda, try "conda install curl"
goto :eof

:norestore
echo nuget restore failed
goto :eof

:NoCompatibleVsInstall
echo Could not find VS 2015 or 2017 installation
goto :eof