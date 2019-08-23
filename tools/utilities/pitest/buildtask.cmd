REM @echo off
REM
REM   Project:  Embedded Learning Library (ELL)
REM   File:     buildtask.cmd
REM   Authors:  Chris Lovett
REM
REM   Requires: Anaconda or Miniconda with Python 3
REM
Setlocal

REM first 3 arguments are fixed
set ELL_SRC=%1
set CONDA_PATH=%2
set VS_VERSION=%3

shift
shift
shift

REM these arguments are optional named arguments
set RPI_CLUSTER=
set RPI_PASSWORD=
set RPI_APIKEY=
set TEST_MODELS_REPO=
set CNTK=OFF
set ONNX=OFF
set SUBSET=

:parse
if "%1"=="" goto :start
set name=%1
shift
set value=%1
shift
set %name:~1%=%value%
goto :parse

:start
if not EXIST "%ELL_SRC%" goto :usage
if not EXIST "%CONDA_PATH%" goto :usage

pushd %ELL_SRC%

call %CONDA_PATH%\Scripts\activate.bat py36

REM this needs to be run under VSTS agent user account (NTSERVICE account)
git lfs install

echo ===================================== BUILD ==================================
call .\rebuild.cmd %VS_VERSION% /ONNX %ONNX% /CNTK %CNTK%
if ERRORLEVEL 1 exit /B 1

cd build
call openblas.cmd

if "%RPI_CLUSTER%"=="" goto :test
echo ===================================== CMAKE with additional options ==================================
if "%TEST_MODELS_REPO%"=="" set TEST_MODELS_REPO=https://github.com/Microsoft/ell-test-models
echo cmake .. -DRPI_CLUSTER=%RPI_CLUSTER% -DRPI_PASSWORD=%RPI_PASSWORD% -DRPI_KEY=%RPI_APIKEY% -DTEST_MODELS_REPO=%TEST_MODELS_REPO% -DCNTK=%CNTK% -DONNX=%ONNX%
cmake .. -DRPI_CLUSTER=%RPI_CLUSTER% -DRPI_PASSWORD=%RPI_PASSWORD% -DRPI_KEY=%RPI_APIKEY% -DTEST_MODELS_REPO=%TEST_MODELS_REPO% -DCNTK=%CNTK% -DONNX=%ONNX%
if ERRORLEVEL 1 exit /B 1
goto :test

:test
echo ===================================== TEST ==================================
if "%SUBSET%"=="" goto :fulltest

ctest . --build-config release -R %SUBSET% -VV --timeout 600 -T Test
if ERRORLEVEL 1 exit /B 1

if not EXIST .\tools\utilities\pitest\test\pi3 goto :pitesterror
goto :done

:pitesterror
echo ### Error: it looks like the pitest didn't run...
exit /B 1

:fulltest
ctest . --build-config release -VV --timeout 600 -T Test
if ERRORLEVEL 1 exit /B 1
goto :done

:done
endlocal
popd
goto :eof

:usage
echo "usage: buildtask.cmd ell_repo_path anaconda_env_path vs_version options..."
echo "options: "
echo "   /ONNX [ON|OFF]"
echo "   /CNTK [ON|OFF]"
echo "   /TEST_MODELS_REPO  URL_of_ell_test_models"
echo "   /RPI_CLUSTER URL_of_pi_cluster_manager"
echo "   /RPI_PASSWORD password"
echo "   /RPI_APIKEY api_key"
exit /B 1
