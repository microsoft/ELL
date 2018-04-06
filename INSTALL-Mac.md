# Building the Embedded Learning Library (ELL) on macOS

ELL enables you to design and deploy intelligent machine-learned models onto single-board computers, like Raspberry Pi and Arduino. Most of your interaction with ELL occurs on a laptop or desktop computer, rather than the single-board machine itself. The steps below describe how to build ELL on a laptop or desktop running macOS.

## Cloning the ELL repository

The instructions below assume that ELL was obtained from `github.com/Microsoft/ELL` using *git*. One way to install a git client is to open a Terminal and type

```shell
brew install git
```

To clone the ELL repository, type

```shell
git clone https://github.com/Microsoft/ELL.git
```

## Prerequisites for building ELL

[*Homebrew*](http://brew.sh/) is a package manager that makes it easy to install the prerequesits. Homebrew can be downloaded and installed by

```shell
/usr/bin/ruby -e "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/master/install)"
```

If you already have Homebrew installed, update it to the latest version by typing

```shell
brew update
```

### LLVM and Clang

ELL depends on the [*LLVM*](http://llvm.org/) compiler framework, version 3.9.  `Clang` is a C++ compiler built on top of LLVM, and it is the best choice for building ELL. To download and install LLVM and Clang, type

```shell
brew install llvm@3.9
```

Alternatively, if you have already have *Xcode* installed and prefer to use the version of *Clang* included with it, you must still install LLVM and make sure that the command-line version of the *Xcode* tools are available.

```shell
brew install llvm@3.9
xcode-select --install
```

If you already have LLVM installed, ensure that you have version 3.9. Note that CMake assumes that the LLVM binary files are located in the standard Homebrew location, which is `/usr/local/opt`.

### CMake 3.8, SWIG 3.0.12, OpenBLAS, and Doxygen via Homebrew

ELL uses the [*CMake*](https://cmake.org/) build system, version 3.8 or newer.

Optionally, ELL can take advantage of these additional tools:

* [*SWIG*](http://swig.org) version 3.0.12 - a tool that generates Python interfaces to C++ libraries. Required if you intend to use ELL from Python.
* [*OpenBLAS*](http://www.openblas.net/) - version 0.2.19.3 - fast linear algebra. OpenBLAS can make models execute up to 10 times faster.
* [*Doxygen*](www.doxygen.org/) -  version 1.8.13 - it is used to generate nice code documentation for the ELL API.

To install all of the above, type

```shell
brew install cmake
brew install swig
brew install openblas
brew install doxygen
```

## Using ELL in Python

ELL can optionally be used from Python 3.6. 
An easy way to install Python and all the required modules is with [Miniconda](https://conda.io/miniconda.html).
Download and install Miniconda from here <https://conda.io/miniconda.html>.

After installing Miniconda, create a Python 3.6 environment and include the `numpy` module by typing

```shell
conda create -n py36 numpy python=3.6
```

Next, activate the environment you just created by typing

```shell
source activate py36
```

You need to repeat this activation command each time you open a new terminal and intend to use ELL from Python. Also, make sure to activate the `py36` environment before building ELL, to ensure that Python interfaces are created.

*OpenCV* is a library that helps with capturing and preprocessing images. To install OpenCV in the current Python environment, type

```shell
conda install -c conda-forge opencv
```

## Building ELL

We build ELL by using CMake to create a makefile, invoking that makefile, and optionally building Python interfaces. If you intend to build Python interfaces, make sure to activate the `py36` miniconda environment as described above.

In the repository root directory, create a `build` subdirectory and change to that directory.

```shell
mkdir build
cd build
```

Invoke CMake by typing

```shell
cmake ..
```

Don't forget the two dots (..) at the end of the command! This creates a makefile for the project. Next, invoke the makefile by typing

```shell
make
```

Optionally, build Python interfaces by typing

```shell
make _ELL_python
```

The generated executables will appear in `ELL/build/bin`.

## Troubleshooting

**LLVM not found, please check that LLVM is installed.**

Try telling CMake where to find LLVM as follows:

```shell
cmake -DLLVM_DIR=/usr/local/Cellar/llvm@3.9/3.9.1_1/lib/cmake/llvm ..
```

## Advanced Installation

The instructions above are enough to start using ELL. For more advanced topics, like testing and generating documentation, please see our [advanced installation instructions](INSTALL-Advanced.md).

