---
layout: default
title: Setting up your Raspberry Pi for tutorials
permalink: /tutorials/Setting-up-your-Raspberry-Pi
---

# Setting up your Raspberry Pi for tutorials

*by Chris Lovett and Ofer Dekel*

Most of our tutorials follow a common workflow. The first steps involve authoring and designing an ELL model - these steps are typically done on a laptop or desktop computer (Windows, Linux, or Mac). The next steps involve deploying the ELL model and invoking it on the Raspberry Pi - these steps are typically done on the Pi itself. Therefore, you need to setup both your computer and your Pi with the necessary prerequisites. The step-by-step instructions below describe how to setup your Pi. Installation steps for your computer can be found here:

* [Windows](https://github.com/Microsoft/ELL/blob/master/INSTALL-Windows.md)
* [Ubuntu Linux](https://github.com/Microsoft/ELL/blob/master/INSTALL-Ubuntu.md)
* [Mac](https://github.com/Microsoft/ELL/blob/master/INSTALL-Mac.md)

## Basic Pi Setup

### Power Adapter and Power Cable
AI workloads guzzle power, so be sure to use a high quality power supply. If you use a USB power adapter and micro USB cable, choose an adapter rated for at least 12 Watts (2.4 Amps) per USB port. We've had good experience with 12W-per-port USB power adapters from Apple, Anker, and Amazon Basics. Long and thin micro USB cables will often result in a noticeable voltage drop and fail to provide sufficient power to the Raspberry Pi. For a few extra dollars you can get a nice name-brand cable, like the Anker PowerLine, and save yourself a lot of frustration.

### Operating System
Our tutorials assume that the operating system running on your Pi is *Raspbian Jessie* ([NOOBS](https://downloads.raspberrypi.org/NOOBS/images/NOOBS-2017-07-05/) or [image](https://downloads.raspberrypi.org/raspbian/images/raspbian-2017-07-05/)), not the more recent *Raspbian Stretch*.

### Network
Connect your Pi to the network, either over Wifi or with an Ethernet cable. We will use the network both to download required software to the Pi and to transfer compiled ELL models from your computer to the Pi.

### CMake
We use CMake on the Raspberry Pi to create Python modules that can be called from our tutorial code. To install CMake on your Pi, connect to the network, open a terminal window, and type

```shell
sudo apt-get update
sudo apt-get install -y cmake
```

### OpenBLAS
OpenBLAS is a library for fast linear algebra operations, which can significantly increase the speed of your models. It is optional, but highly recommended. To install OpenBLAS, type the following.

```shell
sudo apt-get install -y libopenblas-dev
```

### curl
`curl` is a command line tool used to transfer data via URL. When files are required to be downloaded from a URL, the instructions assume you have `curl` available to perform the download. To install `curl`, type the following:
```shell
sudo apt-get install -y curl
```

### Python 3.4 via Miniconda
Our tutorials require Python 3.4 on the Pi (and Python 3.6 on your computer).
An easy way to install Python and all the required modules is with [Miniconda](https://conda.io/miniconda.html).
To install Miniconda, type the following.

```shell
curl -O --location http://repo.continuum.io/miniconda/Miniconda3-latest-Linux-armv7l.sh
chmod +x Miniconda3-latest-Linux-armv7l.sh
./Miniconda3-latest-Linux-armv7l.sh
```

When prompted to install, reply [yes] to add Miniconda3 to your PATH. Next create an environment, as follows.

```shell
source ~/.bashrc
conda create --name py34 python=3.4
source activate py34
```

Remember to run `source activate py34` each time you start a new terminal window.

### OpenCV
[OpenCV](http://opencv.org/) is a computer vision library that enables us to read images from a camera, resize them, and prepare them for processing by ELL. To install OpenCV, type the following.

```shell
conda install -c microsoft-ell opencv -y
```

To run the `C++` tutorials you also need the C++ OpenCV SDK, which you can install on your Raspberry Pi using the following command.

```shell
sudo apt-get install -y libopencv-dev
```

By default, OpenCV can read images from a USB webcam, but not from the Raspberry Pi camera. To enable the Pi camera, first make sure that the camera interface is enabled by running the Pi configuration tool.

```shell
sudo raspi-config
```

Select `5 Interfacing Options`, hit `Enter`, select `P1 Camera` and hit `Enter` again. Select `Yes` to enable the camera interface. Then, load the camera module, as follows.

```shell
sudo modprobe bcm2835-v4l2
```

### SSH
Our tutorials require copying files to run on the Pi. A typical way to copy files to the Pi is to use the Unix `scp` tool or the Windows [WinSCP](https://winscp.net/eng/index.php) tool.

To enable SSH on your Pi, run the Pi configuration tool.

```shell
sudo raspi-config
```

Select `5 Interfacing Options`, hit `Enter`, select `P2 SSH` and hit `Enter` again. Select `Yes` to enable the SSH server.

## Tips and Tweaks

Raspberry Pis weren't designed to run AI workloads. Many AI workloads, like visual object tracking and audio keyword detection, run continuously for long periods of time and require near realtime responsiveness. You probably don't want the operating system to go into energy saving mode, turning off the screen and dynamically changing the processor speed.

### Disabling Energy Star and Screensaver

Edit the file `~/.config/lxsession/LXDE-pi/autostart`, for example, by typing

```shell
leafpad ~/.config/lxsession/LXDE-pi/autostart
```

Add the following lines to this file.

```
@xset -dpms
@xset s noblank
@xset s off
```

The first line disables energy star and the next two lines disable the screensaver. These changes take effect after rebooting.

### Disable Dynamic Clocking

The Raspberry Pi supports dynamic clocking, which means that it can change the processor frequency according to processor load. You can check the range of processor frequencies that your Raspberry Pi is configured for by typing `lscpu`. To disable dynamic clocking, edit `/boot/config.txt`, for example, by typing

```shell
sudo leafpad /boot/config.txt
```

Add the setting

```
force_turbo=1
```

This change takes effect after rebooting.

### Underclocking and Overclocking

Computation produces heat. Even at the default processor frequency of 700MHz, a Raspberry Pi running a serious AI workload at room temperature can overheat unless fitted with a physical cooling device. See [actively cooling your Pi](/ELL/tutorials/Active-cooling-your-Raspberry-Pi-3/). If you don't want to physically cool your Pi, you can cool it by reducing the processor frequency (underclocking).

If you do fit your Pi with an active cooling attachment, you can also increase the processor frequency (overclocking) without overheating. **Be warned** that overclocking your Raspberry Pi can void your warranty. Also, we've experienced some weird behavior when overclocking Raspberry Pis: some Pi units freeze up while other units lose their USB peripherals. You won't know how your specific Pi will react to overclocking until you try it. Overclocking isn't for the faint of heart - **proceed at your own risk**.

To change your processor frequency, edit `/boot/config.txt`, for example, by typing the following.

```shell
sudo leafpad /boot/config.txt
```

The default processor speed is 700 MHz. To underclock your processor, add the setting

```
arm_freq=600
```

The change takes effect after a reboot. Once the Pi reboots, you can confirm the change using the utility `lscpu`. To check if a frequency of 600MHz is slow enough, run your AI workload and measure the processor temperature. Measuring temperature can be done as follows.

```shell
watch /opt/vc/bin/vcgencmd measure_temp
```

This command prints the processor temperature every 2 seconds. When fully stressing your Pi, you want the processor temperature to stay far below 85 degrees Celsius. As a rule of thumb, we like to keep processor temperature below 60 degrees.

To overclock the processor, experiment with frequencies of 800, 900, 1000, and even 1200. You will also want to tweak some of the other parameters, such as `sdram_freq` and `over_voltage`. We like to use the following configuration.

```
arm_freq=900
sdram_freq=500
over_voltage=6
temp_limit=75
```

## Troubleshooting

**ImportError: libavcodec.so.56: cannot open shared object file: No such file or directory**

If you do not have the built in libavcodec.so.56, it probably means that you are running a newer version of Raspbian. Our tutorials currently require Raspbian Jessie.


**ImportError: No module named 'numpy'**

You probably forgot to activate your Miniconda environment using `source activate py34`. See `Miniconda` instructions above.

**(ELL model:18037): Gtk-WARNING **: cannot open display**

You are probably trying to use OpenCV from an SSH terminal window. In this case, you simply need to tell OpenCV which display to use with the command `export DISPLAY=:0`. Many of our tutorials also expect keyboard input to terminate the demo. From SSH you can kill the application from the command line by typing CTRL+C.