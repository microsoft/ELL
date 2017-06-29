# Tutorial for getting started with computer vision in ELL

In this tutorial we will load a pre-trained Convolutional Neural Net (CNN).
We will then connect your video camera using Open CV and do some almost realtime image recognition of dogs, birds and all kinds of
fun stuff.  We will then get that working on a Raspberry Pi using the ELL model compiler.
Here's an example of what the result will look like:

![screenshot](Screenshot.png)

## Prerequisites

First we will use `Python 3.6` for this tutorial and you will need to build the 
[ELL Pythong Language Bindings](../../../interfaces/README.MD), so please follow those instructions first.
We `highly recommend` using the miniconda or full anaconda python environment because it comes with many
handy tools like `curl` which we will use later on.

You will also need a simple web cam or a pi cam.  If you don't have one handy we will show you how to load
static images or .mp4 videos and process those instead.

You will also need [Open CV](http://opencv.org/) which is used to read images from your camera, and convert
them to [NumPy](https://docs.scipy.org/doc/numpy-dev/user/quickstart.html) arrays so that ELL can process them.
There are download instructions on the opencv website, but we also created a simple [Open CV setup](opencv.md) page for you.

`Note`: You will need to build ELL `after` you install Python, so that the `CMake` step picks up on the fact that you have Python
3.6 installed.  If you did not do that, then please go back to the ELL build instructions and repeat those steps.

Lastly, we will also need `NumPy` which you can install using conda as follows:

    conda install numpy

### Downloading a pre-trained model

Now you can choose to use two different routes for pre-trained models, and so the tutorial forks at this point,
please pick one of the following:

 * [Microsoft Cognitive Toolkit](cntk.md)
 * [Darknet](darknet.md)

### Other

A list of other useful models, from both CNTK and Darknet, can be found in the [PretrainedModels](./PretrainedModels.md) section.