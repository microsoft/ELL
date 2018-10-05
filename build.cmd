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
set Vs15=0
set DEBUG=0
set Vs15Path=
set NOPYTHON=
set STRICT=
set ONNX=OFF
set CNTK=OFF
set TEST_MODEL_REPO=%GIT_REPO%
if "%TEST_MODEL_REPO%"=="" set TEST_MODEL_REPO=https://github.com/Microsoft/ell-test-models
:parse
if "%1" == "" goto :step2
if "%1" == "/debug" set DEBUG=1
if "%1" == "/strict" set STRICT=-DSTRICT_MODE=ON
if "%1" == "/nopython" set NOPYTHON=-DDISABLE_PYTHON=ON
if "%1" == "/ONNX" set ONNX=%2
if "%1" == "/CNTK" set CNTK=%2
shift
goto :parse

:step2
set installationPath=
set InstallationVersion=
for /f "usebackq tokens=1* delims=: " %%i in (`external\vswhere.2.1.3\tools\vswhere.exe -version "[15.7,16.0)"`) do (
  if "!DEBUG!"=="1" echo Found VS version %%i %%j
  if /i "%%i"=="installationVersion" (
    set VERSION=%%j
    set VER=!VERSION:~0,2!
    if "!VER!"=="15" set Vs15=1&& set Vs15Path=!installationPath!
  )
  if /i "%%i"=="installationPath" (
    set installationPath=%%j
  )
)

if "!Vs15!" == "0" goto :NoCompatibleVsInstall

set CMakeGenerator=Visual Studio 15 2017 Win64
REM put the VS 2017 version of cmake ahead of the list so we use it.
set PATH=!Vs15Path!\Common7\IDE\CommonExtensions\Microsoft\CMake\CMake\bin;!PATH!
if "%VisualStudioVersion%"=="" call "!Vs15Path!\Common7\Tools\VsDevCmd.bat" -no_logo -arch=amd64 -host_arch=amd64
echo %CMakeGenerator%> %TEMP%\ELL_BUILD_VS15

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
echo cmake -G "!CMakeGenerator!" !STRICT! !NOPYTHON! -DCNTK=%CNTK% -DONNX=%ONNX% "-DGIT_REPO=!TEST_MODEL_REPO!" ..
cmake -G "!CMakeGenerator!" "!STRICT!" "!NOPYTHON!" -DCNTK=%CNTK% -DONNX=%ONNX% "-DGIT_REPO=!TEST_MODEL_REPO!" ..
goto :buildit

:buildit
for /F "tokens=* USEBACKQ" %%i in (
  `powershell -nologo -noprofile -command "$proc = $env:NUMBER_OF_PROCESSORS - 1; if ($proc -lt 1) { $proc = 1; }; echo $proc"`
) do (
  set procs=%%i
)
echo Building with !procs! processes

cmake --build . --config Release -- /m:!procs! /verbosity:minimal
if ERRORLEVEL 1 goto :builderror

if NOT "!NOPYTHON!" == "" goto :eof

cmake --build . --target _ELL_python --config Release -- /m:!procs! /verbosity:minimal
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
echo Could not find VS 2017 installation (minimum version 15.7)
exit /B 1
