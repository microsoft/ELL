---
layout: default
title: Getting Started with Image Classification on the Raspberry Pi
permalink: /tutorials/Getting-Started-with-Image-Classification-on-the-Raspberry-Pi/
---
# Getting Started with Image Classification on Raspberry Pi

In this tutorial, you will download a pretrained image classifier from the [gallery](/ELL/gallery/) to your laptop or desktop computer and compile it for the Raspberry Pi. Then, you will copy the compiled classifier to your Pi and write a Python script that captures images from the Pi's camera and attempts to classify them.  

## Materials

* Laptop or desktop computer (Windows, Linux, or Mac)
* Raspberry Pi
* Raspberry Pi Camera or USB webcam (optional)
* Display (optional)
* Active cooling attachment (see our [tutorial on cooling your Pi](/ELL/tutorials/Active-Cooling-your-Raspberry-Pi-3/)) (optional)

## Prerequisites

* Install ELL on your computer ([Windows](https://github.com/Microsoft/ELL/blob/master/INSTALL-Windows.md), [Ubuntu Linux](https://github.com/Microsoft/ELL/blob/master/INSTALL-Ubuntu.md), [Mac](https://github.com/Microsoft/ELL/blob/master/INSTALL-Mac.md)). Specifically, this tutorial relies on ELL, CMake, SWIG, and Python 3.6. 
* Follow our instructions for [setting up your Raspberry Pi](/ELL/tutorials/Setting-Up-your-Raspberry-Pi).

## Download pre-trained model

First open a terminal window and activate your anaconda environment.  If you followed our setup instructions you will have
an environment named `py36` so you would do this to activate that:
```
source activate py36
```
Then cd into your ELL git repo where you did the build already
```
cd ELL/build/tutorials
```
Then make a new directory named `tutorial1` under your ELL git repo in the build/tutorials folder and download the following pre-trained model.  This model is trained on the 1000-class ImageNet data set and has already been converted to the ELL model format.  
```
mkdir tutorial1 && cd tutorial1
curl --location -o ILSVRC2012_labels.txt https://github.com/Microsoft/ELL-models/raw/master/models/ILSVRC2012/ILSVRC2012_labels.txt
curl --location -o ell.zip https://github.com/Microsoft/ELL-models/raw/master/models/ILSVRC2012/d_I224x224x3CMCMCMCMCMCMC1A/d_I224x224x3CMCMCMCMCMCMC1A.ell.zip
```

You should now have a `ILSVRC2012_labels.txt` file and a `ell.zip` file that is about 28 megabytes.
Inside ell.zip is the ell model named `d_I224x224x3CMCMCMCMCMCMC1A.ell` so unzip the archive
and place this file in the tutorial1 folder, then rename it to `ImageNet.ell`.

### Wrap the model in a Python module

For this tutorial we want to call the model from Python.  ELL provides a compiler that can take the model and compile it into code that will run on a target platform - in this case the Raspberry Pi running Linux, so it generates code for armv7-linux-gnueabihf, and for the cortex-a53 CPU.

````
python ../../tools/wrap/wrap.py ILSVRC2012_labels.txt ImageNet.ell -lang python -target pi3    
````

We also want to copy some additional python code to your pi for the purpose of running this tutorial:

````
cp ../../tools/utilities/pythonlibs/demo*.py pi3

Windows: copy ..\..\tools\utilities\pythonlibs\demo*.py pi3
````
You can also copy this static image over for testing if you do not have a web camera for your pi:

````
cp ../../tools/utilities/pitest/coffeemug.jpg pi3

Windows: copy ..\..\tools\utilities\pitest\coffeemug.jpg pi3
````

You should now have a pi3 folder that is ready to copy to your Raspberry Pi.  You can do that using the 'scp' tool.  On Windows you can use [WinSCP](https://winscp.net/eng/index.php).

### SSH into Raspberry Pi

Now log into your Raspberry Pi, either remotely using SSH or directly if you have keyboard and screen attached.

Find the 'pi3' folder you just copied over using scp or winscp and run the following:

````
cd pi3
mkdir build && cd build
cmake ..
make
cd ..
````

This builds the Python Module that is then loadable by the demo Python scripts.

### Process a static image 

Now if you followed the [Raspberry Pi Setup Instructions](/ELL/tutorials/Setting-Up-your-Raspberry-Pi) you should have a miniconda
environment named py34.  So to run the tutorial do this:

````
source activate py34
python demo.py ILSVRC2012_labels.txt --compiled ImageNet --image coffeemug.jpg
````
And it will classify the image, you should see output like this:
````
coffee mug(85%)
````

And if you have a display connected you should see something like this:

![screenshot](/ELL/tutorials/Getting-Started-with-Image-Classification-on-the-Raspberry-Pi/Screenshot.png)

### Process Video

If you have a USB camera attached to your Pi then you can also use ELL to process video frames:

````
python demo.py ILSVRC2012_labels.txt  --compiled ImageNet
````

You will see the same kind of window appear only this time it is showing the video stream.
Then when your camera is pointed at an object that the model recognizes you will see the label and 
confidence % at the top together with an estimated frame rate.

`Tip`: for quick image recognition results you can point the video camera at a web image of a dog 
on your PC screen.  ImageNet models can usually do a good job recognizing  different dog breeds and 
many types of African animals.

### Toubleshooting

If you run into trouble there's some troubleshooting instructions at the bottom of the 
[Raspberry Pi Setup Instructions](/ELL/tutorials/Setting-Up-your-Raspberry-Pi).