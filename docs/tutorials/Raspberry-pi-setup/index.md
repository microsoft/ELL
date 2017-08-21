---
layout: default
title: ELL Tutorials
permalink: /tutorials/Raspberry-pi-setup/
---

## Raspberry Pi Software Setup 

Our tutorials depend on Raspbian Jessie.  Do not use Stretch.
Download your Rasberry Pi 3 Raspbian image from 
[2017-07-05-raspbian-jessie](https://downloads.raspberrypi.org/raspbian/images/raspbian-2017-07-05/2017-07-05-raspbian-jessie.zip).

The ELL tutorials will also need the following additional components.

### CMake and OpenBLAS

````
sudo apt-get update
sudo apt-get install -y cmake libopenblas-dev
````

### Python 3.4 from Miniconda

````
wget http://repo.continuum.io/miniconda/Miniconda3-latest-Linux-armv7l.sh
chmod +x Miniconda3-latest-Linux-armv7l.sh
./Miniconda3-latest-Linux-armv7l.sh
````

When it prompts to install, we recommend [yes] for adding Miniconda3 to the PATH.

### Create Conda Environment

````
source ~/.bashrc 
conda create --name py34 python=3
source activate py34

````
Remember each time you log in to your pi run `source activate py34` to activate this conda environment.

### OpenCv

OpenCv is a computer vision library that makes it easy for us to read images from your USB camera,
resize them and convert them to NumPy arrays for processing by ELL.

````
conda install -c microsoft-ell opencv
````

### Toubleshooting

**ImportError: libavcodec.so.56: cannot open shared object file: No such file or directory**

If you do not have the built in libavcodec.so.56 then it means you have a newer version of Raspbian.
To get the older one please reflash your Raspberry pi with this version of Raspbian Jessie:
[2017-07-05-raspbian-jessie](https://downloads.raspberrypi.org/raspbian/images/raspbian-2017-07-05/2017-07-05-raspbian-jessie.zip)


**ImportError: No module named 'numpy'**

You probably forgot to activate your Python 3.4 environment using `source activate py34`.
See `Miniconda` instructions above.


