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

    sudo apt-get install zlibc zlib1g zlib1g-dev

#### SWIG

The language bindings for Python are generated using SWIG. To build them, you need to install SWIG version 3.0.12 or later. 

```
wget http://prdownloads.sourceforge.net/swig/swig-3.0.12.tar.gz
tar zxvf swig-3.0.12.tar.gz && cd swig-3.0.12
./configure --without-pcre && make && sudo make install
```

#### Python

The tutorials require Python 3.6, we recommend you use [Miniconda](https://conda.io/miniconda.html), which works well with Jupyter notebooks and provides a way to manage different Python versions.   Note: you can also use the full [Anaconda](https://www.continuum.io/downloads) if you already have that installed.

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

### Optional Prerequisites (OpenBLAS and Doxygen)
ELL can optionally take advantage of the optimized linear algebra libraries in [*OpenBLAS*](http://www.openblas.net/), and generate code documentation using *Doxygen*. To install these optional dependencies, type

    sudo apt-get install -y libopenblas-dev doxygen

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
