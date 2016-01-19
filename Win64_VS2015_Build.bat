:: check that necessary tools are in place
IF NOT EXIST "\Program Files (x86)\MSBuild\14.0\Bin\amd64\MSBuild.exe" (Goto :EOF)
IF NOT EXIST "\Program Files (x86)\CMake\bin\cmake.exe" (GOTO :EOF)

:: create the Visual Studio Win64 solution
rmdir /s /q build
mkdir build
cd build
"\Program Files (x86)\CMake\bin\cmake.exe" -G "Visual Studio 14 2015 Win64" ..

:: compile and test the Debug configuration
"\Program Files (x86)\MSBuild\14.0\Bin\amd64\MSBuild.exe" Layers.sln /p:Configuration=Debug
"\Program Files (x86)\CMake\bin\ctest.exe" -V -C Debug

:: compile the Release configuration
:: "\Program Files (x86)\MSBuild\14.0\Bin\amd64\MSBuild.exe" Layers.sln /p:Configuration=Release


