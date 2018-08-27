---
layout: default
title: Importing models
permalink: /tutorials/Importing-models/
---

# Importing models

*by Chris Lovett and Byron Changuion*

The [Embedded Learning Library (ELL) gallery](/ELL/gallery/) includes different pretrained ELL models for you to download and use. However, you may also want to train your own models using other training systems. This tutorial shows you how to import neural network models that were saved in the [Microsoft Cognitive Toolkit (CNTK)](https://www.microsoft.com/en-us/cognitive-toolkit/), [Darknet](https://pjreddie.com/darknet/) or [ONNX](https://onnx.ai/) format.

---
## Before you begin
Complete the following steps before starting the tutorial.
* Install ELL on your computer ([Windows](https://github.com/Microsoft/ELL/blob/master/INSTALL-Windows.md), [Ubuntu Linux](https://github.com/Microsoft/ELL/blob/master/INSTALL-Ubuntu.md), [macOS](https://github.com/Microsoft/ELL/blob/master/INSTALL-Mac.md)).

## What you will need

* Laptop or desktop computer


## Preparing to import models

After following the setup instructions, you have an environment named **py36**. Open a terminal window and activate your Anaconda environment.

```shell
[Linux/macOS] source activate py36
[Windows] activate py36
```

**Curl** provides an easy way to download files. If Curl isn't installed in your Anaconda environment, install it as follows.

```shell
conda install curl
```

Choose to import a model from either the CNTK file format or the Darknet file format and proceed to the relevant section below.

**Note** In the commands below, replace `<ELL-root>` with the path to the location where ELL is cloned, as
described in the installation instructions for your platform.

## Importing a model from the Microsoft Cognitive Toolkit (CNTK)

You can convert a neural network model in the CNTK file format into an ELL model using the **cntk_import.py** Python script, found in the **ELL/tools/importers/cntk** directory. In this tutorial, you will import one of the models available on the [ELL gallery](https://github.com/Microsoft/ELL-models). 

Firs, to import a model from the CNTK file format, you must install the [CNTK python packages](https://docs.microsoft.com/en-us/cognitive-toolkit/setup-cntk-on-your-machine).

Next, download the model to your computer.

```shell
curl --location -o model.cntk.zip https://github.com/Microsoft/ELL-models/raw/master/models/ILSVRC2012/d_I160x160x3CMCMCMCMCMCMC1AS/d_I160x160x3CMCMCMCMCMCMC1AS.cntk.zip
unzip model.cntk.zip
mv d_I160x160x3CMCMCMCMCMCMC1AS.cntk model.cntk
```

Next, run the importer script, making sure to provide the CNTK model as a command line parameter.

```shell
python <ELL-root>/tools/importers/CNTK/cntk_import.py model.cntk
```

Output similar to this will appear as the model is being imported.

```
Loading...
Selected CPU as the process wide default device.

Finished loading.
Pre-processing...
Will not process ReduceElements- skipping this layer as irrelevant.
Will not process ClassificationError- skipping this layer as irrelevant.
Will not process ReduceElements- skipping this layer as irrelevant.
Will not process CrossEntropyWithSoftmax- skipping this layer as irrelevant.
Will not process Reshape- skipping this layer as irrelevant.
Will not process Combine- skipping this layer as irrelevant.
Minus : 160x160x3 -> 160x160x3 | input padding 0 output padding 0
BatchNormalization : 160x160x3 -> 162x162x3 | input padding 0 output padding 1
Convolution (ReLU) : 162x162x3 -> 160x160x16 | input padding 1 output padding 0
MaxPooling : 160x160x16 -> 80x80x16 | input padding 0 output padding 0
BatchNormalization : 80x80x16 -> 82x82x16 | input padding 0 output padding 1
Convolution (ReLU) : 82x82x16 -> 80x80x64 | input padding 1 output padding 0
MaxPooling : 80x80x64 -> 40x40x64 | input padding 0 output padding 0
BatchNormalization : 40x40x64 -> 42x42x64 | input padding 0 output padding 1
Convolution (ReLU) : 42x42x64 -> 40x40x64 | input padding 1 output padding 0
MaxPooling : 40x40x64 -> 20x20x64 | input padding 0 output padding 0
BatchNormalization : 20x20x64 -> 22x22x64 | input padding 0 output padding 1
Convolution (ReLU) : 22x22x64 -> 20x20x128 | input padding 1 output padding 0
MaxPooling : 20x20x128 -> 10x10x128 | input padding 0 output padding 0
BatchNormalization : 10x10x128 -> 12x12x128 | input padding 0 output padding 1
Convolution (ReLU) : 12x12x128 -> 10x10x256 | input padding 1 output padding 0
MaxPooling : 10x10x256 -> 5x5x256 | input padding 0 output padding 0
BatchNormalization : 5x5x256 -> 7x7x256 | input padding 0 output padding 1
Convolution (ReLU) : 7x7x256 -> 5x5x512 | input padding 1 output padding 0
MaxPooling : 5x5x512 -> 3x3x512 | input padding 0 output padding 0
BatchNormalization : 3x3x512 -> 5x5x512 | input padding 0 output padding 1
Convolution (ReLU) : 5x5x512 -> 3x3x1024 | input padding 1 output padding 0
BatchNormalization : 3x3x1024 -> 3x3x1024 | input padding 0 output padding 0
Convolution (ReLU) : 3x3x1024 -> 3x3x1000 | input padding 0 output padding 0
AveragePooling : 3x3x1000 -> 1x1x1000 | input padding 0 output padding 0
Softmax : 1x1x1000 -> 1x1x1 | input padding 0 output padding 0

Finished pre-processing.
Saving model file: 'model.ell'
```

After the script finishes, a new file named **model.ell** appears in the directory. This ELL model file is a JSON file that contains both the model configuration and the model weights.

In addition to the model, download the categories file. The categories file contains the names of the categories predicted by the model, which you need later.

```shell
curl --location -o categories.txt https://github.com/Microsoft/ELL-models/raw/master/models/ILSVRC2012/categories.txt
```

## Importing a model from Darknet

You can convert a neural network model in the Darknet file format into an ELL model using the **darknet_import.py** Python script, found in the **<ELL-root>/tools/importers/darknet** directory. In this tutorial, you will import the Darknet reference model, available from the [Darknet website](https://pjreddie.com/darknet/). 

First, download the model to your computer.

```shell
curl --location -o model.cfg https://raw.githubusercontent.com/pjreddie/darknet/master/cfg/darknet.cfg
curl --location -o model.weights https://pjreddie.com/media/files/darknet.weights
```

Next, run the importer script, making sure to provide the model configuration file (.cfg) and model weights files (.weights) as command line parameters.

```shell
python <ELL-root>/tools/importers/darknet/darknet_import.py model.cfg model.weights
```

Output similar to the following appears as the model is being imported.

```
Convolution :  224x224x3  ->  224x224x16 | input padding 1  output padding 0
MaxPooling :  224x224x16  ->  112x112x16 | input padding 0  output padding 1
Convolution :  112x112x16  ->  112x112x32 | input padding 1  output padding 0
MaxPooling :  112x112x32  ->  56x56x32 | input padding 0  output padding 1
Convolution :  56x56x32  ->  56x56x64 | input padding 1  output padding 0
MaxPooling :  56x56x64  ->  28x28x64 | input padding 0  output padding 1
Convolution :  28x28x64  ->  28x28x128 | input padding 1  output padding 0
MaxPooling :  28x28x128  ->  14x14x128 | input padding 0  output padding 1
Convolution :  14x14x128  ->  14x14x256 | input padding 1  output padding 0
MaxPooling :  14x14x256  ->  7x7x256 | input padding 0  output padding 1
Convolution :  7x7x256  ->  7x7x512 | input padding 1  output padding 1
MaxPooling :  7x7x512  ->  4x4x512 | input padding 1  output padding 1
Convolution :  4x4x512  ->  4x4x1024 | input padding 1  output padding 0
Convolution :  4x4x1024  ->  4x4x1000 | input padding 0  output padding 0
AveragePooling :  4x4x1000  ->  1x1x1000 | input padding 0  output padding 0
Softmax :  1x1x1000  ->  1x1x1000 | input padding 0  output padding 0
Saving model file: 'model.ell'
```

After the script finishes, a new file named **model.ell** appear in the directory. This ELL model file is a JSON file that contains both the model configuration and the model weights.

In addition to the model, download the categories file. The categories file contains the names of the categories predicted by the model, which you'll need later.

```shell
curl --location -o categories.txt https://raw.githubusercontent.com/pjreddie/darknet/master/data/imagenet.shortnames.list
```

**Note** This categories file is different from the one used by the CNTK model, above.

## Importing a model from Open Neural Network

You can convert a neural network model in the ONNX ([Open Neural Network Exchange](https://onnx.ai/)) file format into an ELL model using the **onnx_import.py** Python script found in the **ELL/tools/importers/onnx** directory. In this tutorial, you will first export a pre-trained model from [PyTorch](https://pytorch.org/) to [ONNX](https://onnx.ai/) format, then you'll import the ONNX model into ELL. 

First, make sure you have [PyTorch](https://pytorch.org/) and [torchvision](https://pytorch.org/) installed.

Next, download the pre-trained model definition using `curl` as follows:

```shell
curl -O --location https://github.com/pytorch/vision/raw/master/torchvision/models/vgg.py
```

Then run the following script.  This script uses the PyTorch library to download a pre-trained **vgg11** model and export it to the ONNX format:

```python
import torch.nn as nn
import torch
import math
import torch.onnx

import vgg

output_filename = "model.onnx"

print("Loading {}".format(vgg.model_urls['vgg11']))

torch_model = vgg.vgg11(pretrained=True) 

# in order to export the model, torch needs to run one forward inference
# to compute the model graph.  We can do this with a random input.
x = torch.randn(1, 3, 224, 224, requires_grad=True)

# Export the model
print("Exporting {}".format(output_filename))
torch_out = torch.onnx._export(torch_model,         # model being run
                               x,                   # model input 
                               output_filename,     # where to save the model 
                               export_params=True)  # store the trained weights
```

Next, run the importer script, making sure to provide the ONNX model as a command line parameter.

```shell
python <ELL-root>/tools/importers/ONNX/onnx_import.py model.onnx
```

Output similar to this will appear as the model is being imported.

```shell
Loading...

Finished loading.
Graph producer: pytorch version 0.3
Graph total len: 23
Processing the following importer nodes in order:
Input 0: [] -> ['0'], attributes {}
    input_shape []
    output_shape [((3, 224, 224), 'channel_row_column')]
    padding {'size': 0, 'scheme': 0}
    output_padding {'size': 1, 'scheme': 0}
    weights dict_keys([])

Convolution 23: ['0'] -> ['23'], attributes {'size': 3, 'stride': 1, 'padding': 1, 'dilation': 1}
    input_shape [((3, 224, 224), 'channel_row_column')]
    output_shape [((64, 224, 224), 'channel_row_column')]
    padding {'size': 1, 'scheme': 0}
    output_padding {'size': 0, 'scheme': 0}
    weights dict_keys(['weights', 'bias'])

ReLU 24: ['23'] -> ['24'], attributes {'activation': 0}
    input_shape [((64, 224, 224), 'channel_row_column')]
    output_shape [((64, 224, 224), 'channel_row_column')]
    padding {'size': 0, 'scheme': 0}
    output_padding {'size': 0, 'scheme': 0}
    weights dict_keys([])
.
.
.
FullyConnected 53: ['50'] -> ['53'], attributes {}
    input_shape [((4096, 1, 1), 'channel_row_column')]
    output_shape [((1000, 1, 1), 'channel_row_column')]
    padding {'size': 0, 'scheme': 0}
    output_padding {'size': 0, 'scheme': 0}
    weights dict_keys(['weights', 'bias'])

Converting intermediate importer nodes to ELL nodes....
Importing node 0
ImporterNode 0 -> intermediate ELL nodes ['1000']
ImporterNode 0 -> intermediate ELL nodes ['1000', '1001']
ImporterNode 0 -> intermediate ELL nodes ['1000', '1001', '1002']
ImporterNode 0 -> intermediate ELL nodes ['1000', '1001', '1002', '1003']
Importing node 23
ImporterNode 23 -> intermediate ELL nodes ['1004']
ImporterNode 23 -> intermediate ELL nodes ['1004', '1005']
.
.
.
ImporterNode 53 -> intermediate ELL nodes ['1012']

Finished pre-processing.
Saving model file: 'model.ell'
```

After the script finishes, a new file named **model.ell** will be saved to your current directory. 

## Next steps

After the model is in the ELL format, the steps to use it are the same, whether it came from CNTK, Darknet or ONNX. To invoke the model from Python, follow the steps in [Getting started with image classification on the Raspberry Pi](/ELL/tutorials/Getting-started-with-image-classification-on-the-Raspberry-Pi/), but replace the model suggested in that tutorial with the model you just imported. Alternatively, to invoke the model from C++, follow the steps in [Getting started with image classification on the Raspberry Pi in C++](/ELL/tutorials/Getting-started-with-image-classification-in-cpp/).

You may need to use different [categories.txt](https://github.com/Microsoft/ELL-models/blob/master/models/ILSVRC2012/categories.txt) files though, because the order of the expected categories depends on how the model was  trained.

**Note** Using a converted Darknet model from pjreddie will require OpenCV input reordered from BGR to RGB. [tutorial_helpers.py](/ELL/tutorials/shared/tutorial_helpers.py) and [tutorialHelpers.h](/ELL/tutorials/Getting-started-with-image-classification-in-cpp/tutorialHelpers.h) provide helper functions for Python and C++, respectively, that take an argument to reorder the input image from BGR to RGB.

## Troubleshooting

**Failed importer** If you try to import a model that contains an unsupported layer type, the importer will fail. ELL currently supports only a subset of the layer types and activation functions that can appear in CNTK, Darknet and ONNX models. If you cannot work around this by changing your model please file an [issue on github](https://github.com/Microsoft/ELL/issues).
