# Tutorial for getting started with computer vision in ELL

## Prerequisites
In order to use one of these importers, be sure to first install the ELL python bindings.
- If you do not already have the ELL python bindings installed, follow the instructions in [Interfaces](../../../interfaces/README.MD) to build them.

The Python tutorials will reference this module and the necessary importers (mentioned below) by relative path.

### Downloading a pre-trained model
The easiest way to get started is to import a pre-trained image classifier. ELL supports importing from:
* [CNTK](https://www.microsoft.com/en-us/cognitive-toolkit/) via the *cntk_to_ell* Python module (found in tools/importers/CNTK)
* [Darknet](https://pjreddie.com/darknet/) via the *darknet_to_ell* Python module (found in tools/importers/Darknet)

#### Pre-trained CNTK models
CNTK's [model gallery](https://www.microsoft.com/en-us/cognitive-toolkit/features/model-gallery/) contains two VGG models trained on the 1000-class [ImageNet Challenge](http://image-net.org/challenges/LSVRC/2015/index) dataset.
You can download the VGG16 or VGG19 models from [here](https://github.com/Microsoft/CNTK/tree/master/Examples/Image/Classification/VGG) using your browser,
or run:

curl -O https://www.cntk.ai/Models/Caffe_Converted/VGG16_ImageNet_Caffe.model

curl -O https://www.cntk.ai/Models/Caffe_Converted/VGG19_ImageNet_Caffe.model

These models are fairly large but are more accurate than smaller models.

#### Pre-trained Darknet models
Darknet has several models trained on the 1000-class [ImageNet Challenge](http://image-net.org/challenges/LSVRC/2015/index) dataset. For smaller devices, we recommend the reference Darknet model, since it is fast and reasonably accurate.

Darknet's models have 2 files: a .cfg describing the composition of the model, and a .weights file which contain the learned weights. You can download one of the pre-trained Darknet models from [here](https://pjreddie.com/darknet/imagenet/) using your browser, or run:

curl -O https://raw.githubusercontent.com/pjreddie/darknet/master/cfg/darknet.cfg

curl -O https://pjreddie.com/media/files/darknet.weights

#### Class Labels
Each model should have a text file specifying a list of class labels that correspond to the model prediction output.

This tutorial app reads in the labels as a .txt file. Two files are provided as examples, one for the VGG16 CNTK model mentioned in [Pre-trained CNTK models](), and the other for the Darknet reference model mentioned in [Pre-trained Darknet models]().

## Step by Step guide to running the tutorial
todo
