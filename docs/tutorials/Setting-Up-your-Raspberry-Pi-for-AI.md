---
layout: default
title: Setting Up your Raspberry Pi for AI
permalink: /tutorials/Setting-Up-your-Raspberry-Pi-for-AI
---

# Setting Up your Raspberry Pi for AI

Raspberry Pis weren't designed to run AI workloads. Many AI workloads, like visual object tracking and audio keyword detection, run continously for long periods of time and require near realtime responsiveness. Moreover, modern AI pipelines are very compute-intensive and push the Raspberry Pi to the limits of its abilities. Therefore, deploying our AI code onto the Raspberry Pi poses some unique challenges.  

We can mitigate some of these challenges by choosing the right hardware and software setup. Here's a list of ideas and best-practices to turn your Raspberry Pi into an AI friendly platform.

## Power Adapter and Power Cable

Make sure to use a high quality USB power adapter. We recommend using an adapter rated for 12 Watts (2.4 Amps) per USB port. That is, if your power adapter has two USB ports, it should be rated for 24 Watts (4.8 Amps). We use the 12W USB power adapters from Apple, Anker, and Amazon Basics and those brands seem to work well.  

A high quality micro USB cable is also important. Long and thin cables will often result in a surprisingly noticeable voltage drop and will fail to provide sufficient power to your Raspberry Pi. Generic unbranded cables are hit-and-miss, and for a few dollars you can get a nice name-brand cable, like the Anker PowerLine, and save yourself a lot of frustration. 

## Consistent Performance 

Many AI tasks run continuously for hours or days and require consistent performance from the underlying hardware and operating system. You probably don't want the operating system going into energy saving mode or turning off the screen if the keyboard isn't touched for 10 minutes. You also don't want the operating system to dynamically change the processor speed. 

### Disabling Energy Star and Screensaver

Edit the file `~/.config/lxsession/LXDE-pi/autostart`, for example, by typing 

    leafpad ~/.config/lxsession/LXDE-pi/autostart

Add the following lines to this file

    @xset -dpms
    @xset s noblank 
    @xset s off 
 
The first line disables energy star and the next two lines disable the screensaver. These changes take effect after rebooting.

### Disable Dynamic Clocking

The Raspberry Pi supports dynamic clocking, which means that it can change the processor frequency according to processor load. You can check the range of processor frequencies that your Raspberry Pi is configured for by typing `lscpu`. To disable dynamic clocking, edit `/boot/config.txt`, for example, by typing 

   sudo leafpad /boot/config.txt

Add the setting

    force_turbo=1

This change takes effect after rebooting.

## Underclocking and Overclocking

Computation produces heat. Even at the default processor frequency of 700MHz, a Raspberry Pi running a serious AI workload at room temperature will typically overheat unless fitted with a physical cooling device. We recommend following our instructions for 3D printing an [active cooling attachment for your Pi](/ELL/gallery/Active-Cooling-Raspberry-Pi-3). If you don't want to physically cool your Pi, you can cool it by reducing the processor frequency (underclocking). 

If you do fit your Pi with active cooling, you can alo increase the processor frequency (overclocking) without overheating. Be warned that overclocking your Raspberry Pi can void your warranty. Also, we've experienced some wierd behavior when overclocking our Pis. Some units freeze up while other units lose their USB peripherals. You won't know how your specific Pi will react to overclocking until you try it. Overclocking isn't for the faint of heart - proceed at your own risk.

### Setting Processor Clock Frequency

To change your processor frequency, edit `/boot/config.txt`, for example, by typing 

   sudo leafpad /boot/config.txt

The default processor speed is 700 MHz. To underclock your processor, add the setting 

    arm_freq=600

The change takes effect after a reboot. Once the Pi reboots, you can confirm the change using the utility `lscpu`. To check if a frequency of 600MHz is slow enough, run your AI workload and measure the processor temperature. Measuring temperature can be done by typing

    watch /opt/vc/bin/vcgencmd measure_temp

This command prints the processor temperature every 2 seconds. When fully stressing your Pi, you want the processor temperature to stay far below 85 degrees Celsius. As a rule of thumb, we like to keep processor temperature below 60 degrees.

To overclock the processor, experiment with frequencies of 800, 900, 1000, and even 1200. You will also want to tweak some of the other parameters, such as `sdram_freq` and `over_voltage`. We like to use this configuration:

    arm_freq=900
    sdram_freq=500
    over_voltage=6
    temp_limit=75

