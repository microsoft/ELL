
## Microsoft Cognitive Toolkit

To use the CNTK you will need to install the [CNTK python packages](https://docs.microsoft.com/en-us/cognitive-toolkit/setup-linux-python).
From your conda environment it will be something like this, but you need to find
the version that matches your Python environment:

    pip install https://cntk.ai/PythonWheel/CPU-Only/cntk-2.0-cp36-cp36m-win_amd64.whl

We have provided a Python script that can load CNTK models.
See the *cntk_to_ell* Python module (found in tools/importers/cntk)

After building ELL and the ELL Python Language Bindings you will find the following folder in your build output.
So from your conda environment go into that directory:

    cd build/tutorials/vision/gettingStarted

Then download the CNTK ImageNet reference model as follows:

    curl -O https://www.cntk.ai/Models/Caffe_Converted/VGG16_ImageNet_Caffe.model

If your conda environment doesn't have curl just run this:

    conda install curl

## Class Labels

You will notice we have already provided the following file which contains a list of class labels that correspond 
to the model prediction output.

    cntkVgg16ImageNetLabels.txt

In this file you will see all sorts of fun classes from dog breeds to birds, fire truck, school bus, and many more.
It has 1000 classes it can predict.


## Reference Implementation

Ok, so now you can run the following Python script to load up this model and test it out with the ELL Reference 
Implementation. This will run on your PC (not on the Raspberry Pi yet), just to test if everything is working:

    python cntkDemo.py

Note: if you have more than one video camera on your PC and you want to switch to a different one you can change 
the camera index by passing a command line argument to the script as follows:

    python cntkDemo.py 1

Note: if you don't have a video camera, you can run the model on an image file by specifying the filename in place 
of the camera index. The script should handle any image format readable by OpenCV (currently, Windows bitmap (.bmp, .dib), 
JPEG (.jpeg, .jpg, .jpe), JPEG 2000 (.jp2), PNG (.png), Sun raster (.sr, .ras), and TIFF (.tiff, .tif)).

    python cntkDemo.py impala.jpg

You will see a bunch of output like this while it loads up the model:

    Convolution :  226x226x3  ->  224x224x64 | padding  1
    ReLU :  224x224x64  ->  226x226x64 | padding  0
    Convolution :  226x226x64  ->  224x224x64 | padding  1
    ReLU :  224x224x64  ->  224x224x64 | padding  0
    Pooling :  224x224x64  ->  114x114x64 | padding  0
    Convolution :  114x114x64  ->  112x112x128 | padding  1
    ReLU :  112x112x128  ->  114x114x128 | padding  0
    Convolution :  114x114x128  ->  112x112x128 | padding  1
    ReLU :  112x112x128  ->  112x112x128 | padding  0
    Pooling :  112x112x128  ->  58x58x128 | padding  0
    Convolution :  58x58x128  ->  56x56x256 | padding  1
    ReLU :  56x56x256  ->  58x58x256 | padding  0
    Convolution :  58x58x256  ->  56x56x256 | padding  1
    ReLU :  56x56x256  ->  58x58x256 | padding  0
    Convolution :  58x58x256  ->  56x56x256 | padding  1
    ReLU :  56x56x256  ->  56x56x256 | padding  0
    Pooling :  56x56x256  ->  30x30x256 | padding  0
    Convolution :  30x30x256  ->  28x28x512 | padding  1
    ReLU :  28x28x512  ->  30x30x512 | padding  0
    Convolution :  30x30x512  ->  28x28x512 | padding  1
    ReLU :  28x28x512  ->  30x30x512 | padding  0
    Convolution :  30x30x512  ->  28x28x512 | padding  1
    ReLU :  28x28x512  ->  28x28x512 | padding  0
    Pooling :  28x28x512  ->  16x16x512 | padding  0
    Convolution :  16x16x512  ->  14x14x512 | padding  1
    ReLU :  14x14x512  ->  16x16x512 | padding  0
    Convolution :  16x16x512  ->  14x14x512 | padding  1
    ReLU :  14x14x512  ->  16x16x512 | padding  0
    Convolution :  16x16x512  ->  14x14x512 | padding  1
    ReLU :  14x14x512  ->  14x14x512 | padding  0
    Pooling :  14x14x512  ->  7x7x512 | padding  0
    linear :  7x7x512  ->  1x1x4096 | padding  0
    ReLU :  1x1x4096  ->  1x1x4096 | padding  0
    linear :  1x1x4096  ->  1x1x4096 | padding  0
    ReLU :  1x1x4096  ->  1x1x4096 | padding  0
    linear :  1x1x4096  ->  1x1x1000 | padding  0
    Softmax :  1x1x1000  ->  1x1x1000 | padding  0

    
Wait for a few seconds (depending on your computer performance) then you will see the video with image
classification labels at the top, like this:

![screenshot](Screenshot.png)

## Compilation

Once you've tested that the imported model works, you can now proceed with [compiling the model and running it on different devices](compiling.md).

    

