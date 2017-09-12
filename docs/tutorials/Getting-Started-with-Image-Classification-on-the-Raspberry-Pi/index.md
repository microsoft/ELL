---
layout: default
title: Getting Started with Image Classification on the Raspberry Pi
permalink: /tutorials/Getting-Started-with-Image-Classification-on-the-Raspberry-Pi/
---
# Getting Started with Image Classification on Raspberry Pi

![screenshot](/ELL/tutorials/Getting-Started-with-Image-Classification-on-the-Raspberry-Pi/Screenshot.png)

In this tutorial, you will download a pretrained image classifier from the [ELL gallery](/ELL/gallery/) to your laptop or desktop computer and compile it for the Raspberry Pi. Then, you will copy the compiled classifier to your Pi and write a Python script that captures images from the Pi's camera and attempts to classify them.  

## Materials

* Laptop or desktop computer (Windows, Linux, or Mac)
* Raspberry Pi
* Raspberry Pi Camera or USB webcam (optional)
* Display (optional)
* Active cooling attachment (see our [tutorial on cooling your Pi](/ELL/tutorials/Active-Cooling-your-Raspberry-Pi-3/)) (optional)

## Prerequisites

* Install ELL on your computer ([Windows](https://github.com/Microsoft/ELL/blob/master/INSTALL-Windows.md), [Ubuntu Linux](https://github.com/Microsoft/ELL/blob/master/INSTALL-Ubuntu.md), [Mac](https://github.com/Microsoft/ELL/blob/master/INSTALL-Mac.md)). Specifically, this tutorial relies on ELL, CMake, SWIG, and Python 3.6. 
* Follow our instructions for [setting up your Raspberry Pi](/ELL/tutorials/Setting-Up-your-Raspberry-Pi).

## Activate your environment, create tutorials directory
Open a terminal window and activate your anaconda environment.  If you followed our setup instructions you will have
an environment named `py36` so you would do this to activate that:
```
source activate py36
```
Then cd into your ELL git repo where you did the build already, and create a `tutorials` folder to group all the tutorials under:
```
cd ELL/build
mkdir tutorials
cd tutorials
```

## Download pre-trained model
Make a new directory named `tutorial1` in the `build/tutorials` folder which is where we will download a pre-trained model. 
```
mkdir tutorial1
cd tutorial1
```
ELL has a [gallery of pre-trained models](/ELL/gallery). For this tutorial, we'll use a model trained on the ILSVRC2012 data set. Along with the model, you'll want to download a labels file that has friendly text names for each of the 1000 classes that the model is trained to recognize.
We'll use the (ILSVRC2012_labels.txt)(https://github.com/Microsoft/ELL-models/raw/master/models/ILSVRC2012/ILSVRC2012_labels.txt) labels file and [this model file](https://github.com/Microsoft/ELL-models/raw/master/models/ILSVRC2012/d_I224x224x3CMCMCMCMCMCMC1A/d_I224x224x3CMCMCMCMCMCMC1A.ell.zip) from the gallery. Note that the model file is zipped, and has a long name indicating its architecture. For convenience, we'll just want to save it locally as `ell.zip`:
```
curl --location -o ILSVRC2012_labels.txt https://github.com/Microsoft/ELL-models/raw/master/models/ILSVRC2012/ILSVRC2012_labels.txt
curl --location -o ell.zip https://github.com/Microsoft/ELL-models/raw/master/models/ILSVRC2012/d_I224x224x3CMCMCMCMCMCMC1A/d_I224x224x3CMCMCMCMCMCMC1A.ell.zip
```
Inside `ell.zip` is the ell model named `d_I224x224x3CMCMCMCMCMCMC1A.ell`, so unzip the archive to the current directory (`tutorial1`). Recent versions of git come with the `unzip` tool:
```
unzip ell.zip
```
 Rename the `d_I224x224x3CMCMCMCMCMCMC1A.ell` model file to `model.ell`:

| Unix    | `rename d_I224x224x3CMCMCMCMCMCMC1A.ell model.ell` |
| Windows | `ren d_I224x224x3CMCMCMCMCMCMC1A.ell model.ell` |

You should now have a `ILSVRC2012_labels.txt` file and a `model.ell` file in the `tutorial1` folder.

## Wrap the model in a Python module
For this tutorial we want to call the model from Python.  ELL provides a compiler that takes a model and compiles it into code that will run on a target platform - in this case the Raspberry Pi running Linux, so it generates code for armv7-linux-gnueabihf, and for the cortex-a53 CPU.

````
python "../../tools/wrap/wrap.py" ILSVRC2012_labels.txt model.ell -lang python -target pi3   
````
You should see output similar to the following:

    compiling model...
    generating python interfaces for model in pi3
    running llc...
    success, now copy the pi3 to your target machine and build it there

We also want to copy some additional python code to your Raspberry Pi for the purpose of running this tutorial. You can also copy a static image over for testing:

| Unix    | `cp ../../tools/utilities/pythonlibs/*.py pi3`       <br> `cp ../../tools/utilities/pitest/coffeemug.jpg pi3`   |
| Windows | `copy ..\..\tools\utilities\pythonlibs\*.py pi3` <br> `copy ..\..\tools\utilities\pitest\coffeemug.jpg pi3` |

You should now have a `pi3` folder containing a python module for your model, as well as some helpful python utilities which we'll 
use in the next section.

## Call your model from a Python app
Create a new text file called `demo.py` in your `tutorial1` folder. We'll add Python code to:
* Load the compiled image classification model
* Get an image
* Run the image through the model
* Show the classification results

If you don't want to type it out, the script can found [here](/ELL/tutorials/Getting-Started-with-Image-Classification-on-the-Raspberry-Pi/demo.py), otherwise follow along below.

First, we need to import the libraries we'll be using in this app, which include system ultilities, numpy and demoHelper that we copied over from ELL utilities:
```python
import sys
import os
import numpy as np
import cv2
import demoHelper as d
```
Then we define our main function, which instantiates a helper class to load the model. The helper knows which model to load by parsing the commandline arguments:
```python
def main(args):
    helper = d.DemoHelper()
    helper.parse_arguments(args,
            "Runs the given ELL model passing images from camera or static image file\n"
            "Either the ELL model file, or the compiled model's Python module must be given,\n"
            "using the --model or --compiledModel options respectively.\n"
            "Example:\n"
            "   python demo.py categories.txt --compiledModel tutorial1/pi3/model1\n"
            "   python demo.py categories.txt --model model1.ell\n"
            "This shows opencv window with image classified by the model using given labels")
```
The command-line arguments also specify whether our images come from an image file or an attached webcam. Either way, we need to initialize our image source.
```python
    # Initialize image source
    helper.init_image_source()
    # In order to minimize console output, we compare the model's current prediction with its previous. Store the last prediction in 'lastPrediction'
    lastPrediction = ""
```
Next, we set up a loop that keeps going until the helper indicates it is done. Typically, this is when the user hits the Esc key.
At the start of every loop iteration, we want to grab an image from the image source:
```python
    while (not helper.done()):
        # Grab next frame
        frame = helper.get_next_frame()
```
`frame` now holds image data for the model. However, it often cannot be used as-is, because models are typically trained with:
* specific image sizes e.g. 224 x 224 x 3
* specific ordering of color channels e.g. RGB.
Our helper uses OpenCV to grab images from the image source (file or webcam). Their size is dependent on the source, and the ordering is always BGR from OpenCV.
Therefore, we need to crop and or resize the image while maintaining the same aspect ratio, and reorder the color channels from BGR to RGB. Since
this is such a common operation, the helper implements this in a method called `prepare_image_for_predictor`:

```python
        # Prepare the image to send to the model.
        # This involves scaling to the required input dimension and re-ordering from BGR to RGB
        data = helper.prepare_image_for_predictor(frame)
```
We are now ready to get a classify the image in the frame. The model has a `predict` method, which will return a list of probabilities for each of the 1000 classes it can detect:

```python
        # Get the model to classify the image, by returning a list of probabilities for the classes it can detect
        predictions = helper.predict(data)
```
Note that this is just an array of values, where each element is a probability between 0 and 1. It is typical to reject any that do not meet a particular threshold, since that represents low confidence results. Re-ordering so that we get only the Top 5 predictions is also useful. The index of the prediction represents the class, the value represents the score. We can use the labels file to match the index of the prediction to its text label, and then construct a string with the label and score.

```python
        # Get the (at most) top 5 predictions that meet our threshold. This is returned as a list of tuples,
        # each with the text label and the prediction score.
        top5 = helper.get_top_n(predictions, 5)

        # Turn the top5 into a text string to display
        text = "".join([helper.get_label(element[0]) + "(" + str(int(element[1]*100)) + "%)  " for element in top5])

```
Lastly, let's display the results. We'll do this in 2 ways:
* to the console by calling `print` with the prediction text. In order to not spam the console, we'll only print if the current 
prediction text is different from the previous one.
* to a GUI window. The helper has methods that use OpenCV to add text to an image and then show that image in a window

```python
        save = False
        if (text != lastPrediction):
            print(text)
            save = True
            lastPrediction = text

        # Draw the text on the frame
        frameToShow = frame
        helper.draw_label(frameToShow, text)
        helper.draw_fps(frameToShow)

        # Show the new frame
        helper.show_image(frameToShow, save)
```

Your `pi3` folder is ready to copy to your Raspberry Pi.  You can do that using the 'scp' tool.  On Windows you can use [WinSCP](https://winscp.net/eng/index.php).

## SSH into Raspberry Pi

Now log into your Raspberry Pi, either remotely using SSH or directly if you have keyboard and screen attached.

Find the `pi3` folder you just copied over using scp or winscp and run the following:

````
cd pi3
mkdir build && cd build
cmake ..
make
cd ..
````

This builds the Python Module that is then loadable by the demo Python scripts.

## Process a static image 

Now if you followed the [Raspberry Pi Setup Instructions](/ELL/tutorials/Setting-Up-your-Raspberry-Pi) you should have a miniconda
environment named py34.  So to run the tutorial do this:

````
source activate py34
python demo.py ILSVRC2012_labels.txt --compiledModel model --image coffeemug.jpg
````
And it will classify the image, you should see output like this:
````
coffee mug(85%)
````

And if you have a display connected you should see something like the screenshot at the top of this page.

## Process Video

If you have a USB camera attached to your Pi then you can also use ELL to process video frames:

````
python demo.py ILSVRC2012_labels.txt  --compiledModel model
````

You will see the same kind of window appear only this time it is showing the video stream.
Then when your camera is pointed at an object that the model recognizes you will see the label and 
confidence % at the top together with an estimated frame rate.

`Tip`: for quick image recognition results you can point the video camera at a web image of a dog 
on your PC screen.  ImageNet models can usually do a good job recognizing  different dog breeds and 
many types of African animals.

## Next steps
Different models have different characteristics. For example, some are slow but accurate, while others are faster and less accurate. Some have different power draw than others.

Experiment with which model works best for you by downloading other models in the [ELL gallery](/ELL/gallery/).

Try these related tutorials:
* [Fun with Dogs and Cats](/ELL/tutorials/Fun-with-Dogs-and-Cats/)
* [Comparing Image Classification models side by side on the Raspberry Pi](/ELL/tutorials/Comparing-Image-Classification-models-side-by-side-on-the-Raspberry-Pi/)

## Toubleshooting

If you run into trouble there's some troubleshooting instructions at the bottom of the 
[Raspberry Pi Setup Instructions](/ELL/tutorials/Setting-Up-your-Raspberry-Pi).