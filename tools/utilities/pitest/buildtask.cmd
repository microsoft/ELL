REM 
REM   Project:  Embedded Learning Library (ELL)
REM   File:     buildtask.cmd
REM   Authors:  Chris Lovett
REM 
REM   Requires: Anaconda2 with Python 3
REM 

setlocal
set ELL_SRC=%1
set CONDA_PATH=%2
set VS_VERSION=%3
set RPI_CLUSTER=%4
set PASSWORD=%5
set KEY=%6
pushd %ELL_SRC%

call %CONDA_PATH%\Scripts\activate.bat ell

echo ===================================== BUILD ==================================
call .\rebuild.cmd %VS_VERSION%
if ERRORLEVEL 1 exit 1

cd build

if "%4"=="" goto :fulltest

echo ===================================== TEST ==================================
cmake .. -DRPI_CLUSTER=%RPI_CLUSTER% -DRPI_PASSWORD=%PASSWORD% -DRPI_KEY=%KEY%
if ERRORLEVEL 1 exit /B 1

ctest . --build-config release -R pitest_test -VV
if ERRORLEVEL 1 exit /B 1
goto :eof
goto :done

:fulltest
ctest . --build-config release
if ERRORLEVEL 1 exit /B 1
goto :eof

:done
endlocal
popd