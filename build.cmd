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
set Vs14=0
set Vs15=0
set UseVs14=0
set UseVs15=0
set DEBUG=0
set Vs14Path=
set Vs15Path=
set NOPYTHON=
set STRICT=
:parse
if "%1" == "" goto :step2
if "%1"=="14" set UseVs14=1
if "%1"=="15" set UseVs15=1
if "%1" == "/debug" set DEBUG=1
if "%1" == "/strict" set STRICT=-DSTRICT_MODE=ON
if "%1" == "/nopython" set NOPYTHON=-DDISABLE_PYTHON=ON
shift
goto :parse

:step2
set installationPath=
set InstallationVersion=
for /f "usebackq tokens=1* delims=: " %%i in (`external\vswhere.2.1.3\tools\vswhere.exe -legacy`) do (
  if "!DEBUG!"=="1" echo Found VS version %%i %%j
  if /i "%%i"=="installationVersion" (
    set VERSION=%%j
    set VER=!VERSION:~0,2!
    if "!VER!"=="14" set Vs14=1&& set Vs14Path=!installationPath!
    if "!VER!"=="15" set Vs15=1&& set Vs15Path=!installationPath!
  )
  if /i "%%i"=="installationPath" (
    set installationPath=%%j
  )
)

if "!Vs14! and !Vs15!" == "0 and 0" goto :NoCompatibleVsInstall
if "!Vs14! and !UseVs14!" == "0 and 1" goto :NoVs14
if "!Vs15! and !UseVs15!" == "0 and 1" goto :NoVs15

set CMakeGenerator=Visual Studio 14 2015 Win64

if "!UseVs14! and !UseVs15! and !Vs14Path!" == "0 and 0 and " (
    set UseVs15=1
) 

if "!UseVs15!" == "1" (
    set CMakeGenerator=Visual Studio 15 2017 Win64
    REM put the VS 2017 version of cmake ahead of the list so we use it.
    set PATH=!Vs15Path!\Common7\IDE\CommonExtensions\Microsoft\CMake\CMake\bin;!PATH!
    if "%VisualStudioVersion%"=="" call "!Vs15Path!\Common7\Tools\VsDevCmd.bat"
)
if "!UseVs14!" == "1" (
    if "%VisualStudioVersion%"=="" call "!Vs14Path!\Common7\Tools\VsDevCmd.bat"
)

if "!UseVs14! and !UseVs15! and !Vs14! and !Vs15!" == "0 and 0 and 1 and 1" (
    set /p id="Use VS 2017 ? "
    if /i "!id!"=="y" set CMakeGenerator=Visual Studio 15 2017 Win64
    if /i "!id!"=="yes" set CMakeGenerator=Visual Studio 15 2017 Win64
)

if "!DEBUG!"=="1" set
where cl.exe
echo Using VS version !CMakeGenerator!
cmake --version
if ERRORLEVEL 1 goto :nocmake

:mkbuild
if not exist build mkdir build
if ERRORLEVEL 1 goto :nodelete

if "!DEBUG!"=="1" dir "%VCToolsInstallDir%\bin\Hostx86\x86\"
cd build
echo cmake -G "!CMakeGenerator!" "!STRICT!" "!NOPYTHON!" ..
cmake -G "!CMakeGenerator!" "!STRICT!" "!NOPYTHON!" ..
if ERRORLEVEL 1 goto :cmakerror
goto :buildit

:cmakerror
REM try specifying the compiler
set CPATH=%VCToolsInstallDir:\=/%
echo %CPATH%
cmake -G "!CMakeGenerator!" "!STRICT!" "-DCMAKE_C_COMPILER=%CPATH%bin/Hostx86/x86/cl.exe" "-DCMAKE_CXX_COMPILER=%CPATH%bin/Hostx86/x86/cl.exe" ..
if ERRORLEVEL 1 goto :nocmake

:buildit
set procs=1
for /f "usebackq tokens=1*"  %%i in (`nproc`) do (
   set /a procs=%%i - 1
   echo procs=!procs!
)

cmake --build . --config Release -- /m:!procs!
if ERRORLEVEL 1 goto :builderror

if NOT "!NOPYTHON!" == "" goto :eof

cmake --build . --target _ELL_python --config Release -- /m:!procs!
if ERRORLEVEL 1 goto :builderror

goto :eof

:nodelete
echo Error deleting 'build' folder, is it locked?
exit /B 1

:builderror
exit /B 1

:nocurl
echo is curl installed?  If you are using anaconda, try "conda install curl"
exit /B 1

:norestore
echo nuget restore failed
exit /B 1

:NoCompatibleVsInstall
echo Could not find VS 2015 or 2017 installation
exit /B 1

:NoVs14
echo Requested we use VS 2015 but it was not found
exit /B 1

:NoVs15
echo Requested we use VS 2017 but it was not found
exit /B 1

:nocmake
echo Cmake returned an erorr or was not found
if "!DEBUG!"=="1"  type D:/a/1/s/build/CMakeFiles/CMakeOutput.log
exit /B 1