# Building the Embedded Learning Library (ELL) on Ubuntu Linux

ELL enables you to design and deploy intelligent machine-learned models onto single-board computers, like Raspberry Pi and Arduino. Most of your interaction with ELL occurs on a laptop or desktop computer, rather than the single-board machine itself. The steps below describe how to build ELL on a laptop or desktop running Ubuntu Linux.

## Cloning the ELL repository

The instructions below assume that ELL was obtained from [https://github.com/Microsoft/ELL](https://github.com/Microsoft/ELL) using *git*. The [git scm client](https://git-scm.com/) is typically installed by default on Ubuntu systems, but if it isn't, open a terminal and type

```shell
sudo apt-get install -y git
```

To clone the ELL repository, type

```shell
git clone https://github.com/Microsoft/ELL.git
```

## Prerequisites for building ELL

Ubunto provides the Advanced Packaging Tool `apt` for downloading and installing prerequisites. First, make sure that `apt` is up to date by running:

```shell
sudo apt-get -y update
```

### GCC 5, CMake 3.8, libedit, zlib, OpenBLAS, and Doxygen via apt-get

ELL requires the following tools and libraries, some of which are installed by default on Ubuntu systems:

* *GCC 5* or newer - C++14 compiler
* [*CMake*](https://cmake.org/) version 3.8 or newer - build system
* *libedit* and *zlib* libraries

Optionally, ELL can take advantage of these additional tools:

* [*OpenBLAS*](http://www.openblas.net/) - version 0.2.19.3 - fast linear algebra. This is optional but can make models execute up to 10 times faster.
* [*Doxygen*](www.doxygen.org/) - version 1.8.13 - this is optional, it is used to generate nice code documentation for the ELL API.

To install all of the above, type
```shell
sudo apt-get install -y gcc cmake libedit-dev zlibc zlib1g zlib1g-dev
sudo apt-get install -y libopenblas-dev doxygen
```
### LLVM 3.9

ELL depends on the [*LLVM*](http://llvm.org/) compiler framework, version 3.9-dev. To install it use this command

```shell
sudo apt-get install -y llvm-3.9-dev
```

This works on Ubuntu 16.04 Xenial.  If you have some other Linux version and llvm-3.9-dev is not available then
[*LLVM*](http://llvm.org/) has more instructions on how to install it manually.

### curl

`curl` is a command line tool used to transfer data via URL. When files are required to be downloaded from a URL, the instructions assume you have `curl` available to perform the download. To install `curl`, type the following:
```shell
sudo apt-get install -y curl
```

### SWIG 3.0.12

[*SWIG*](http://swig.org) is a tool that generates Python interfaces to C++ libraries. If you intend to use ELL from Python, you must install SWIG version 3.0.12. At the time of writing this document, `apt-get` doesn't yet have the latest version of `SWIG`, so it must be installed manually

```shell
curl -O --location http://prdownloads.sourceforge.net/swig/swig-3.0.12.tar.gz
tar zxvf swig-3.0.12.tar.gz && cd swig-3.0.12
./configure --without-pcre && make && sudo make install
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
conda install -c conda-forge opencv -y
```

## Building ELL

You can build ELL by using CMake to create a makefile, invoking that makefile, and optionally building Python interfaces. If you intend to build Python interfaces, make sure to activate the `py36` miniconda environment as described above.

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

# Advanced Installation

The instructions above are enough to start using ELL. For more advanced topics, like testing and generating documentation, please see our [advanced installation instructions](INSTALL-Advanced.md).
