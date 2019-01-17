@echo off
pushd %~dp0
set ext_path=%~dp0external
set skip_update=false
set dev_cmd=false

:ParseArgs
if "%1"=="" (
    goto Continue
) else (
    if "%1" equ "-skip_update" (
        set skip_update=true
    ) else if "%1" equ "-dev_cmd" (
        set dev_cmd=true
    ) else (
        goto PrintUsage
    )
    shift
    goto ParseArgs
)
:Continue

setlocal EnableDelayedExpansion

if %skip_update% equ true (
    goto FinishChecking
)

echo === Setup Nuget ===

REM Check Nuget version >= 3.5
echo Checking for Nuget...
goto CheckNuget
:NugetAvailable
powershell "%ext_path%\nuget help | select -First 1"
echo.

echo === Setup MiniConda ===

REM Check for conda version >= 4.5
echo Checking for Conda...
goto CheckConda
:CondaAvailable
powershell "conda --version | select -First 1"
echo.

:FinishChecking

REM Check for python version
for /f "tokens=1 delims= " %%G in ('powershell "conda info --envs"') do (
    if "%%G" equ "py36" (
        endlocal
        echo == Activate conda py36 environment ===
        call activate.bat py36
        goto Python36Available
    )
)

echo === Creating py36 environment ===
call conda.bat create -n py36 pip python=3.6 -y

echo == Activate conda py36 environment ===
call activate.bat py36
echo Activated, now running pip...
pip install -r requirements.txt

:Python36Available
python --version
echo.
setlocal EnableDelayedExpansion

if %dev_cmd% equ true (
    REM Start a new window for VS dev cmd
    for /f "delims=" %%i in ('%ext_path%\vswhere.2.1.3\tools\vswhere.exe -prerelease -latest -property installationPath') do (
        if exist "%%i\Common7\Tools\vsdevcmd.bat" (
            echo == Load VS Developer Command Prompt ==
            start %comspec% /k "%%i\Common7\Tools\vsdevcmd.bat"
        )
    )
)

goto:eof

:CheckNuget
for /f "tokens=3 delims= " %%G in ('powershell "%ext_path%\nuget help | select -First 1"') do set nuget_ver=%%G
for /f "tokens=1,2 delims=." %%G in ('echo %nuget_ver%') do (
    if %%G lss 3 (
        goto InstallNuget
    ) else if %%G equ 3 (
        if %%H lss 5 (
            goto InstallNuget
        )
    ) else (
        goto NugetAvailable
    )
)

:InstallNuget
REM Get nuget
echo getting newest nuget...
curl -o %ext_path%\nuget.exe --location https://dist.nuget.org/win-x86-commandline/latest/nuget.exe
echo.

REM Update all nuget packages
echo installing required nuget packages
%ext_path%\nuget.exe restore %ext_path%\packages.config -PackagesDirectory %ext_path%
echo.

REM Use PROCESSOR_IDENTIFIER to detect Haswell vs SandyBridge
REM According to https://software.intel.com/en-us/articles/intel-architecture-and-processor-identification-with-cpuid-model-and-family-numbers
REM SandyBridge should have model number of 0x2A(42) and 0x2D(45)
REM PROCESSOR_IDENTIFIER example: Intel64 Family 6 Model 63 Stepping 2, GenuineIntel -> model 63 != SandyBridge
for /f "tokens=5 delims= " %%G in ('echo %PROCESSOR_IDENTIFIER%') do (
    if %%G equ 42 (
        goto SandyBridge
    ) else if %%G equ 45 (
        goto SandyBridge
    ) else (
        goto Haswell
    )
)

:SandyBridge
endlocal
if %PROCESSOR_ARCHITECTURE% == AMD64 (
    set PATH=%PATH%;%ext_path%\OpenBLASLibs.0.2.19.3\build\native\x64\sandybridge\bin
    setx PATH "%ext_path%\OpenBLASLibs.0.2.19.3\build\native\x64\sandybridge\bin;%PATH%" /M
) else (
    set PATH=%PATH%;%ext_path%\OpenBLASLibs.0.2.19.3\build\native\x86\sandybridge\bin
    setx PATH "%ext_path%\OpenBLASLibs.0.2.19.3\build\native\x86\sandybridge\bin;%PATH%" /M
)
goto AddNugetPath

:Haswell
endlocal
if %PROCESSOR_ARCHITECTURE% == AMD64 (
    set PATH=%PATH%;%ext_path%\OpenBLASLibs.0.2.19.3\build\native\x64\haswell\bin
    setx PATH "%ext_path%\OpenBLASLibs.0.2.19.3\build\native\x64\haswell\bin;%PATH%" /M
) else (
    set PATH=%PATH%;%ext_path%\OpenBLASLibs.0.2.19.3\build\native\x86\haswell\bin
    setx PATH "%ext_path%\OpenBLASLibs.0.2.19.3\build\native\x86\haswell\bin;%PATH%" /M
)
goto AddNugetPath

:AddNugetPath
set PATH=%PATH%;%ext_path%\Doxygen.1.8.13\tools;%ext_path%\vswhere.2.1.3\tools;%ext_path%\swigwintools.3.0.12\tools\swigwin-3.0.12;%ext_path%\LLVMNativeWindowsLibs.x64.6.0.1\llvm-6.0\bin
setx PATH "%ext_path%\Doxygen.1.8.13\tools;%ext_path%\vswhere.2.1.3\tools;%ext_path%\swigwintools.3.0.12\tools\swigwin-3.0.12;%ext_path%\LLVMNativeWindowsLibs.x64.6.0.1\llvm-6.0\bin;%PATH%" /M
setlocal EnableDelayedExpansion
goto NugetAvailable

:CheckConda
for /f "tokens=3 delims= " %%G in ('powershell "conda --version | select -First 1"') do set conda_ver=%%G
for /f "tokens=1,2 delims=." %%G in ('echo %conda_ver%') do (
    if %%G lss 4 (
        goto InstallConda
    ) else if %%G equ 4 (
        if %%H lss 3 (
            goto InstallConda
        )
    ) else (
        goto CondaAvailable
    )
)

:InstallConda
echo getting newest conda (this will take couple minutes)...
endlocal
if %PROCESSOR_ARCHITECTURE% == AMD64 (
    curl -o %ext_path%\miniconda.exe --location https://repo.continuum.io/miniconda/Miniconda3-latest-Windows-x86_64.exe
) else (
    curl -o %ext_path%\miniconda.exe --location https://repo.continuum.io/miniconda/Miniconda3-latest-Windows-x86.exe
)
%ext_path%\MiniConda.exe /S /D=%UserProfile%\Miniconda3
set PATH=%PATH%;%UserProfile%\Miniconda3\Scripts
setx PATH "%UserProfile%\Miniconda3\Scripts;%PATH%" /M
echo.
setlocal EnableDelayedExpansion
goto CondaAvailable

:PrintUsage
echo SETUP-Windows.cmd [-help] [-skip_update] [-dev_cmd]
echo -help, -h, -?       Display usage message
echo -skip_update        Skip checking and installing requiments
echo -dev_cmd            Open VS Developer Command Prompt if available
goto:eof

endlocal
