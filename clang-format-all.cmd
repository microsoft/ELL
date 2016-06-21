@echo off

:: Header files
for /r %%f in (*.h) do (
    clang-format %%f > clang-format-temp
    del %%f
    type clang-format-temp > %%~pnxf
    echo %%~pnxf
    del clang-format-temp
)

:: tcc files
for /r %%f in (*.tcc) do (
    clang-format %%f > clang-format-temp
    del %%f
    type clang-format-temp > %%~pnxf
    echo %%~pnxf
    del clang-format-temp
)

:: Source files
for /r %%f in (*.cpp) do (
    clang-format %%f > clang-format-temp
    del %%f
    type clang-format-temp > %%~pnxf
    echo %%~pnxf
    del clang-format-temp
)

