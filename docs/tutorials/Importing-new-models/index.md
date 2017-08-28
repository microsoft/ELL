---
layout: default
title: Importing Models
permalink: /tutorials/Importing-new-models/
---

# Importing New Models

### Materials

- Just your desktop PC (Windows, Mac, Linux)
- USB web cam (optional)

### Prerequisites

1. We will use `Python 3.6` for this tutorial on your desktop PC.

2. You will need to be sure you built ELL as per [Installing and building ELL](/ELL/Installing-and-building-ELL/).  
You will need to build ELL after you install Python from your activate your conda environment, 
so that the `CMake` step picks up on the fact that you have Python 3.6 installed.

3. This tutorial assumes your ELL bits live in the location "~/git/ELL".  On Linux and Mac
this may well be the case.  On Windows you may need to replace this syntax with a real Windows
drive location like this: "d:/git/ELL".  **Note:** Windows supports forward slashes if you
include the entire path in double quotes.  This makes it easier to share tutorial text 
across Linux and Windows.  We will also use "cp" to copy files, on windows change that to "copy".

4. CMake 3.5 or newer, which you needed to build ELL.  On Linux you will need GCC 4.9 ore later or CLang 3.9.
On Windows you will also need Visual Studio 2015 ot 2017 with C++ language feature installed.

### Overview

So far in our tutorials we have used pre-trained ELL models from the ELL model gallery.
But eventually you will want to use a model that you trained or one that the ELL team has
not converted to the ELL model format.

In this tutorial you will learn how to import Darknet and Microsoft Cognitive Toolkit (CNTK) models and convert them 
to the ELL model format, then we will test them to make sure they work.

## Darknet

First we start with Darknet.  Feel free to skip to the CNTK section below if that's what
you are more interested in.

[Joseph Redmon](https://pjreddie.com/darknet/) has published some CNN models on his website.
Joseph calls his project *Darknet*, which sounds spooky, but his models are harmless, and really quite fast. 

We have provided a Python script that can load those Darknet models.
See the *darknet_import.py* Python module (found in the `tools/importers/darknet` directory)

So let's download a Darknet ImageNet reference model using `curl` to a new temporary directory
on your PC using your `anaconda environment`:

    curl -O https://raw.githubusercontent.com/pjreddie/darknet/master/cfg/darknet.cfg
    curl -O https://pjreddie.com/media/files/darknet.weights

If your conda environment doesn't have curl just run this:

    conda install curl

So, to convert this Darknet model to the ELL format simply create a new directory

    python "~/git/ELL/tools/importers/darknet/darknet_import.py" darknet.cfg darknet.weights

You should see some output like this while it is loading the model:

    convolutional:  224 x 224 x 3 ->  224 x 224 x 16 , pad  1
    max_pool:  224 x 224 x 16 ->  112 x 112 x 16 , pad  0
    convolutional:  112 x 112 x 16 ->  112 x 112 x 32 , pad  1
    max_pool:  112 x 112 x 32 ->  56 x 56 x 32 , pad  0
    convolutional:  56 x 56 x 32 ->  56 x 56 x 64 , pad  1
    max_pool:  56 x 56 x 64 ->  28 x 28 x 64 , pad  0
    convolutional:  28 x 28 x 64 ->  28 x 28 x 128 , pad  1
    max_pool:  28 x 28 x 128 ->  14 x 14 x 128 , pad  0
    convolutional:  14 x 14 x 128 ->  14 x 14 x 256 , pad  1
    max_pool:  14 x 14 x 256 ->  7 x 7 x 256 , pad  0
    convolutional:  7 x 7 x 256 ->  7 x 7 x 512 , pad  1
    max_pool:  7 x 7 x 512 ->  4 x 4 x 512 , pad  1
    convolutional:  4 x 4 x 512 ->  4 x 4 x 1024 , pad  1
    convolutional:  4 x 4 x 1024 ->  4 x 4 x 1000 , pad  0
    avg_pool:  4 x 4 x 1000 ->  1 x 1 x 1000 , pad  0
    softmax:  1 x 1 x 1000 ->  1 x 1 x 1000
    Saving model file: 'darknet.ellmodel'
    Saving config file: 'darknet_config.json'

Wait for a few seconds (depending on your computer performance) and you should see the 
following new files in your directory:

    darknet.ellmodel
    darknet_config.json

The config file simply contains information about the inputs and outputs for this model.

### Testing Darknet

Now let's test that this model works on your PC.  For this you will need to copy the
`darknetImageNetLabels.txt` from this tutorial folder:

    cp ~/git/ELL/docs/Importing-new-models/darknetImageNetLabels.txt .

 Then to build an ELL project that will run on your PC:

    python "~/git/ELL/wrap.py darknet_config.json darknetImageNetLabels.txt darknet.ellmodel -target host
    cd host
    mkdir build
    cd build

Run CMake:
    
|=
| Linux | cmake ..
|-
| Windows with Visual Studio 2015 | cmake -G "Visual Studio 14 2015 Win64" ..
|-
| Windows with Visual Studio 2017 | cmake -G "Visual Studio 15 2017 Win64" ..
|=


Then to build it:

    cmake --build . --config Release
    cd ..
    cp /y ~/git/ELL/tools/utilities/pythonlibs/demo* .

And finally to run the test on video input:

    python demo.py darknet_config.json darknetImageNetLabels.txt

And to test with a static image:

    python demo.py darknet_config.json darknetImageNetLabels.txt --image coffeemug.jpg

## CNTK

To use the CNTK you will need to install the [CNTK python packages](https://docs.microsoft.com/en-us/cognitive-toolkit/setup-linux-python).
From your conda environment it will be something like this, but you need to find
the version that matches your Python environment version exactly, this is the
Python 3.6 version:

    pip install https://cntk.ai/PythonWheel/CPU-Only/cntk-2.0-cp36-cp36m-win_amd64.whl

We have provided a Python script that can load CNTK models.
See the *cntk_import.py* Python module (found in tools/importers/cntk)

After building ELL and the ELL Python Language Bindings create a new temporary directory
where we will put the cntk model and open a new conda environment terminal window in
that location.

Then download the CNTK ImageNet reference model as follows:

    curl -O https://www.cntk.ai/Models/Caffe_Converted/VGG16_ImageNet_Caffe.model

If your conda environment doesn't have curl just run this:

    conda install curl

Now import this model into the ELL format:

    python "~/git/ELL/tools/importers/cntk/cntk_import.py" VGG16_ImageNet_Caffe.model

Now you should see some output like this:

    Loading...
    Selected CPU as the process wide default device.

    Finished loading.
    Pre-processing...
    Convolution :  226x226x3  ->  224x224x64 | input padding 1  output padding 0
    ReLU :  224x224x64  ->  226x226x64 | input padding 0  output padding 1
    Convolution :  226x226x64  ->  224x224x64 | input padding 1  output padding 0
    ReLU :  224x224x64  ->  224x224x64 | input padding 0  output padding 0
    MaxPooling :  224x224x64  ->  114x114x64 | input padding 0  output padding 1
    Convolution :  114x114x64  ->  112x112x128 | input padding 1  output padding 0
    ReLU :  112x112x128  ->  114x114x128 | input padding 0  output padding 1
    Convolution :  114x114x128  ->  112x112x128 | input padding 1  output padding 0
    ReLU :  112x112x128  ->  112x112x128 | input padding 0  output padding 0
    MaxPooling :  112x112x128  ->  58x58x128 | input padding 0  output padding 1
    Convolution :  58x58x128  ->  56x56x256 | input padding 1  output padding 0
    ReLU :  56x56x256  ->  58x58x256 | input padding 0  output padding 1
    Convolution :  58x58x256  ->  56x56x256 | input padding 1  output padding 0
    ReLU :  56x56x256  ->  58x58x256 | input padding 0  output padding 1
    Convolution :  58x58x256  ->  56x56x256 | input padding 1  output padding 0
    ReLU :  56x56x256  ->  56x56x256 | input padding 0  output padding 0
    MaxPooling :  56x56x256  ->  30x30x256 | input padding 0  output padding 1
    Convolution :  30x30x256  ->  28x28x512 | input padding 1  output padding 0
    ReLU :  28x28x512  ->  30x30x512 | input padding 0  output padding 1
    Convolution :  30x30x512  ->  28x28x512 | input padding 1  output padding 0
    ReLU :  28x28x512  ->  30x30x512 | input padding 0  output padding 1
    Convolution :  30x30x512  ->  28x28x512 | input padding 1  output padding 0
    ReLU :  28x28x512  ->  28x28x512 | input padding 0  output padding 0
    MaxPooling :  28x28x512  ->  16x16x512 | input padding 0  output padding 1
    Convolution :  16x16x512  ->  14x14x512 | input padding 1  output padding 0
    ReLU :  14x14x512  ->  16x16x512 | input padding 0  output padding 1
    Convolution :  16x16x512  ->  14x14x512 | input padding 1  output padding 0
    ReLU :  14x14x512  ->  16x16x512 | input padding 0  output padding 1
    Convolution :  16x16x512  ->  14x14x512 | input padding 1  output padding 0
    ReLU :  14x14x512  ->  14x14x512 | input padding 0  output padding 0
    MaxPooling :  14x14x512  ->  7x7x512 | input padding 0  output padding 0
    linear :  7x7x512  ->  1x1x4096 | input padding 0  output padding 0
    ReLU :  1x1x4096  ->  1x1x4096 | input padding 0  output padding 0
    linear :  1x1x4096  ->  1x1x4096 | input padding 0  output padding 0
    ReLU :  1x1x4096  ->  1x1x4096 | input padding 0  output padding 0
    linear :  1x1x4096  ->  1x1x1000 | input padding 0  output padding 0
    Softmax :  1x1x1000  ->  1x1x1000 | input padding 0  output padding 0

    Finished pre-processing.
    Saving model file: 'VGG16_ImageNet_Caffe.ellmodel'

This will take a few minutes because the model file is pretty large, about 150 mb.

    VGG16_ImageNet_Caffe.ellmodel
    VGG16_ImageNet_Caffe_config.json

The config file simply contains information about the inputs and outputs for this model.

### Testing CNTK Model

Now let's test that this model works on your PC.  For this you will need to copy the
`cntkVgg16ImageNetLabels.txt` from this tutorial folder:

    cp ~/git/ELL/docs/Importing-new-models/cntkVgg16ImageNetLabels.txt .

 Then to build an ELL project that will run on your PC:

    python "~/git/ELL/wrap.py VGG16_ImageNet_Caffe_config.json cntkVgg16ImageNetLabels.txt VGG16_ImageNet_Caffe.ellmodel -target host
    cd host
    mkdir build
    cd build

Run CMake:

|=
| Linux | cmake ..
|-
| Windows with Visual Studio 2015 | cmake -G "Visual Studio 14 2015 Win64" ..
|-
| Windows with Visual Studio 2017 | cmake -G "Visual Studio 15 2017 Win64" ..
|=

Then to build it:

    cmake --build . --config Release
    cd ..
    cp /y ~/git/ELL/tools/utilities/pythonlibs/demo* .

And finally to run the test on video input:

    python demo.py VGG16_ImageNet_Caffe_config.json cntkVgg16ImageNetLabels.txt

And to test with a static image:

    python demo.py VGG16_ImageNet_Caffe_config.json cntkVgg16ImageNetLabels.txt --image coffeemug.jpg

### Summary

Notice that the steps for testing an ELL model are virtually identical.  Once the model
is converted to .ellmodel format it doesn't matter any more whether it came from Darknet
or CNTK.  The only difference is the label file where the labels can be in a different order
because of how the model was trained.