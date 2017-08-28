---
layout: default
title: Installing ELL on Linux
permalink: /Installing-and-building-ELL/INSTALL-Linux/
---

# Building the Embedded Learning Library (ELL) on Ubuntu Linux

### Cloning the ELL repository

The instructions below assume that ELL was obtained from *GitHub* using *git*. The *git* client is typically installed by default on Ubuntu systems, but if it isn't, open a terminal and type

    sudo apt-get install git

In a terminal, clone the ELL repository by typing

    git clone https://github.com/Microsoft/ELL.git

### Prerequisites
We recommend using the `apt-get` package manager to download and install prerequisites. First, make sure that apt-get is up to date, by typing

    sudo apt-get -y update

#### GCC and CMake
Next, you will need *gcc* and [*CMake*](https://cmake.org/). They are often installed by default on Ubuntu systems, but to confirm type

    sudo apt-get install -y gcc cmake

ELL requires gcc 5 and above for C++14 support.

#### LLVM
You will also need the dev version of [*LLVM-3.9*](http://llvm.org/). At the time of writing this document, `apt-get` doesn't yet have the required version of LLVM. To check this, type

    apt-cache show llvm-dev

and look for the version number. If the version number is 3.9 or greater, you can simply type `sudo apt-get install -y llvm-dev`. Otherwise, do the following:

    sudo apt-get install -y wget
    wget -O - http://apt.llvm.org/llvm-snapshot.gpg.key | sudo apt-key add -
    sudo apt-add-repository "deb http://apt.llvm.org/xenial/ llvm-toolchain-xenial-3.9 main"
    sudo apt-get -y update
    sudo apt-get install -y llvm-3.9-dev

#### libedit
Install the BSD editline and history libraries

    sudo apt-get install -y libedit-dev

#### zlib

    sudo apt-get install -y zlibc zlib1g zlib1g-dev

#### SWIG

The language bindings for Python are generated using SWIG. To build them, you need to install SWIG version 3.0.12 or later. 

```
wget http://prdownloads.sourceforge.net/swig/swig-3.0.12.tar.gz
tar zxvf swig-3.0.12.tar.gz && cd swig-3.0.12
./configure --without-pcre && make && sudo make install
```

### Optional Prerequisites (OpenBLAS and Doxygen)
ELL can optionally take advantage of the optimized linear algebra libraries in [*OpenBLAS*](http://www.openblas.net/), and generate code documentation using *Doxygen*. To install these optional dependencies, type

    sudo apt-get install -y libopenblas-dev doxygen

### Activate Conda Environment

The following commandline instructions assume you are using your activated Python 3.6 conda environment in a new terminal window.  
See [Python setup instructions](/ELL/Installing-and-building-ELL/).

    source activate py36

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

The create code documentation, type

    make doc

The *Doxygen* generated files will appear in `ELL/build/doc`.
