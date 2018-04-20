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
set OUTPUT_PATH=%3
set VS_VERSION=%4
set RPI_CLUSTER=%5
set PASSWORD=%6
set KEY=%7
pushd %ELL_SRC%

call %CONDA_PATH%\Scripts\activate.bat ell

echo ===================================== BUILD ==================================
call .\rebuild.cmd %VS_VERSION%
if ERRORLEVEL 1 exit 1

echo ===================================== TEST ==================================
cd build
cmake .. -DRPI_CLUSTER=%RPI_CLUSTER% -DRPI_PASSWORD=%PASSWORD% -DRPI_KEY=%KEY%
if ERRORLEVEL 1 exit /B 1

ctest . --build-config release -VV
if ERRORLEVEL 1 exit /B 1
goto :eof

endlocal
popd