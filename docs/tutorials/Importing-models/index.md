---
layout: default
title: Importing models
permalink: /tutorials/Importing-models/
---

# Importing models

*by Byron Changuion*

The [ELL gallery](/ELL/gallery/) includes different pre-trained ELL models for you to download and use. However, you may also want to train your own models. In this tutorial, we will import Neural Network models that were trained with the [Microsoft Cognitive Toolkit (CNTK)](https://www.microsoft.com/en-us/cognitive-toolkit/) or with [Darknet](https://pjreddie.com/darknet/). 

---

#### Materials

* Laptop or desktop computer

#### Prerequisites

* Install ELL on your computer ([Windows](https://github.com/Microsoft/ELL/blob/master/INSTALL-Windows.md), [Ubuntu Linux](https://github.com/Microsoft/ELL/blob/master/INSTALL-Ubuntu.md), [macOS](https://github.com/Microsoft/ELL/blob/master/INSTALL-Mac.md)).
* To import a model from the CNTK file format, you must install the [CNTK python packages](https://docs.microsoft.com/en-us/cognitive-toolkit/setup-cntk-on-your-machine).

## Importing models

If you followed the setup instructions, you should have an environment named `py36`. Open a terminal window and activate your Anaconda environment. 

```shell
[Linux/macOS] source activate py36
[Windows] activate py36
```

We recommend using Curl to download files. If Curl isn't installed in your Anaconda environment, install it as follows. 

```shell
conda install curl
```

Choose whether you want to import a model from the CNTK file format or the Darknet file format and proceed to the relevant section below.

## Importing a model from the Microsoft Cognitive Toolkit (CNTK)

A Neural Network model in the CNTK file format can be converted into an ELL model using the *cntk_import.py* Python script, found in the `ELL/tools/importers/cntk` directory. As an example, we will import one of the models available on the [CNTK website](https://www.microsoft.com/en-us/cognitive-toolkit/). First, download the model to your computer.

```shell
curl --location -o model.cntk https://www.cntk.ai/Models/Caffe_Converted/VGG16_ImageNet_Caffe.model
```

Next, run the importer script, making sure to provide the CNTK model as a command line parameter.

```shell
python "~/ELL/tools/importers/cntk/cntk_import.py" model.cntk
```

You should see some output like this as the model is being imported. 

```
Loading...
Selected CPU as the process wide default device.

Finished loading.
Pre-processing...
Will not process ReduceElements - skipping this layer as irrelevant.
Will not process ClassificationError - skipping this layer as irrelevant.
Will not process ReduceElements - skipping this layer as irrelevant.
Will not process CrossEntropyWithSoftmax - skipping this layer as irrelevant.
Will not process Reshape - skipping this layer as irrelevant.
Will not process Combine - skipping this layer as irrelevant.
Minus :  224x224x3  ->  226x226x3 | input padding 0  output padding 1
Convolution(LeakyReLU) :  226x226x3  ->  224x224x16 | input padding 1  output padding 0
BatchNormalization :  224x224x16  ->  224x224x16 | input padding 0  output padding 0
MaxPooling :  224x224x16  ->  114x114x16 | input padding 0  output padding 1
Convolution(LeakyReLU) :  114x114x16  ->  112x112x64 | input padding 1  output padding 0
BatchNormalization :  112x112x64  ->  112x112x64 | input padding 0  output padding 0
MaxPooling :  112x112x64  ->  58x58x64 | input padding 0  output padding 1
Convolution(LeakyReLU) :  58x58x64  ->  56x56x64 | input padding 1  output padding 0
BatchNormalization :  56x56x64  ->  56x56x64 | input padding 0  output padding 0
MaxPooling :  56x56x64  ->  30x30x64 | input padding 0  output padding 1
Convolution(LeakyReLU) :  30x30x64  ->  28x28x128 | input padding 1  output padding 0
BatchNormalization :  28x28x128  ->  28x28x128 | input padding 0  output padding 0
MaxPooling :  28x28x128  ->  16x16x128 | input padding 0  output padding 1
Convolution(LeakyReLU) :  16x16x128  ->  14x14x256 | input padding 1  output padding 0
BatchNormalization :  14x14x256  ->  14x14x256 | input padding 0  output padding 0
MaxPooling :  14x14x256  ->  9x9x256 | input padding 0  output padding 1
Convolution(LeakyReLU) :  9x9x256  ->  7x7x512 | input padding 1  output padding 0
BatchNormalization :  7x7x512  ->  7x7x512 | input padding 0  output padding 0
MaxPooling :  7x7x512  ->  6x6x512 | input padding 0  output padding 1
Convolution(LeakyReLU) :  6x6x512  ->  4x4x1024 | input padding 1  output padding 0
BatchNormalization :  4x4x1024  ->  4x4x1024 | input padding 0  output padding 0
Convolution :  4x4x1024  ->  4x4x1000 | input padding 0  output padding 0
AveragePooling :  4x4x1000  ->  1x1x1000 | input padding 0  output padding 0
Softmax :  1x1x1000  ->  1x1x1 | input padding 0  output padding 0

Finished pre-processing.
Saving model file: 'model.ell'
```

When the script finishes running, a new file named `model.ell` will appear in your directory. This ELL model file is a JSON file that contains both the model configuration and the model weights. 

In addition to the model, also download the categories file, which contains the names of the categories predicted by the model, and which we require below.

```shell
curl --location -o categories.txt https://raw.githubusercontent.com/Microsoft/ELL-models/master/models/ILSVRC2012/ILSVRC2012_labels.txt
```

## Importing a model from Darknet

A Neural Network model in the Darknet file format can be converted into an ELL model using the *darknet_import.py* Python script, found in the `ELL/tools/importers/darknet` directory. As an example, we will import the Darknet reference model, available from the [Darknet website](https://pjreddie.com/darknet/). First, download the model to your computer.

```shell
curl --location -o model.cfg https://raw.githubusercontent.com/pjreddie/darknet/master/cfg/darknet.cfg
curl --location -o model.weights https://pjreddie.com/media/files/darknet.weights
```

Next, run the importer script, making sure to provide the model configuration file (.cfg) and model weights files (.weights) as command line parameters.

```shell
python "~/ELL/tools/importers/darknet/darknet_import.py" model.cfg model.weights
```

You should see some output like this as the model is being imported. 

```
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
Saving model file: 'model.ell'
```

When the script finishes running, a new file named `model.ell` will appear in your directory. This ELL model file is a JSON file that contains both the model configuration and the model weights. 

In addition to the model, also download the categories file, which contains the names of the categories predicted by the model, and which we require below.

```shell
curl --location -o categories.txt https://raw.githubusercontent.com/pjreddie/darknet/master/data/imagenet.shortnames.list
```

Note that this categories file is different from the one used by the CNTK model above. 

## Using the model

Once the model is in the ELL format, it no longer matters whether it came from CNTK or Darknet. To invoke the model from Python, follow the steps in [Getting started with image classification on the Raspberry Pi](/ELL/tutorials/Getting-started-with-image-classification-on-the-Raspberry-Pi/), but replace the model suggested in that tutorial with the model you just imported. Alternatively, to invoke the model from C++, follow the steps in [Getting started with image classification on the Raspberry Pi in C++](/ELL/tutorials/Getting-started-with-image-classification-in-cpp/). 

## Troubleshooting

ELL currently supports a subset of the layer types and activation functions that can appear in CNTK and Darknet models. If you try to import a model that contains an unsupported layer type, the importer will fail. With time, ELL will support more layer types. In the meantime, try changing the model to use the supported layer types. 
