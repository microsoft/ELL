rem Script to run swig standalone without CMake on Windows
rem Generates the swig python 3 wrappers in a sub folder called "output"
rem Pre-requisites: swig.exe in the PATH environment variable

rem Generate example map
..\..\..\build\bin\Debug\makeExamples.exe -o map

rem Generate model IR, header, and SWIG interfaces
..\..\..\build\bin\Debug\compile.exe -imap ELL_step10.map -cfn Step10 -m clockStep --ir --header --swig --blas

rem Run swig
mkdir output
swig.exe -python -c++ -Fmicrosoft -py3 -outdir output -c++ ^
-I../../../interfaces/common/include ^
-I../../../interfaces/common ^
-I../../../libraries/emitters/include ^
-o output/ELL_step10PYTHON_wrap.cxx ^
ELL_step10.i
