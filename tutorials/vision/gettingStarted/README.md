# Tutorial for getting started with computer vision in ELL

## Prerequisites
In order to use one of these importers, be sure to first install the ELL python bindings.
- If you do not already have the ELL python bindings installed, follow the instructions in [Interfaces](../../../interfaces/README.MD) to build them.

The Python tutorials will reference this module and the necessary importers (mentioned below) by relative path.

### Downloading a pre-trained model
The easiest way to get started is to import a pre-trained image classifier. ELL supports importing from:
* [CNTK](https://www.microsoft.com/en-us/cognitive-toolkit/) via the *cntk_to_ell* Python module (found in tools/importers/CNTK)
* [Darknet](https://pjreddie.com/darknet/) via the *darknet_to_ell* Python module (found in tools/importers/Darknet)

For this tutorial, start by downloading the Darknet reference model from [here](https://pjreddie.com/darknet/imagenet/#reference), using your browser.
Or you can download directly using the following commandline:

    curl -O https://raw.githubusercontent.com/pjreddie/darknet/master/cfg/darknet.cfg
    curl -O https://pjreddie.com/media/files/darknet.weights

A list of other useful models, from both CNTK and Darknet, can be found in the [PretrainedModels](./PretrainedModels.md) section.

#### Class Labels
Each model should have a text file specifying a list of class labels that correspond to the model prediction output.

This tutorial app reads in the labels as a .txt file. Two files are provided as examples, one for the VGG16 CNTK model mentioned in [Pre-trained CNTK models](), and the other for the Darknet reference model mentioned in [Pre-trained Darknet models]().

## Step by Step guide to running the tutorial
todo
