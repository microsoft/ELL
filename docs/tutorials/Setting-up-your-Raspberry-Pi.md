---
layout: default
title: Setting up your Raspberry Pi for tutorials
permalink: /tutorials/Setting-up-your-Raspberry-Pi
---

# Setting up your Raspberry Pi device for tutorials

*by Chris Lovett and Ofer Dekel*

Most Embedded Learning Library (ELL) tutorials follow a common workflow. You typically start by designing an ELL model on a laptop or desktop computer. Then, you transition to your Raspberry Pi device to compile and run your application. This means that you'll need to set up your computer and your Pi appropriately before starting any of the ELL tutorials. 

Find setup instructions for your computer here:

* [Windows](https://github.com/Microsoft/ELL/blob/master/INSTALL-Windows.md)
* [Ubuntu Linux](https://github.com/Microsoft/ELL/blob/master/INSTALL-Ubuntu.md)
* [Mac](https://github.com/Microsoft/ELL/blob/master/INSTALL-Mac.md)

This tutorial provides step-by-step instructions for setting up your Raspberry Pi device. 

## Adapters, cables, and software for your Raspberry Pi device

* **Power adapter and power cable.** AI workloads use a lot of power, so be sure to use a high quality power supply. If you use a USB power adapter and micro-USB cable, choose an adapter rated for at least 12 watts (2.4 Amps) per USB port. Suggested models include 12-watt USB power adapters from Apple, Anker, and Amazon Basics. 

Avoid long and thin micro-USB cables because using them will often create a noticeable voltage drop and they may fail to provide sufficient power to the Raspberry Pi device. 

* **Operating system**. These tutorials prefer that the operating system running on your Pi is [Raspbian Stretch](https://www.raspberrypi.org/downloads/raspbian/) so if your Pi is running Jessie you will need to upgrade.  If you really need to use Jessie for some reason see Troubleshooting below.

* **Network**. You'll use a network  to download required software to the Pi and to transfer compiled ELL models from your computer to the Pi. Connect your Pi to your network, either over Wifi or with an Ethernet cable. 

* **Pi Camera** By default, OpenCV can read images from a USB webcam, but not from the Raspberry Pi camera. To enable the Pi camera, first make sure that the camera interface is enabled by running the Pi configuration tool.

```shell
sudo raspi-config
```
1. Select **5 Interfacing Option** and press **Enter**. 
2. Select **P1 Camera** and press **Enter** . 
3. Select **Yes** to enable the camera interface. 4. Load the camera module, as follows.
```shell
sudo modprobe bcm2835-v4l2
```

* **Secure Shell (SSH)** The tutorials require you to copy files to the Raspberry Pi device. A typical way to copy files to the Pi is to use an SSH tool, such as the Unix **scp** tool or the Windows [WinSCP](https://winscp.net/eng/index.php) tool. To enable SSH on your Pi, run the Pi configuration tool.
```shell
sudo raspi-config
```
1. Select **5 Interfacing Options** and press **Enter**. 
2. Select **P2 SSH** and press **Enter**. 
3. Select **Yes** to enable the SSH server.

## Programming tools
* **CMake** You'll use CMake on the Raspberry Pi to create Python modules that can be called from the tutorial code. To install CMake on your Pi, connect to the network, open a terminal window, and type the following.
```shell
sudo apt-get update
sudo apt-get install -y cmake
```

* **OpenBLAS** This is a library for fast linear algebra operations, which can significantly increase the speed of your models. It is optional, but highly recommended. To install OpenBLAS, type the following.
```shell
sudo apt-get install -y libopenblas-dev
```

* **curl** This is a command line tool used to transfer data via URL. When files are required to be downloaded from a URL, the instructions assume you have **curl** available to perform the download. To install `curl`, type the following.
```shell
sudo apt-get install -y curl
```

* **Python 3.4 via Miniconda** All of the tutorials require Python 3.4 on the Pi (and Python 3.6 on your computer). An easy way to install Python and all the required modules is with [Miniconda](https://conda.io/miniconda.html).
To install Miniconda, type the following.
```shell
curl -O --location http://repo.continuum.io/miniconda/Miniconda3-latest-Linux-armv7l.sh
chmod +x Miniconda3-latest-Linux-armv7l.sh
./Miniconda3-latest-Linux-armv7l.sh
```
When prompted to install, reply [yes] to add Miniconda3 to your PATH. Then, create an environment, as follows.
```shell
source ~/.bashrc
conda create --name py34 python=3.4
source activate py34
```
Remember to run `source activate py34` each time you start a new terminal window.

* **[OpenCV](http://opencv.org/)** 
 is a computer vision library that makes it easy to read images from a camera, resize them, and prepare them for processing by ELL. To install OpenCV, type the following.
```shell
conda install -c microsoft-ell opencv -y
```

* **C++ OpenCV SDK** If you intend to run the C++ tutorials, you also need the C++ OpenCV SDK, which you can install on your Raspberry Pi using the following command.
```shell
sudo apt-get install -y libopencv-dev
```

## Performance tips

Raspberry Pi devices weren't designed to run AI workloads. Many AI workloads, like visual object tracking and audio keyword detection, run continuously for long periods of time and require near real-time responsiveness. You may want to disable features such as Energy Star, screensaver, and dynamic clocking in order to get the best performance for AI workloads.

### Disable Energy Star and Screensaver

Edit the file `~/.config/lxsession/LXDE-pi/autostart`, for example, by typing the following.

```shell
nano ~/.config/lxsession/LXDE-pi/autostart
```

Add the following lines to this file.

```
@xset -dpms
@xset s noblank
@xset s off
```

The first line disables Energy Star and the next two lines disable the screensaver. These changes take effect after rebooting.

### Disable dynamic clocking

The Raspberry Pi supports dynamic clocking, which means that it can change the processor frequency according to processor load. You can check the range of processor frequencies that your Raspberry Pi is configured for by typing `lscpu`. To disable dynamic clocking, edit `/boot/config.txt`, for example, by typing the following. 

```shell
sudo nano /boot/config.txt
```

Add the setting.

```
force_turbo=1
```

This change takes effect after rebooting.

### Heat management 

Computation produces heat. Even at the default processor frequency of 700 megahertz (MHz), a Raspberry Pi running a large AI workload at room temperature can overheat unless it is fitted with a physical cooling device. (See the tutorial about [actively cooling your Pi](/ELL/tutorials/Active-cooling-your-Raspberry-Pi-3/).) If you don't want to physically cool your Pi, you can cool it by reducing the processor frequency (underclocking).

If you do fit your Pi with an active cooling attachment, you can also increase the processor frequency (overclocking) without overheating. 

**Warning** Overclocking your Raspberry Pi will void your warranty. Overclocking might cause your Raspberry Pi to freeze or lose USB peripheral connections. Proceed at your own risk. 

To change your processor frequency, edit `/boot/config.txt`, for example, by typing the following.

```shell
sudo nano /boot/config.txt
```

The default processor speed is 700 MHz. To underclock your processor, add the setting

```
arm_freq=600
```

The change takes effect after a reboot. After the Pi reboots, you can confirm the change using the utility **lscpu**. 

To check if a frequency of 600 MHz is slow enough, run your AI workload and measure the processor temperature. Measuring temperature of your Pi as follows.

```shell
watch /opt/vc/bin/vcgencmd measure_temp
```

This command prints the processor temperature every 2 seconds. When fully stressing your Pi, you want the processor temperature to stay far below 85 degrees Celsius. Try keep the processor temperature below 60 degrees Celsius.

To overclock the processor, experiment with frequencies of 800, 900, 1,000, and even 1,200. You will also want to adjust some of the other parameters, such as `sdram_freq` and `over_voltage`. Try the following configuration.

```
arm_freq=900
sdram_freq=500
over_voltage=6
temp_limit=75
```

## Troubleshooting

**Raspbian Jessie**

If you really need to use Raspbian Jessie then you will need to install a different version of OpenCV.
To do that run the following:
```
conda install -c microsoft-ell/label/jessie opencv
```

**ImportError: libavcodec.so.57: cannot open shared object file: No such file or directory**

You might be running Rasbian Jessie but you installed OpenCV for Stretch. You can check this by running `lsb_release -a`. If it says 'Codename: jessie' then you need to upgrade
to [Raspbian Stretch](https://www.raspberrypi.org/downloads/raspbian/) or install Jessie version of OpenCV, see above.

**ImportError: libavcodec.so.56: cannot open shared object file: No such file or directory**

You might be running Rasbian Stretch but you installed OpenCV for Jessie. You can check this by running `lsb_release -a`. If it says 'Codename: stretch' then you need to install the OpenCV version for Stretch which you can do with this command:
```
conda install -c microsoft-ell/label/stretch opencv
```


**ImportError: No module named 'numpy'**

You probably forgot to activate your Miniconda environment using `source activate py34`. See `Miniconda` instructions above.

**Gtk-WARNING : cannot open display**

You are probably trying to use OpenCV from an SSH terminal window. You must tell OpenCV which display to use with the command `export DISPLAY=:0`. 

Note that many of the tutorial scripts terminate when they receive keyboard input - to terminate a script over SSH, type `CTRL+C`.