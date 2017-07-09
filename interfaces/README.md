# ELL Language Bindings

## Overview

This section covers how to generate and test the ELL language bindings, which provide language-specific wrappers for ELL's library functions.

## General setup

The language bindings are generated using SWIG. To build them, you need to install SWIG version 3.0.12 or later. 

* Linux / Mac:
```
wget http://www.swig.org/download.htmlhttp://prdownloads.sourceforge.net/swig/swig-3.0.12.tar.gz
tar zxvf swig-3.0.12.tar.gz && cd swig-3.0.12.tar
./configure --without-pcre && make && sudo make install
```
* Windows:
On windows we have already included a [Nuget package](https://www.nuget.org/packages/swigwintools/3.0.12) for swig in the initial setup, so you should be good to go!

## Python

1. Install Python 3.6, preferably through [Miniconda](https://conda.io/miniconda.html), which works well with Jupyter notebooks and provides a way to manage different python versions. ELL SWIG bindings require Python 3.6.
* Linux: Miniconda3-Linux-x86_64
* Mac: Miniconda3-latest-MacOSX-x86_64
* Windows: Miniconda3-latest-Windows-x86_64

Note: you can also use the full [Anaconda](https://www.continuum.io/downloads) if you already have that installed.

2. Configure the Python 3.6 environment using Miniconda
```
# Create the environment
conda create -n py36 anaconda python=3
# Activate the environment Linux / Mac:
source activate py36
# Activate the environment Windows:
activate py36
# ELL requires gcc 5 and above for C++14. Upgrade anaconda's environment to support it.
conda install libgcc
```
3. Generate the python bindings. In the repository root directory from within the py36 anaconda environment:
```
mkdir build
cd build
cmake ..
# Linux / Mac
make _ELL_python
# Windows
cmake --build . --target _ELL_python --config Release
```

Note: if you already built ELL before you installed Python, you will need to delete that build and repeat the above steps
in order to ensure the build is correctly setup to do the additional Python related build steps.

4. Test the python bindings, which are located in build/interfaces/python/test:
```
cd interfaces/python/test
python test.py
```
