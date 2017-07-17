# Building a 64-bit version of the Embedded Learning Library (ELL) on Windows

### Cloning the ELL repository

The instructions below assume that ELL was obtained from `github.com/Microsoft/ELL` using *git*. For example, one way of doing this is to download and install the *git* command line tools from <https://git-scm.com/download> and then clone the ELL repository by opening a command window and typing 

    git clone https://github.com/Microsoft/ELL.git

### Prerequisites

#### Visual Studio 2015 update 3 with C++ compiler, or Visual Studio 2017
##### Visual Studio 2015 update 3
If using *Microsoft Visual Studio 2015 update 3*, make sure the C++ compiler is installed. Note that the C++ compiler installation is not enabled by default, so you must select the custom installation option and manually check the C++ checkbox.
##### Visual Studio 2017
A free version is available at <https://www.visualstudio.com/vs/community/>. Select the following Workloads during install:
* `Desktop Development with C++`

#### CMake
ELL uses the [*CMake*](https://cmake.org/) build system. You can download and install it from <https://cmake.org/download/>.

#### LLVM (as well as OpenBLAS and Doxygen)
ELL depends on the [*LLVM*](http://llvm.org/) compiler framework. Optionally, it can take advantage of the fast linear algebra libraries in [*OpenBLAS*](http://www.openblas.net/) and generate documentation using *Doxygen*. An easy way to get the prebuilt 64-bit version of LLVM and the optional dependencies is to use the [*NuGet*](https://www.nuget.org/) package manager (version 3.5 or newer). The relevant NuGet packages are specified in a config file in the `ELL/external` directory.  We recommend using the NuGet command line tool (NuGet CLI), which can be obtained by following the instructions at <https://docs.nuget.org/ndocs/guides/install-nuget>. After downloading NuGet CLI, change to the repository's root directory (`ELL`):

#### Python

The tutorials require Python 3.6, we recommend you use [Miniconda](https://conda.io/miniconda.html), which works well with Jupyter notebooks and provides a way to manage different Python versions.   Note: you can also use the full [Anaconda](https://www.continuum.io/downloads) if you already have that installed.

To configure the Python 3.6 environment using Miniconda:
```
# Create the environment
conda create -n py36 anaconda python=3
# Activate the environment
activate py36
```
Now you have an activated `conda` Python 3.6 environment, and if you build ELL from this environment then the Python language bindings will be built and you can run the tutorials.

#### Nuget Packages

    cd ELL

and invoke the command

    nuget.exe restore external/packages.config -PackagesDirectory external

NuGet will download the prerequisites into the `ELL/external` directory.

### Building Visual Studio Solution using CMake

Next, use CMake to create a Visual Studio solution. In the repository's root directory (`ELL`), create a `build` subdirectory and change to that directory:

    mkdir build
    cd build

Next, invoke cmake as follows:
    
For Visual Studio 2015:

    cmake -G "Visual Studio 14 2015 Win64" -DPROCESSOR_HINT=haswell ..

For Visual Studio 2017:

    cmake -G "Visual Studio 15 2017 Win64" -DPROCESSOR_HINT=haswell ..


**Important:** don't forget the two dots (..) at the end of the command! This command creates a solution file named `ELL.sln`, along with other files in the `build` directory. 

### Adding OpenBLAS to your PATH environment

By default, CMake will try to determine the correct version of the OpenBLAS library to use for your processor
type. If you want to override the automatic choice, you can tell CMake which version to use by setting the `PROCESSOR_HINT`
variable.

The tutorials will need to be able to find OpenBLAS just installed by nuget.
The above CMake operation has already located the right version to use and you should see some CMake output like this:
````
-- Processor family: 6, model: 79
-- Using OpenBLAS compiled for haswell
-- Using BLAS include path: 
D:/git/ELL/external/OpenBLASLibs.0.2.19.3/build/native/x64/haswell/include
-- Using BLAS library: D:/git/ELL/external/OpenBLASLibs.0.2.19.3/build/native/x64/haswell/lib/libopenblas.dll.a
````
In this example the output is telling you to use the `haswell` version of OpenBLAS, so you should add the following to your system PATH environment variable:
````
set PATH=%PATH%;D:\git\ELL\external\OpenBLASLibs.0.2.19.3\build\native\x64\haswell\bin
````
Of course, don't just copy the above directly, you should use the correct paths to your git repo.

### Building ELL


Now you can build ELL by typing:

    cmake --build . --config Release

And lastly, so you can run the tutorials, the following will build the Python language bindings for ELL:

    cmake --build . --target _ELL_python --config Release 

*Note:* you can also open the `ELL.sln` Visual Studio Solution and build it from there, which is how we normally do our development work on ELL.
Choose either the "Debug" configuration or the "Release" configuration, and use the shortcut `Ctrl+Shift+B` to build the solution. 

In both cases, the project executables will appear in a directory named `ELL/build/bin`.

### Testing ELL
    
    cd build
    ctest --build-config Release

The ctest tool comes with your cmake installation.  For more info see [ctest](https://cmake.org/cmake/help/v3.9/manual/ctest.1.html).

If tests fail you can add the `-VV` option to get verbose output from each test which can help narrow down the problem.

### Generating code documentation

The create code documentation, open the `ELL.sln` solution file in Visual Studio, find the project named `doc` in the solution explorer, and build it. The *Doxygen* generated files will appear in `ELL/build/doc`.
