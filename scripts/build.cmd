@echo off
Setlocal EnableDelayedExpansion

cd %~dp0
cd ..
if EXIST external\nuget.exe goto :restore
curl --output external\nuget.exe https://dist.nuget.org/win-x86-commandline/latest/nuget.exe
if ERRORLEVEL 1 goto :nocurl

:restore
external\nuget restore external/packages.config -PackagesDirectory external
if ERRORLEVEL 1 goto :norestore

REM find which supported VS version is installed
set DEBUG=0
set Vs15=0
set Vs15Path=
set Vs16=0
set Vs16Path=
set NOPYTHON=
set STRICT=
set ONNX=OFF
set CNTK=OFF
set TEST_MODELS_REPO=%GIT_REPO%
set VsVer=
if "%TEST_MODELS_REPO%"=="" set TEST_MODELS_REPO=https://github.com/Microsoft/ell-test-models
:parse
if "%1" == "" goto :step2
if "%1" == "/debug" set DEBUG=1
if "%1" == "/strict" set STRICT=-D STRICT_MODE=ON
if "%1" == "/nopython" set NOPYTHON=-D DISABLE_PYTHON=ON
if "%1" == "/ONNX" set ONNX=%2
if "%1" == "/CNTK" set CNTK=%2
if "%1" == "15" set VsVer=15
shift
goto :parse

:step2
set installationPath=
set InstallationVersion=
for /f "usebackq tokens=1* delims=: " %%i in (`external\vswhere.2.6.7\tools\vswhere.exe -prerelease -version "[15.7,17.0)"`) do (
  if "!DEBUG!"=="1" echo Found VS version %%i %%j
  if /i "%%i"=="installationVersion" (
    set VERSION=%%j
    set VER=!VERSION:~0,2!
    if "!VER!"=="15" set Vs15=1&& set Vs15Path=!installationPath! && echo Found VS 2017
    if "!VER!"=="16" set Vs16=1&& set Vs16Path=!installationPath! && echo Found VS 2019
  )
  if /i "%%i"=="installationPath" (
    set installationPath=%%j
  )
)

if "!VsVer!" == "15" goto :skip16
if "!Vs16!" == "1" goto :vs16
:skip16
if "!Vs15!" == "1" goto :vs15
goto :NoCompatibleVsInstall

:vs16
echo Selecting VS 2019
set CMakeGenerator=Visual Studio 16 2019
set CMakeArgs=-A x64 -T host=x64
REM put the VS 2019 version of cmake ahead of the list so we use it.
set PATH=!Vs16Path!\Common7\IDE\CommonExtensions\Microsoft\CMake\CMake\bin;!PATH!
if "%VisualStudioVersion%"=="" call "!Vs16Path!\Common7\Tools\VsDevCmd.bat" -no_logo -arch=amd64 -host_arch=amd64
echo %CMakeGenerator%> %TEMP%\ELL_BUILD_VS16
goto :selected

:vs15
echo Selecting VS 2017
set CMakeGenerator=Visual Studio 15 2017 Win64
set CMakeArgs=-T host=x64
REM put the VS 2017 version of cmake ahead of the list so we use it.
set PATH=!Vs15Path!\Common7\IDE\CommonExtensions\Microsoft\CMake\CMake\bin;!PATH!
if "%VisualStudioVersion%"=="" call "!Vs15Path!\Common7\Tools\VsDevCmd.bat" -no_logo -arch=amd64 -host_arch=amd64
echo %CMakeGenerator%> %TEMP%\ELL_BUILD_VS15
goto :selected

:selected
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
echo cmake -G "!CMakeGenerator!" %CMakeArgs% !STRICT! !NOPYTHON! -D CNTK=%CNTK% -D ONNX=%ONNX% -D "TEST_MODELS_REPO=!TEST_MODELS_REPO!" ..
cmake -G "!CMakeGenerator!" %CMakeArgs% "!STRICT!" "!NOPYTHON!" -D CNTK=%CNTK% -D ONNX=%ONNX% -D "TEST_MODELS_REPO=!TEST_MODELS_REPO!" ..
goto :buildit

:buildit
set procs=%ELL_BUILD_PROCS%
if "%procs%"=="" set /a procs=%NUMBER_OF_PROCESSORS% - 1
if "%procs%"=="0" set procs=1
echo Building with %procs% processes

cmake --build . --config Release -- /m:%procs% /verbosity:minimal
if ERRORLEVEL 1 goto :builderror

if NOT "!NOPYTHON!" == "" goto :eof

cmake --build . --target _ELL_python --config Release -- /m:%procs% /verbosity:minimal
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
