# Building the Embedded Learning Library (ELL) on macOS

ELL enables you to design and deploy intelligent machine-learned models onto single-board computers, like Raspberry Pi and Arduino. Most of your interaction with ELL occurs on a laptop or desktop computer, rather than the single-board machine itself. The steps below describe how to build ELL on a laptop or desktop running macOS.

## Cloning the ELL repository

The instructions below assume that ELL was obtained from `github.com/Microsoft/ELL` using *git*. One way to install a git client is to open a Terminal and type

    brew install git

To clone the ELL repository, type

    git clone https://github.com/Microsoft/ELL.git

## Prerequisites for building ELL

We recommend using the [*Homebrew*](http://brew.sh/) package manager to download and install prerequisites. Homebrew can be downloaded and installed by

    /usr/bin/ruby -e "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/master/install)"

If you already have Homebrew installed, update it to the latest version by typing

    brew update

### LLVM and Clang

ELL depends on the [*LLVM*](http://llvm.org/) compiler framework, version 3.9 or newer. `Clang` is a C++ compiler built on top of LLVM, and we recommend using Clang to build ELL. To download and install LLVM and Clang, type

    brew install llvm@3.9 --with-clang

Alternatively, if you have already have *Xcode* installed and prefer to use the version of *Clang* included with it, you must still install LLVM and make sure that the command-line version of the *Xcode* tools are available.

    brew install llvm@3.9
    xcode-select --install

If you already have LLVM installed, ensure that you have version 3.9. Note that CMake assumes that the LLVM binary files are located in the standard Homebrew location, which is `/usr/local/opt`.

### CMake 3.8, SWIG 3.0.12, OpenBLAS, and Doxygen via Homebrew

ELL uses the [*CMake*](https://cmake.org/) build system, version 3.8 or newer.

Optionally, ELL can take advantage of these additional tools:
* [*SWIG*](http://swig.org) version 3.0.12 or newer - a tool that generates Python interfaces to C++ libraries. Required if you intend to use ELL from Python
* [*OpenBLAS*](http://www.openblas.net/) - fast linear algebra
* *Doxygen* - code documentation

To install all of the above, type

    brew install cmake
    brew install swig
    brew install homebrew/science/openblas
    brew install doxygen

## Using ELL in Python

ELL can optionally be used from Python 3.6. We recommend using the [Miniconda](https://conda.io/miniconda.html) distribution of Python, which makes it easy to install any required Python modules. Download and install Miniconda from here <https://conda.io/miniconda.html>.

After installing Miniconda, create a Python 3.6 environment by typing

    conda create -n py36 anaconda python=3.6

Next, activate the environment you just created by typing

    source activate py36

You need to repeat this activation command each time you open a new terminal and intend to use ELL from Python. Also, make sure to activate the `py36` environment before building ELL, to ensure that Python interfaces are created.

For computer vision tasks, we recommend capturing and preprocessing images using *OpenCV*. To install OpenCV in the current Python environment, type

    conda install -c conda-forge opencv

## Building ELL

We build ELL by using CMake to create a makefile, invoking that makefile, and optionally building Python interfaces. If you intend to build Python interfaces, make sure to activate the `py36` miniconda environment as described above.

In the repository root directory, create a `build` subdirectory and change to that directory.

    mkdir build
    cd build

Invoke CMake by typing

    cmake ..

Don't forget the two dots (..) at the end of the command! This creates a makefile for the project. Next, invoke the makefile by typing

    make

Optionally, build Python interfaces by typing

    make _ELL_python

The generated executables will appear in `ELL/build/bin`.

# Advanced Installation

The instructions above are enough to start using ELL. For more advanced topics, like testing and generating documentation, please see our [advanced installation instructions](INSTALL-Advanced.md).

