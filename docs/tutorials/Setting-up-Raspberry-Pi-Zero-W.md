---
layout: default
title: Setting up your Raspberry Pi Zero W for tutorials
permalink: /tutorials/Setting-up-Raspberry-Pi-Zero-W
---

# Setting up Raspberry Pi Zero W for tutorials

*by Eric Tang*

This is a tutorial for those of you who really want to run ELL on a Raspberry Pi Zero W. Since Raspberry Pi Zero is based on ARMV6 which has little support from latest Conda, you have to build many python packages in other tutorials by your own from source. You need lots of patience to make it work since Raspberry Pi Zero has much less power compared with a Raspberry Pi 3 Model B. We still recommend you to follow the instructions for [setting up your Raspberry Pi](/ELL/tutorials/Setting-up-your-Raspberry-Pi) for a much easier starting experience.

## Basic Pi Setup

### Operating System
Our tutorials assume that the operating system running on your Pi is *Raspbian Jessie* ([NOOBS](https://downloads.raspberrypi.org/NOOBS/images/NOOBS-2017-07-05/) or [image](https://downloads.raspberrypi.org/raspbian/images/raspbian-2017-07-05/)), not the more recent *Raspbian Stretch*.

### Network
Connect your Pi to the network, either over Wifi or with an Ethernet cable. We will use the network both to download required software to the Pi and to transfer compiled ELL models from your computer to the Pi. Raspberry Pi Zero has a Wifi module on board which makes it easy to connect over Wifi. If you are using a Raspberry Pi Zero with no integrated Wifi module, you may have to connect your board via a Ethernet cable or install a third part wifi dongle.

### Increase swap file size on Raspberry Pi Zero
We will build a few packages from source on our Raspberry Pi Zero. It's suggested to increase the swap file size on your Raspberry Pi Zero to avoid memory issue during your installation. 
Start this by editing the swap space configuration file:

```shell
sudo nano /etc/dphys-swapfile
```

Add the following line to the opened configuration file:

```shell
CONF_SWAPSIZE=1024
```

Save the file and reload the swap space configuration by type the following commands:

```shell
sudo /etc/init.d/dphys-swapfile stop
sudo /etc/init.d/dphys-swapfile start
```

### CMake
We use CMake on the Raspberry Pi to create Python modules that can be called from our tutorial code. To install CMake on your Pi, connect to the network, open a terminal window, and type

```shell
sudo apt-get update
sudo apt-get install -y cmake
```

### Python 3.4 installation
Our tutorials require Python 3.4 on the Pi (and Python 3.6 on your computer). Since the last Conda support for ARMV6 is not actively maintained, here we install Python 3.4 directly into our system path. We will also install pip3 which is a python package management tool.

To install Python 3.4 on your Raspbian Jessie, type the following:

```shell
sudo apt-get install python3 python3-dev
wget https://bootstrap.pypa.io/get-pip.py
sudo python3 get-pip.py
```

### OpenBLAS
OpenBLAS is a library for fast linear algebra operations, which can significantly increase the speed of your models. It is optional, but highly recommended.

You should not use the libopenblas-dev from the Raspbian distribution channel. Instead, build it from the source to make sure it's built for ARMV6 which is our Raspberry Pi Zero runs on. 

To install OpenBLAS, type the following.

```shell
git clone https://github.com/xianyi/OpenBLAS.git OpenBLAS
cd OpenBLAS
make
sudo make install
```

The whole build process will take about 4-5 hours. The above steps will install a working copy of OpenBLAS to /opt/OpenBLAS

### Numpy
Numpy is a numerical processing package required by OpenCV and our tutorial. Here we will install a copy of Numpy package which uses the OpenBlas we installed in the last step:

```shell
cd ~
git clone https://github.com/numpy/numpy.git numpy
cd numpy
```

create a new file called sit.cfg under numpy directory and type in following contents:

```shell
[default]
include_dirs = /opt/OpenBLAS/include
library_dirs = /opt/OpenBLAS/lib
 
[openblas]
openblas_libs = openblas
library_dirs = /opt/OpenBLAS/lib
 
[lapack]
lapack_libs = openblas
library_dirs = /opt/OpenBLAS/lib
```

now still under the same directory, run the following commands:

```shell
sudo pip3 install cython -v
sudo apt-get install -y gfortran
python3 setup.py build --fcompiler=gnu95
sudo python3 setup.py install
```

The cython will take a while to install on Raspberry Pi Zero W, so keep patient.

### curl
`curl` is a command line tool used to transfer data via URL. When files are required to be downloaded from a URL, the instructions assume you have `curl` available to perform the download. To install `curl`, type the following:
```shell
sudo apt-get install -y curl
```

Remember to run `python3 xxxx.py` when you run any python scripts from ELL tutorial.

### OpenCV
[OpenCV](http://opencv.org/) is a computer vision library that enables us to read images from a camera, resize them, and prepare them for processing by ELL. We cannot install it easily through Anaconda like we do for Raspberry Pi 3 since we don't have a Anaconda tool here, so we have to install it manually again. Now take a deep breath, this will take you lots of time.

Before we start building OpenCV source, we need to install a few tools. Install them by following the steps below:

```shell
sudo apt-get install build-essential pkg-config -y
sudo apt-get install libjpeg-dev libtiff5-dev libjasper-dev libpng12-dev
sudo apt-get install libavcodec-dev libavformat-dev libswscale-dev libv4l-dev
sudo apt-get install libxvidcore-dev libx264-dev
sudo apt-get install libgtk2.0-dev libgtk-3-dev
sudo apt-get install libatlas-base-dev gfortran
```

OK, now we'll download the OpenCV source and get into our OpenCV installation business.

```shell
cd ~
wget -O opencv.zip https://github.com/Itseez/opencv/archive/3.3.0.zip
unzip opencv.zip
wget -O opencv_contrib.zip https://github.com/Itseez/opencv_contrib/archive/3.3.0.zip
unzip opencv_contrib.zip
cd ~/opencv-3.3.0/
mkdir build
cd build
cmake -D CMAKE_BUILD_TYPE=RELEASE \
    -D CMAKE_INSTALL_PREFIX=/usr/local \
    -D INSTALL_PYTHON_EXAMPLES=ON \
    -D OPENCV_EXTRA_MODULES_PATH=~/opencv_contrib-3.3.0/modules \
    -D BUILD_EXAMPLES=ON ..
```

Check the output of the CMake and make sure the "Python 3" section includes correct path to your environment. If everything looks fine, type in the following command in the OpenCV directory:

```shell
make
```

Now go get a coffee or have a good sleep and leave your Pi Zero does its own work. The whole compiling process will take ~10 hours on a Raspberry Pi Zero. 

Once you get through with your compilation, congratulations, you get the hardest work done so far. Keep moving by type in the following command:

```shell
sudo make install
```

OK. Now your OpenCV should be successfully installed. Type in the following command to verify the installation:

```shell
python3 -c "import cv2; print(cv2.__version__)"
```

You should see an output of 3.3.0.


## Tips and Tweaks

You should check out the tutorial for [setting up your Raspberry Pi](/ELL/tutorials/Setting-up-your-Raspberry-Pi) for topics such as setting up camera and underclocking/overcloking. 