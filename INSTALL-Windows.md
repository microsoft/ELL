# Building a 64-bit version of the Embedded Learning Library (ELL) on Windows

ELL enables you to design and deploy intelligent machine-learned models onto single-board computers, like Raspberry Pi and Arduino. Most of your interaction with ELL occurs on a laptop or desktop computer, rather than the single-board machine itself. The steps below describe how to build ELL on a laptop or desktop running Windows.

## Cloning the ELL repository

The instructions below assume that ELL was obtained from `github.com/Microsoft/ELL` using *git*. For example, one way of doing this is to download and install the git command line tools from <https://git-scm.com/download> and then clone the ELL repository by opening a command prompt and typing

    git clone https://github.com/Microsoft/ELL.git

## Prerequisites for building ELL

### Visual Studio

ELL requires a C++ compiler. On Windows, we support *Visual Studio 2015 update 3 with C++ compiler* and *Visual Studio 2017 with C++ Desktop Development Workload*. A free version of Visual Studio 2017 is available at <https://www.visualstudio.com/vs/community/> (make sure to select *Desktop Development with C++* during installation).

### CMake 3.8

ELL uses the [*CMake*](https://cmake.org/) build system, version 3.8 or newer. Download and install it from <https://cmake.org/download/>.

### LLVM 3.9, SWIG 3.0.12, OpenBlas, and Doxygen via NuGet

ELL depends on the [*LLVM*](http://llvm.org/) compiler framework, version 3.9 or newer.

SWIG is a tool that generates Python interfaces to C++ libraries. If you intend to use ELL from Python, you must install [*SWIG*](http://swig.org) version 3.0.12 or newer.

Optionally, ELL can take advantage of these additional tools:
* [*OpenBLAS*](http://www.openblas.net/) - fast linear algebra
* *Doxygen* - code documentation

We recommend installing all of the above. An easy way to get prebuilt 64-bit versions of these packages is to use the [*NuGet*](https://www.nuget.org/) package manager, version 3.5 or newer. The relevant NuGet packages are specified in `ELL/external/packages.config`. We recommend using the NuGet command line tool called NuGet CLI, which can be downloaded from <https://docs.nuget.org/ndocs/guides/install-nuget>. After downloading and installing NuGet CLI, open a command prompt, change to the repository's root directory (`ELL`) and type

    nuget.exe restore external/packages.config -PackagesDirectory external

NuGet will download the prerequisites into the `ELL/external` directory.

## Using ELL in Python

ELL can optionally be used from Python 3.6. We recommend using the [Miniconda](https://conda.io/miniconda.html) distribution of Python, which makes it easy to install any required Python modules. Download and install Miniconda from here <https://conda.io/miniconda.html>.

After installing Miniconda, create a Python 3.6 environment by typing

    conda create -n py36 anaconda python=3.6

Next, activate the environment you just created by

    activate py36

You need to repeat this activation command each time you open a new terminal and intend to use ELL from Python. Also, make sure to activate the `py36` environment before building ELL, to ensure that Python interfaces are created.

For computer vision tasks, we recommend capturing and preprocessing images using *OpenCV*. To install OpenCV in the current Python environment, type

    conda install -c conda-forge opencv

## Building ELL

We build ELL by using CMake to create a Visual Studio solution, building that solution, and optionally building Python interfaces. If you intend to build Python interfaces, make sure to activate the `py36` environment as described above.

In the repository root directory, create a `build` subdirectory and change to that directory.

    mkdir build
    cd build

If your installed compiler is Visual Studio 2015, invoke CMake as follows

    cmake -G "Visual Studio 14 2015 Win64" ..

Don't forget the two dots (..) at the end of the command! This command creates a solution file named `ELL.sln`, and other files, in the `build` directory.
If your compiler is Visual Studio 2017, invoke CMake as follows

    cmake -G "Visual Studio 15 2017 Win64" ..

Again, don't forget the two dots (..) at the end of the command. After creating the Visual Studio solution, build ELL by typing

    cmake --build . --config Release

The project executables will appear in `ELL/build/bin`. Finally, to build ELL's Python language bindings, type

    cmake --build . --target _ELL_python --config Release

# Advanced Installation

The instructions above are enough to start using ELL. For more advanced topics, like testing and generating documentation, please see our [advanced installation instructions](INSTALL-Advanced.md).
