# Building a 64-bit version of the Embedded Learning Library (ELL) on Windows

### Cloning the ELL repository

The instructions below assume that ELL was obtained from `github.com/Microsoft/ELL` using *git*. For example, one way of doing this is to download and install the *git* command line tools from <https://git-scm.com/download> and then clone the ELL repository by opening a command window and typing 

    git clone https://github.com/Microsoft/ELL.git

### Prerequisites

##### Visual Studio 2015 update 3 with C++ compiler, or Visual Studio 2017
###### Visual Studio 2015 update 3
If using *Microsoft Visual Studio 2015 update 3*, make sure the C++ compiler is installed. Note that the C++ compiler installation is not enabled by default, so you must select the custom installation option and manually check the C++ checkbox.
###### Visual Studio 2017
A free version is available at <https://www.visualstudio.com/vs/community/>. Select the following Workloads during install:
* `Desktop Development with C++`

##### CMake
ELL uses the [*CMake*](https://cmake.org/) build system. You can download and install it from <https://cmake.org/download/>.

##### LLVM (as well as OpenBLAS and Doxygen)
ELL depends on the [*LLVM*](http://llvm.org/) compiler framework. Optionally, it can take advantage of the fast linear algebra libraries in [*OpenBLAS*](http://www.openblas.net/) and generate documentation using *Doxygen*. An easy way to get the prebuilt 64-bit version of LLVM and the optional dependencies is to use the [*NuGet*](https://www.nuget.org/) package manager (version 3.5 or newer). The relevant NuGet packages are specified in a config file in the `ELL/external` directory.  We recommend using the NuGet command line tool (NuGet CLI), which can be obtained by following the instructions at <https://docs.nuget.org/ndocs/guides/install-nuget>. After downloading NuGet CLI, change to the repository's root directory (`ELL`):

    cd ELL

and invoke the command

    nuget.exe restore external/packages.config -PackagesDirectory external

NuGet will download the prerequisites into the `ELL/external` directory.

By default, CMake will try to determine the correct version of the OpenBLAS library to use for your processor
type. If you want to override the automatic choice, you can tell CMake which version to use by setting the `PROCESSOR_HINT`
cache variable when you initially call CMake.

For Visual Studio 2015:

    cmake -G "Visual Studio 14 2015 Win64" -DPROCESSOR_HINT=haswell ..

For Visual Studio 2017:

    cmake -G "Visual Studio 15 2017 Win64" -DPROCESSOR_HINT=haswell ..

Or, if you have already run CMake to set up the build environment, you can just set the variable:

    cmake -DPROCESSOR_HINT=haswell ..

Currently, the only supported option for this variable is `haswell`.

### Building ELL
Next, use CMake to create a Visual Studio solution. In the repository's root directory (`ELL`), create a `build` subdirectory and change to that directory:

    mkdir build
    cd build

Next, invoke cmake as follows:

    cmake -G "Visual Studio 14 2015 Win64" -DPROCESSOR_HINT=haswell ..

**Important:** don't forget the two dots (..) at the end of the command! This command creates a solution file named `ELL.sln`, along with other files in the `build` directory. 

There are two alternative ways to build the library. One option is to build everything from the command line, by typing 

    cmake --build .

**Important** don't forget the dot (.) at the end of the command!

The other option is to open the solution file in Visual Studio and build it there. To do this, type:

    ELL.sln

Visual studio will open the solution. Choose either the "Debug" configuration or the "Release" configuration, and use the shortcut `Ctrl+Shift+B` to build the solution. 

In both cases, the project executables will appear in a directory named `ELL/build/bin`.

### Testing ELL
    
    cd build
    ctest --build-config Debug

The ctest tool comes with your cmake installation.  For more info see [ctest](https://cmake.org/cmake/help/v3.9/manual/ctest.1.html).

### Generating code documentation

The create code documentation, open the `ELL.sln` solution file in Visual Studio, find the project named `doc` in the solution explorer, and build it. The *Doxygen* generated files will appear in `ELL/build/doc`.
