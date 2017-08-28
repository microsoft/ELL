---
layout: default
title: Installing ELL
permalink: /Installing-and-building-ELL/
---

# Installing ELL

### Cloning the ELL repository

The instructions below assume that ELL was obtained from *GitHub* using [git](https://git-scm.com/download). 

In a terminal, clone the ELL repository by typing:

    git clone https://github.com/Microsoft/ELL.git

You are, of course, free to place the code anywhere you want on your PC, but the
tutorials will refer to the location you chose with the path "~/git/ELL".  If you installed it elsewhere
then in our tutorial instructions replace this shortcut with your real location.

### Python 3.6

The ELL tutorials and some of the ELL tools depend on Python 3.6.

We recommend you use [Miniconda](https://conda.io/miniconda.html), which works well with Jupyter notebooks and provides a way to manage different Python versions.   Note: you can also use the full [Anaconda](https://www.continuum.io/downloads) if you already have that installed.

If you build ELL from a `conda` Python 3.6 environment then the Python language bindings will be built and you can run the tutorials.

To configure the Python 3.6 environment using Miniconda:
```
# Create the environment
conda create -n py36 anaconda python=3
# Activate the environment
source activate py36
```
Now you have an activated `conda` Python 3.6 environment, and if you build ELL from this environment then the Python language bindings will be built and you can run the tutorials.

The tutorials also use OpenCV.  See [Installing OpenCV](/ELL/Installing-OpenCV/).

### Mac, Linux, Windows

The rest of the instructions are platform specific.

- [INSTALL-Mac](/ELL/Installing-and-building-ELL/INSTALL-Mac/)
- [INSTALL-Linux](/ELL/Installing-and-building-ELL/INSTALL-Linux/)
- [INSTALL-Windows](/ELL/Installing-and-building-ELL/INSTALL-Windows/)
