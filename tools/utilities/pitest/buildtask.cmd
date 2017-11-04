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
set RPI_CLUSTER=%4
pushd %ELL_SRC%

call %CONDA_PATH%\Scripts\activate.bat ell

echo ===================================== BUILD ==================================
call .\rebuild.cmd 14
if ERRORLEVEL 1 exit 1

echo ===================================== TEST ==================================
cd build
cmake .. -DRPI_CLUSTER=%RPI_CLUSTER%
if ERRORLEVEL 1 exit /B 1

ctest . --build-config release -VV
if ERRORLEVEL 1 exit /B 1
goto :eof

endlocal
popd