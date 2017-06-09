# ELL Language Bindings

## Overview

This section covers how to generate and test the ELL language bindings, which provide language-specific wrappers for ELL's library functions.

## General setup

The language bindings are generated using SWIG. To build them, you need to install SWIG version 3.0.10 or later. SWIG can be downloaded from [here](http://www.swig.org/download.html)
* Linux:
```
tar zxvf swig-VERSION.tar.gz
cd swig-VERSION/
./configure
make
sudo make install
# May be required on some systems (replace VERSION with the actual version on your system)
sudo ln /lib/x86_64-linux-gnu/libpcre.so.VERSION /usr/lib/libpcre.so.1
```
* Windows:
  * Extract swigwin-VERSION.zip
  * Add the directory containing swig.exe to your PATH environment variable. 

## Python

1. Install Python 3, preferably through Anaconda, which works well with Jupyter notebooks and provides a way to manage different python versions. We currently support Python >= 3.6.
* Linux: Anaconda3-4.3.1-Linux-x86_64
* Windows: Anaconda3-4.3.1-Windows-x86_64

2. Configure the Python 3.6 environment using Anaconda
```
# Create the environment
conda create -n py36 python=3.6 anaconda
# Activate the environment
activate py36
# ELL requires gcc 5 and above for C++14. Upgrade anaconda's environment to support it.
conda install libgcc 
```
3. Generate the python bindings. In the repository root directory from within the py36 anaconda environment:
```
mkdir build
cd build
cmake -DBUILD_INTERFACES=ON ..
# Linux
make
# Windows
cmake --build .
```
4. Test the python bindings, which are located in build/interfaces/python/test:
```
cd interfaces/python/test
python test.py
```
