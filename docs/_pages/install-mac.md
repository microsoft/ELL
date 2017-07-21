---
layout: default
title: Install Mac
permalink: /install-mac/
---
# Building the Embedded Learning Library (ELL) on Mac OS X

### Homebrew

We recommend using the [*Homebrew*](http://brew.sh/) package manager to download and install prerequisites. *Homebrew* can be downloaded and installed by

    /usr/bin/ruby -e "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/master/install)"

Note that if you already have Homebrew installed, you must update it to the latest version, by typing

    brew update

### Cloning the ELL repository

The instructions below assume that ELL was obtained from *GitHub* using *git*. One way to install a git client is to open Terminal and type

    brew install git

To clone the ELL repository using *git*, type

    git clone https://github.com/Microsoft/ELL.git

### Prerequisites
ELL requires [*CMake*](https://cmake.org/), [*LLVM*](http://llvm.org/) version 3.9, and *Clang*. To download and install these prerequisites, type

    brew install cmake
    brew install llvm@3.9 --with-clang

If you have already have *Xcode* installed and would rather use the version of *Clang* included with it, you can omit downloading *Clang* separately,
but you must make sure the command-line version of the *Xcode* tools are available.

    brew install cmake
    brew install llvm@3.9
    xcode-select --install

If you already have *LLVM* installed, ensure that you have version 3.9.

**CMake assumes that the binary files for LLVM are located in /usr/local/opt (The standard brew location)**

### SWIG

The language bindings for Python are generated using SWIG. To build them, you need to install SWIG version 3.0.12 or later. 

```
brew install swig
```

### Python

The tutorials require P`ython 3.6, we recommend you use [Miniconda](https://conda.io/miniconda.html), which works well with Jupyter notebooks and provides a way to manage different Python versions.   Note: you can also use the full [Anaconda](https://www.continuum.io/downloads) if you already have that installed.

If you build ELL from a `conda` Python 3.6 environment then the Python language bindings will be built and you can run the
tutorials.

To configure the Python 3.6 environment using Miniconda:
```
# Create the environment
conda create -n py36 anaconda python=3
# Activate the environment
source activate py36
```
Now you have an activated `conda` Python 3.6 environment, and if you build ELL from this environment then the Python language bindings will be built and you can run the tutorials.

### Optional Dependencies (OpenBLAS and Doxygen)

By default, ELL will use the *BLAS* linear algebra libraries built into the operating system, but it
can optionally take advantage of the optimized linear algebra libraries in [*OpenBLAS*](http://www.openblas.net/).
ELL can also generate code documentation using *Doxygen*. To install these optional dependencies, type

    brew install homebrew/science/openblas
    brew install doxygen

### Building ELL
Next, use CMake to create a makefile. In the repository root directory, create a `build` subdirectory and change to that directory:

    mkdir build
    cd build

Then, invoke cmake:

    cmake ..

**Important:** don't forget the two dots (..) at the end of the command!

Now you can build ELL by typing:

    make

and lastly, so you can run the tutorials, the following will build the Python language bindings for ELL:

    make _ELL_python 

The generated executables will appear in a directory named `ELL/build/bin`.

### Generating code documentation

To create code documentation, type

    make doc

The *Doxygen* generated files will appear in `ELL/build/doc`.
