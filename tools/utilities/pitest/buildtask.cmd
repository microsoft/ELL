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
set RPI_IPADDR=%4
pushd %ELL_SRC%

call %CONDA_PATH%\Scripts\activate.bat ell

echo ===================================== BUILD ==================================
call .\rebuild.cmd 14
if ERRORLEVEL 1 exit 1

echo ===================================== TEST ==================================

pushd %OUTPUT_PATH%
if NOT EXIST Test mkdir Test
cd Test
if EXIST pi3 rd /s /q pi3
popd

set PATH=%PATH%;%ELL_SRC%\external\OpenBLASLibs.0.2.19.3\build\native\x64\haswell\bin
echo Running Raspberry Pi test from %ELL_SRC%\build\tools\utilities\pitest\drivetest.py
python build\tools\utilities\pitest\drivetest.py %RPI_IPADDR% --outdir %OUTPUT_PATH%\Test
if ERRORLEVEL 1 exit 1
goto :eof

endlocal
popd