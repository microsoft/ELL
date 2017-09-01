---
layout: default
title: Getting Started with Image Classification on the Raspberry Pi
permalink: /tutorials/Getting-Started-with-Image-Classification-on-the-Raspberry-Pi/
---
# Getting Started with Image Classification on Raspberry Pi

### Materials

* A Raspberry Pi
* Cooling fan attachment (see our [instructions on cooling your Pi](/ELL/gallery/Raspberry-Pi-3-Fan-Mount)) (optional)
* A USB webcam (optional)
* A display (optional)

### Overview

In this tutorial, you will download a pretrained image classifier from the gallery, compile the classifier for the Raspberry Pi, and write a Python script that invokes the classifier. When the Python script runs on the Pi, you will be able to point the camera as a variety of objects and the classifier will attempt to identify them.

### Prerequisites

1. We will use `Python 3.6` for this tutorial on your dev box.
We highly recommend using the miniconda or full anaconda python environment because it comes with many
handy tools like `curl` which we will use later on.

2. You will also need a simple web cam or a pi cam.  If you don't have one handy, we will show you how to load
static images or .mp4 videos and process those instead.

3. Additional Software is needed on your Raspberry Pi - See [Setup Raspberry Pi](/ELL/tutorials/Raspberry-pi-setup). 

4. You will need to be sure you built ELL as per the ELL INSTALL-xxx.md instructions at the root of this git repo.  You will need to build ELL after you install Python from your activate your conda environment, so that the `CMake` step picks up on the fact that you have Python 3.6 installed.

### Download pre-trained model

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
curl -o categories.txt https://github.com/Microsoft/ELL-models/blob/master/models/ILSVRC2012-1k/db_I%5B224x224x3%5D_CMNCMNCMNCMNCMNCMNC%5B1%5DA.categories.txt
curl -o ell.zip https://github.com/Microsoft/ELL-models/blob/master/models/ILSVRC2012-1k/db_I%5B224x224x3%5D_CMNCMNCMNCMNCMNCMNC%5B1%5DA.ell.zip
unzip ell.zip 
```

(On windows you can use Windows explorer to unzip the model, just place the file in the same directory)

You should now have a `categories.txt` file and a `ImageNet.ell` file.

### Wrap the model in a Python callable module

For this tutorial we want to call the model from Python.  ELL provides a compiler that can take the model and compile it into code that will run on a target platform - in this case the Raspberry Pi running Linux, so it generates code for armv7-linux-gnueabihf, and for the cortex-a53 CPU.

````
python ../../tools/wrap/wrap.py categories.txt ImageNet.ell -lang python -target pi3    
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

This builds the Python Module that is then loadable by the demo python scripts.

### Process a static image 

Now if you followed the [Raspberry Pi Setup Instructions](/ELL/tutorials/Raspberry-pi-setup) you should have a miniconda
environment named py34.  So to run the tutorial do this:

````
source activate py34
python demo.py config.json categories.txt --image coffeemug.jpg
````
And it will classify the image, you should see output like this:
````
school bus(90%)
````

And if you have a display connected you should see something like this:

![screenshot](/ELL/tutorials/Getting-Started-with-Image-Classification-on-the-Raspberry-Pi/Screenshot.png)

### Process Video

If you have a USB camera attached to your Pi then you can also use ELL to process video frames:

````
python demo.py config.json categories.txt
````

You will see the same kind of window appear only this time it is showing the video stream.
Then when your camera is pointed at an object that the model recognizes you will see the label and 
confidence % at the top together with an estimated frame rate.

`Tip`: for quick image recognition results you can point the video camera at a web image of a dog 
on your PC screen.  ImageNet models can usually do a good job recognizing  different dog breeds and 
many types of African animals.

### Toubleshooting

If you run into trouble there's some troubleshooting instructions at the bottom of the 
[Raspberry Pi Setup Instructions](/ELL/tutorials/Raspberry-pi-setup).