---
layout: default
title: Comparing Image Classification models side by side on the Raspberry Pi
permalink: /tutorials/Comparing-Image-Classification-models-side-by-side-on-the-Raspberry-Pi/
---
# Comparing Image Classification models side by side on the Raspberry Pi

![screenshot](/ELL/tutorials/Comparing-Image-Classification-models-side-by-side-on-the-Raspberry-Pi/Screenshot.jpg)

### Materials

* A Raspberry Pi
* Cooling fan attachment (see our [instructions on cooling your Pi](/ELL/gallery/Raspberry-Pi-3-Fan-Mount)) (optional)
* A USB webcam (optional)
* A display (optional)

### Overview

In this tutorial, you will download two pretrained image classifiers from the gallery, compile the classifiers for the Raspberry Pi, and write a Python script that invokes the classifiers in a round-robin fashion and displays results side by side. When the Python script runs on the Pi, you will be able to point the camera at a variety of objects and compare both result and evaluation time per frame of the the classifiers.

### Prerequisites

We recommend that you are familiar with the concepts in [Getting Started with Image Classification on the Raspberry Pi](/ELL/tutorials/Getting-Started-with-Image-Classification-on-the-Raspberry-Pi/).

1. We will use `Python 3.6` for this tutorial on your dev box.
We highly recommend using the miniconda or full anaconda python environment because it comes with many
handy tools like `curl` which we will use later on.

2. You will also need a simple web cam or a pi cam.  If you don't have one handy, we will show you how to load
static images or .mp4 videos and process those instead.

3. Additional Software is needed on your Raspberry Pi - See [Setup Raspberry Pi](/ELL/tutorials/Setting-Up-your-Raspberry-Pi). 

4. You will need to be sure you built ELL as per the ELL INSTALL-xxx.md instructions at the root of this git repo.  You will need to build ELL after you install Python from your activate your conda environment, so that the `CMake` step picks up on the fact that you have Python 3.6 installed.

### Download pre-trained models

First open a terminal window and activate your anaconda environment.  If you followed our setup instructions you will have
an environment named `py36` so you would do this to activate that:
```
source activate py36
```
Then cd into your ELL git repo where you did the build already
```
cd ELL/build
mkdir tutorials
cd tutorials
```
Then make a new directory named `sideBySideTutorial` under your ELL git repo in the build/tutorials folder and download the following pre-trained models.  These models are trained on the 1000-class ImageNet data set and have already been converted to the ELL model format.  
```
mkdir sideBySideTutorial
cd sideBySideTutorial
curl --location -o ImageNetLabels.txt https://github.com/Microsoft/ELL-models/raw/master/models/ILSVRC2012/ImageNetLabels.txt
curl --location -o ell1.zip https://github.com/Microsoft/ELL-models/raw/master/models/ILSVRC2012/d_I224x224x3CMCMCMCMCMCMC1A/d_I224x224x3CMCMCMCMCMCMC1A.ell.zip
```

You should now have a `ImageNetLabels.txt` file and a `ell1.zip` file that is about 28 megabytes.
Inside ell1.zip is the ell model named `d_I224x224x3CMCMCMCMCMCMC1A.ell` so unzip the archive
and place this file in the sideBySideTutorial folder, then rename it to `model1.ell`.

Download the next model from the gallery:
```
curl --location -o ell2.zip https://github.com/Microsoft/ELL-models/raw/master/models/ILSVRC2012/v_I160x160x3CCMCCMCCCMCCCMCCCMF2048/v_I160x160x3CCMCCMCCCMCCCMCCCMF2048.ell.zip 
```
You should now have `ell2.zip` file that is about 166 megabytes.
Inside ell2.zip is the ell model named `v_I160x160x3CCMCCMCCCMCCCMCCCMF2048.ell` so unzip the archive
and place this file in the sideBySideTutorial folder, then rename it to `model2.ell`.

At this point, you have the labels in `ImageNetLabels.txt`, and 2 ELL models: `model1.ell` and `model2.ell`.

### Wrap the models in Python callable modules

For this tutorial we want to call the model from Python.  ELL provides a compiler that can take the model and compile it into code that will run on a target platform - in this case the Raspberry Pi running Linux, so it generates code for armv7-linux-gnueabihf, and for the cortex-a53 CPU.

````
python ../../tools/wrap/wrap.py ImageNetLabels.txt model1.ell -lang python -target pi3 -outdir model1
python ../../tools/wrap/wrap.py ImageNetLabels.txt model2.ell -lang python -target pi3 -outdir model2
````

We also want to copy some additional python code to your pi for the purpose of running this tutorial:

````
cp ../../tools/utilities/pythonlibs/*.py pi3

Windows: copy ..\..\tools\utilities\pythonlibs\*.py pi3
````
You can also copy this static image over for testing if you do not have a web camera for your pi:

````
cp ../../tools/utilities/pitest/coffeemug.jpg pi3

Windows: copy ..\..\tools\utilities\pitest\coffeemug.jpg pi3
````

You should now have `model1` and `model2` folders that are ready to copy to your Raspberry Pi.  You can do that using the 'scp' tool.  On Windows you can use [WinSCP](https://winscp.net/eng/index.php).

### SSH into Raspberry Pi

Now log into your Raspberry Pi, either remotely using SSH or directly if you have keyboard and screen attached.

Find the `model1` folder you just copied over using scp or winscp and run the following:

````
cd model1
mkdir build && cd build
cmake ..
make
cd ..
````
This builds the Python Module that is then loadable by the demo Python scripts. Do the same for the `model2` folder:

````
cd model2
mkdir build && cd build
cmake ..
make
cd ..
````

### Process a static image 

Now if you followed the [Raspberry Pi Setup Instructions](/ELL/tutorials/Setting-Up-your-Raspberry-Pi) you should have a miniconda
environment named py34.  So to run the tutorial do this:

````
source activate py34
python sideBySideDemo.py ImageNetLabels.txt --compiled model1/model1,model2/model2 --image coffeemug.jpg
````

If you have a display connected you should see the screen shot at the top of this page.


### Process Video

If you have a USB camera attached to your Pi then you can also use ELL to process video frames:

````
python sideBySideDemo.py ImageNetLabels.txt  --compiled model1/model1,model2/model2
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