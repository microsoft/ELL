# Getting Started with Computer Vision

For this tutorial, you'll need a Raspberry Pi with a web cam and a screen. After completing these instructions, you can point your Pi's web cam at a variety of objects, and the deployed software will print a label for whatever object appears on camera. For example, we pointed our Pi's cam at a cup of coffee on our desk and saw this on the Pi's screen:

![coffee mug](coffeemug.jpg)

This tutorial shows you how to import a pre-trained image recognition model, compile it for the Raspberry Pi, and write the code that runs the image recognizer and prints the labels.

In this tutorial we will load a pre-trained Convolutional Neural Net (CNN).
We will then connect your video camera using OpenCV and do some almost real-time image recognition of dogs, birds and all kinds of
fun stuff.  We will then get that working on a Raspberry Pi using the ELL model compiler.


## Prerequisites

1. We will use `Python 3.6` for this tutorial.
We highly recommend using the miniconda or full anaconda python environment because it comes with many
handy tools like `curl` which we will use later on.

2. You will also need a simple web cam or a pi cam.  If you don't have one handy, we will show you how to load
static images or .mp4 videos and process those instead.

3. To read images from the camera, you will need [OpenCV](http://opencv.org/). There are download instructions on the opencv website, but we also created a simple [OpenCV setup](opencv.md) page for you.

4. You also need  [NumPy](https://docs.scipy.org/doc/numpy-dev/user/quickstart.html) since ELL  processes images as NumPy arrays. `NumPy` can install using conda as follows:

    ~~~~
    conda install numpy
    ~~~~

5. You will need to be sure you built the ELL Python Language Bindings as per the ELL INSTALL-xxx.md instructions at the root of this git repo.  You will need to build ELL after you install Python (or activate your conda environment), so that the `CMake` step picks up on the fact that you have Python 3.6 installed.

### Downloading a pre-trained model

Now you can choose to use two different routes for pre-trained models, and so the tutorial forks at this point,
please pick one of the following:

 * [Microsoft Cognitive Toolkit](cntk.md)
 * [Darknet](darknet.md)

### Other

A list of other useful models, from both CNTK and Darknet, can be found in the [PretrainedModels](./PretrainedModels.md) section.
