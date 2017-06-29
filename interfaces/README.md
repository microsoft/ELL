# ELL Language Bindings

## Overview

This section covers how to generate and test the ELL language bindings, which provide language-specific wrappers for ELL's library functions.

## General setup

The language bindings are generated using SWIG. To build them, you need to install SWIG version 3.0.12 or later. SWIG can be downloaded from [here](http://www.swig.org/download.html)
* Linux / Mac:
```
tar zxvf swig-VERSION.tar.gz
cd swig-VERSION/
./configure
make
sudo make install
# May be required on some Linux systems (replace VERSION with the actual version on your system)
sudo ln /lib/x86_64-linux-gnu/libpcre.so.VERSION /usr/lib/libpcre.so.1
```
* Windows:
  * Extract swigwin-VERSION.zip
  * Add the directory containing swig.exe to your PATH environment variable. 

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
# Activate the environment
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
