# Building a 64-bit version of the Embedded Learning Library (ELL) on Windows

ELL enables you to design and deploy intelligent machine-learned models onto single-board computers, like Raspberry Pi and Arduino. Most of your interaction with ELL occurs on a laptop or desktop computer, rather than the single-board machine itself. The steps below describe how to build ELL on a laptop or desktop running Windows. You can also use `SETUP-Windows.cmd` to setup your computer.

## Cloning the ELL repository

The instructions below assume that ELL was obtained from `github.com/Microsoft/ELL` using *git*. For example, one way of doing this is to download and install the git command line tools from <https://git-scm.com/download> and then clone the ELL repository by opening a command prompt and typing
```shell
git clone https://github.com/Microsoft/ELL.git
```
## Prerequisites for building ELL

### Visual Studio

ELL requires a C++ compiler. On Windows, you can use *Visual Studio 2019 with C++ Desktop Development Workload*. A free version of Visual Studio 2019 is available at <https://www.visualstudio.com/vs/community/> (make sure to select *Desktop Development with C++* during installation).

### CMake 3.12

ELL uses the [*CMake*](https://cmake.org/) build system and requires version 3.15 or newer. A version of CMake that satisfies this requirement is already provided with Visual Studio 2019. You can find it if you
use the "Developer Command Prompt' that is installed by Visual Studio.  It lives here:
```
C:\Program Files (x86)\Microsoft Visual Studio\2019\Preview\Common7\IDE\CommonExtensions\Microsoft\CMake\CMake\bin\cmake.exe
```
You may also download new versions of cmake from <https://cmake.org/download/> if you want to.

### LLVM 8.0, SWIG 4.0.0, OpenBLAS 0.2.19.3, and Doxygen 1.8.13 via NuGet

The easiest way to get prebuilt 64-bit versions of these packages is to use the [*NuGet*](https://www.nuget.org/) package manager, version 3.5 or newer. The relevant NuGet packages are specified in `ELL/external/packages.config`. The NuGet command line tool called `NuGet CLI` can be downloaded from <https://docs.nuget.org/ndocs/guides/install-nuget>. After downloading and installing NuGet CLI, open a command prompt, change to the repository's root directory (`ELL`) and type

```shell
nuget.exe restore external/packages.config -PackagesDirectory external
```

NuGet will download the prerequisites into the `ELL/external` directory and you are done.

Here's more information in case you need to install things manually.

* [*LLVM*](http://llvm.org/) is a C++ compiler framework, ELL depends on version 8.0.
* [*SWIG*](http://swig.org) version 4.0.0 - a tool that generates Python interfaces to C++ libraries. Required if you intend to use ELL from Python.
* [*OpenBLAS*](http://www.openblas.net/) version 0.2.19.3 - fast linear algebra. This is optional but can make models execute up to 10 times faster.
* [*Doxygen*](www.doxygen.org/) version 1.8.13 - this is optional, it is used to generate nice code documentation for the ELL API.

## Using ELL in Python

ELL can optionally be used from Python 3.6.
An easy way to install Python and all the required modules is with [Miniconda](https://conda.io/miniconda.html).
Download and install Miniconda from here <https://conda.io/miniconda.html>.

After installing Miniconda, create a Python 3.6 environment by typing

```shell
conda create -n py36 python=3.6
```

Next, activate the environment you just created by

```shell
activate py36
```

You need to repeat this activation command each time you open a new terminal and intend to use ELL from Python. Also, make sure to activate the `py36` environment before building ELL, to ensure that Python interfaces are created.

*OpenCV* is a library that helps with capturing and preprocessing images. To install OpenCV in the current Python environment, type

```shell
conda install -c conda-forge opencv
```

In order to run the Python based ELL unit tests you will also need to install the following:

```shell
conda install pytorch torchvision cudatoolkit=10.1 -c pytorch
conda install -c conda-forge onnx
pip install -r requirements.txt
```

Miniconda comes with useful command line tools such as `curl`, which is used to transfer data via URL. When files are required to be downloaded from a URL, the instructions assume you have `curl` available to perform the download. Ensure you activate your conda environment before running commands like `curl`.

## Building ELL

We build ELL by using CMake to create a Visual Studio solution, building that solution, and optionally building Python interfaces. If you intend to build Python interfaces, make sure to activate the `py36` environment as described above.

In the repository root directory, create a `build` subdirectory and change to that directory.

```shell
mkdir build
cd build
```

Invoke CMake as follows

```shell
cmake -G "Visual Studio 16 2019" -A x64 -T host=x64 -D ONNX=ON ..
```
Don't forget the two dots (..) at the end of the command! This command creates a Visual Studio solution file named `ELL.sln`, and the associated project files in the `build` directory.

After creating the Visual Studio solution, build ELL by typing:

```shell
cmake --build . --config Release
```

The project executables will appear in `ELL/build/bin`. Finally, to build ELL's Python language bindings, type

```shell
cmake --build . --target _ELL_python --config Release
```

You can test that the python interface is working by running the following test:

```shell
ctest . --build-config release -R ell-python-interface-test
```

## Path Environment

When you run cmake in the ELL build folder it will output something like this:

````
-- Using OpenBLAS compiled for haswell
-- Using BLAS include path: D:/git/ELL/ELL/external/OpenBLASLibs.0.2.19.3/build/native/x64/haswell/include
-- Using BLAS library: D:/git/ELL/ELL/external/OpenBLASLibs.0.2.19.3/build/native/x64/haswell/lib/libopenblas.dll.a
````

This tells you which version of OpenBLAS ELL will be using.  So if you want to be able to run an ELL program
from anywhere on your machine (outside of your ELL git repo) you will need to help your app find this DLL.
This can be done by modifying your PATH environment variable like this:

```shell
set PATH=%PATH%;D:/git/ELL/ELL/external/OpenBLASLibs.0.2.19.3/build/native/x64/haswell/bin
```

Note: the end of this path was changed to add `/bin` instead of /include or /lib.  The bin folder is where the actual DLL's live and it is the DLL's that your app will need to find.  You can set this PATH locally for a given terminal window or you can add it in your system PATH if you want to.

# Advanced Installation

The instructions above are enough to start using ELL. For more advanced topics, like testing and generating documentation, please see our [advanced installation instructions](INSTALL-Advanced.md).
