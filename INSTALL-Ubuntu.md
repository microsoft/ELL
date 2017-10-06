# Building the Embedded Learning Library (ELL) on Ubuntu Linux

ELL enables you to design and deploy intelligent machine-learned models onto single-board computers, like Raspberry Pi and Arduino. Most of your interaction with ELL occurs on a laptop or desktop computer, rather than the single-board machine itself. The steps below describe how to build ELL on a laptop or desktop running Ubuntu Linux.

## Cloning the ELL repository

The instructions below assume that ELL was obtained from `github.com/Microsoft/ELL` using *git*. The git client is typically installed by default on Ubuntu systems, but if it isn't, open a terminal and type

    sudo apt-get install git

To clone the ELL repository, type

    git clone https://github.com/Microsoft/ELL.git

## Prerequisites for building ELL

We recommend using the `apt-get` package manager to download and install prerequisites. First, make sure that apt-get is up to date by

    sudo apt-get -y update

### gcc 5, CMake 3.8, libedit, zlib, OpenBLAS, and Doxygen via apt-get

ELL requires the following tools and libraries, some of which are installed by default on Ubuntu systems:
* *gcc 5* or newer - C++14 compiler
* [*CMake*](https://cmake.org/) version 3.8 or newer - build system
* *libedit* and *zlib* libraries

Optionally, ELL can take advantage of these additional tools:
* [*OpenBLAS*](http://www.openblas.net/) - fast linear algebra
* *Doxygen* - code documentation

To install all of the above, type

    sudo apt-get install -y gcc cmake libedit-dev zlibc zlib1g zlib1g-dev
    sudo apt-get install -y libopenblas-dev doxygen

### LLVM 3.9

ELL depends on the [*LLVM*](http://llvm.org/) compiler framework, version 3.9-dev or newer. At the time of writing this document, `apt-get` doesn't yet have the latest versions of LLVM. To confirm this, type

    apt-cache show llvm-dev

and look for the version number. If the version number is 3.9 or greater, you can simply type `sudo apt-get install -y llvm-dev`. Otherwise, do the following to install LLVM 3.9-dev:

    sudo apt-get install -y wget
    wget -O - http://apt.llvm.org/llvm-snapshot.gpg.key | sudo apt-key add -
    sudo apt-add-repository "deb http://apt.llvm.org/xenial/ llvm-toolchain-xenial-3.9 main"
    sudo apt-get -y update
    sudo apt-get install -y llvm-3.9-dev

### SWIG 3.0.12

[*SWIG*](http://swig.org) is a tool that generates Python interfaces to C++ libraries. If you intend to use ELL from Python, you must install SWIG version 3.0.12 or newer. At the time of writing this document, `apt-get` doesn't yet have the latest version of `SWIG`, so it must be installed manually

   wget http://prdownloads.sourceforge.net/swig/swig-3.0.12.tar.gz
   tar zxvf swig-3.0.12.tar.gz && cd swig-3.0.12
   ./configure --without-pcre && make && sudo make install

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
