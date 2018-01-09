---
layout: default
title: Getting started with image classification on the Raspberry Pi
permalink: /tutorials/Getting-started-with-image-classification-on-the-Raspberry-Pi/
---

# Getting started with image classification on Raspberry Pi

*by Chris Lovett, Byron Changuion, Ofer Dekel, and Kern Handa*

In this tutorial, we will download a pre-trained image classification model from the [ELL gallery](/ELL/gallery/) to a laptop or desktop computer. We will then compile the model and wrap it in a Python module. Finally, we will write a simple Python script that captures images from the Raspberry Pi's camera and sends them to the Python module for classification.

---

![screenshot](/ELL/tutorials/Getting-started-with-image-classification-on-the-Raspberry-Pi/Screenshot.jpg)

#### Materials

* Laptop or desktop computer
* Raspberry Pi 3
* Raspberry Pi camera or USB webcam
* *optional* - Active cooling attachment (see our [tutorial on cooling your Pi](/ELL/tutorials/Active-cooling-your-Raspberry-Pi-3/))

#### Prerequisites

* Install ELL on your computer ([Windows](https://github.com/Microsoft/ELL/blob/master/INSTALL-Windows.md), [Ubuntu Linux](https://github.com/Microsoft/ELL/blob/master/INSTALL-Ubuntu.md), [macOS](https://github.com/Microsoft/ELL/blob/master/INSTALL-Mac.md)).
* Follow the instructions for [setting up your Raspberry Pi](/ELL/tutorials/Setting-up-your-Raspberry-Pi).

## Step 1: Activate your environment and create a tutorial directory

If you followed the setup instructions, you should have an environment named `py36`. Open a terminal window and activate your Anaconda environment.

```shell
[Linux/macOS] source activate py36
[Windows] activate py36
```

Create a directory for this tutorial anywhere on your computer and `cd` into it.

## Step 2: Download pre-trained model

Download this [compressed ELL model file](https://github.com/Microsoft/ELL-models/raw/master/models/ILSVRC2012/d_I224x224x3CMCMCMCMCMCMC1AS/d_I224x224x3CMCMCMCMCMCMC1AS.ell.zip) into the directory. The model file contains a pre-trained Deep Neural Network for image classification, and is one of the models available from the [ELL gallery](/ELL/gallery). The file's long name indicates the Neural Network's architecture, but don't worry about that for now and save it locally as `model.ell.zip`.

```shell
curl --location -o model.ell.zip https://github.com/Microsoft/ELL-models/raw/master/models/ILSVRC2012/d_I224x224x3CMCMCMCMCMCMC1AS/d_I224x224x3CMCMCMCMCMCMC1AS.ell.zip
```

Unzip the compressed file. On Windows, note that the `unzip` utility is distributed as part of Git, for example, in `\Program Files\Git\usr\bin`.

```shell
unzip model.ell.zip
```

Rename the `d_I224x224x3CMCMCMCMCMCMC1AS.ell` model file to `model.ell`:

```shell
[Linux/macOS] mv d_I224x224x3CMCMCMCMCMCMC1AS.ell model.ell
[Windows] ren d_I224x224x3CMCMCMCMCMCMC1AS.ell model.ell
```

Next, download the `categories.txt` file from [here](https://github.com/Microsoft/ELL-models/raw/master/models/ILSVRC2012/categories.txt) and save it in the directory.

```shell
curl --location -o categories.txt https://github.com/Microsoft/ELL-models/raw/master/models/ILSVRC2012/categories.txt
```

This file contains the names of the 1000 categories that the model is trained to recognize. For example, if the model recognizes an object of category 504, we can read line 504 of `categories.txt` and see that the name of the recognized category is `coffee mug`.

There should now be a `model.ell` file and a `categories.txt` file in the directory.

## Step 3: Compile and run the model on your laptop or desktop computer

Before deploying the model to the Raspberry Pi, we will practice deploying it to the laptop or desktop computer. Deploying an ELL model requires two steps. First, we run a tool named `wrap`, which both compiles `model.ell` into machine code and generates a CMake project to build a Python wrapper for it. Second, we call CMake to build the Python library.

Run `wrap` as follows. Please replace `<ELL-root>` with the path to the location where you have cloned ELL, as described in the installation instructions for your platform.

```shell
python <ELL-root>/tools/wrap/wrap.py model.ell -lang python -target host
```

Note that we gave `wrap` the command line option `-target host`, which tells it to generate machine code for execution on the laptop or desktop computer, rather than machine code for the Raspberry Pi. If all goes well, you should see the following output.

```
compiling model...
generating python interfaces for model in host
running opt...
running llc...
success, now you can build the 'host' folder
```

The `wrap` tool creates a `cmake` project in a new directory named `host`. Create a `build` directory inside the `host` directory and change to that directory

```shell
cd host
mkdir build
cd build
```

To finish creating the Python wrapper, build the `cmake` project.

```shell
[Linux/macOS] cmake .. -DCMAKE_BUILD_TYPE=Release && make && cd ../..
[Windows] cmake -G "Visual Studio 14 2015 Win64" .. && cmake --build . --config release && cd ..\..
```

We have just created a Python module named `model`. This module provides functions that report the shapes of the model's input and output, as well as the `predict` function, which invokes the model.

## Step 4: Invoke the model on your computer

The next step is to create a Python script that loads the model, sends images to it, and interprets the model's output. If you just want the full script, copy it from [here](/ELL/tutorials/Getting-started-with-image-classification-on-the-Raspberry-Pi/call_model.py) into the directory. Otherwise, create an empty text file named `call_model.py` and copy in the code snippets below.

Before writing the script that will use the compiled model, we also want to copy over some Python helper code:

```shell
[Linux/macOS] cp <ELL-root>/docs/tutorials/shared/tutorial_helpers.py .
[Windows] copy <ELL-root>\docs\tutorials\shared\tutorial_helpers.py .
```

First, import a few dependencies and add directories to the path, to allow Python to find the module that we created above.

```python
import cv2
import numpy as np
```

Import the helper code that we copied over. Note that this must precede importing of the model as it helps
find the requisite compiled model files.

```python
import tutorial_helpers as helpers
```

Next, load the Python module representing the wrapped ELL model.

```python
import model
```

Print the model's input and output shapes.

```python
input_shape = model.get_default_input_shape()
output_shape = model.get_default_output_shape()

print("Model input shape: [{0.rows}, {0.columns}, {0.channels}]".format(
    input_shape))
print("Model output shape: [{0.rows}, {0.columns}, {0.channels}]".format(
    output_shape))
```

You should see output similar to this.

```
Model input shape: [224, 224, 3]
Model output shape: [1, 1, 1000]
```

The input to the model is a 3-channel image of height 224 and width 224, which can also be represented as an array of size 224 * 224 * 3 = 150528. The shape of the output is 1 x 1 x 1000, which can be represented as an array of 1000 elements. The model's `predict` method will receive the input array and return the output array.

Choose an image file to send to the model. For example, use this [coffee mug image](/ELL/tutorials/shared/coffeemug.jpg). Use OpenCV to read the image.

```python
sample_image = cv2.imread("coffeemug.jpg")
```

The image stored in the `sample_image` variable cannot be sent to the model as-is, because the model takes its input as an array of `float` values. Moreover, the model expects the input image to have a certain shape (which, in this case, is 224 x 224 x 3) and a specific ordering of the color channels (in this case, Blue-Green-Red). Since preparing images for the model is such a common operation, we created a helper function for it named `prepare_image_for_model`.

```python
input_data = helpers.prepare_image_for_model(sample_image, input_shape.columns,
                                             input_shape.rows)
```

Finally, invoke the model by calling its `predict` method.

```python
predictions = model.predict(input_data)
```

The `predict` method returns a `predictions` array with non-negative scores that sum to 1. Each element of this array corresponds to one of the 1000 image classes recognized by the model. Print the index of the highest confidence category.

```python
prediction_index = int(np.argmax(predictions))
print("Category index: {}".format(prediction_index))
print("Confidence: {}".format(predictions[prediction_index]))
```

This code also looks up the category name in the `categories` array. For example, say that the highest confidence category is category 504. The string in `categories[504]` is the name of category 504, which happens to be `coffee mug`. The value at `predictions[504]` is the model's confidence that the image contains a coffee mug.

## Step 5: Compile the model for execution on the Raspberry Pi

We are ready to cross-compile the model for deployment on the Raspberry Pi. For this, please create a new directory. This new directory will be copied over to the Raspberry Pi. After creating the new directory, copy over the following files from the preceding directory:
* `tutorial_helpers.py`
* `categories.txt`
* `model.ell`

As before, run the `wrap` tool on your laptop or desktop computer, but this time specify the target platform as `pi3`. This tells the ELL compiler to generate machine code for the Raspberry Pi's ARM Cortex A53 processor. This step needs to be performed in the directory to be copied to the Raspberry Pi.

```shell
python <ELL-root>/tools/wrap/wrap.py model.ell -lang python -target pi3
```

The `wrap` tool creates a new directory named `pi3`, which contains a CMake project that can be used to build the desired Python module. This time, we need to build this project on the Raspberry Pi.

To speed up the transfer of files to the Raspberry Pi, you may want to delete the model.ell file first before copying the folder:

```shell
[Linux/macOS] rm -f model.ell
[Windows] del model.ell
```

At this point, you should have a `pi3` directory that contains a `cmake` project that builds the Python wrapper and some helpful Python utilities. The `pi3` directory should be inside a directory that also contains `tutorial_helpers.py` and `categories.txt`.

## Step 6: Write code to invoke the model on the Raspberry Pi

We will write a Python script that invokes the model and runs the demo on the Raspberry Pi. The script will load the Python wrapper that we created above, read images from the camera, pass these images to the model, and display the classification results. If you just want the full script, copy it from [here](/ELL/tutorials/Getting-started-with-image-classification-on-the-Raspberry-Pi/tutorial.py). Otherwise, create an empty text file named `tutorial.py` copy in the code snippets below.

First, import a few dependencies, including system utilities, OpenCV, and NumPy.

```python
import sys
import os
import time
import cv2
```

Import the helper code that we copied over. Note that this must precede importing of the model as it helps
find the requisite compiled model files.

```python
import tutorial_helpers as helpers
```

Import the Python wrapper for the compiled ELL model.

```python
import model
```

Add a function that reads an image from the camera.

```python
def get_image_from_camera(camera):
    if camera:
        ret, frame = camera.read()
        if not ret:
            raise Exception("your capture device is not returning images")
        return frame
    return None
```

Define the main entry point and use the camera as the image source.

```python
def main():
    camera = cv2.VideoCapture(0)
```

The argument `0` in the function call above selects the default camera. If you have more than one camera connected to your Pi, choose which camera to use by changing this argument. Next, read the category names from the `categories.txt` file.

```python
    with open("categories.txt", "r") as categories_file:
        categories = categories_file.read().splitlines()
```

The model expects its input in a certain shape. Get this shape and store it for use later on.

```python
    input_shape = model.get_default_input_shape()
```

Next, set up a loop that keeps going until OpenCV indicates it is done, which is when the user hits any key. At the start of each iteration, read an image from the camera.

```python
    while (cv2.waitKey(1) & 0xFF) == 0xFF:
        image = get_image_from_camera(camera)
```

As mentioned above, the image stored in the `image` variable cannot be sent to the model as-is and needs to be processed using the `prepare_image_for_model` function, from the `helpers` module.

```python
        input_data = helpers.prepare_image_for_model(
            image, input_shape.columns, input_shape.rows)
```

With the processed image input handy, call the `predict` method to invoke the model.

```python
        predictions = model.predict(input_data)
```

As before, the `predict` method fills the `predictions` array with the model output. Each element of this array corresponds to one of the 1000 image classes recognized by the model. Extract the top 5 predicted categories by calling the helper function `get_top_n`.

```python
        top_5 = helpers.get_top_n(predictions, 5)
```

`top_5` is an array of tuples, where the first element in each tuple is the category index and the second element is the probability of that category. Match the category indices in `top_5` with the corresponding names in `categories` and construct a string that represents the model output.

```python
        header_text = ", ".join(["({:.0%}) {}".format(
            element[1], categories[element[0]]) for element in top_5])
```

Use the `draw_header` helper function to display the predicted category on the Raspberry Pi's display. Also display the camera image.

```python
        helpers.draw_header(image, header_text)
        cv2.imshow("ELL model", image)
```
Finally, write the code that invokes the `main` function and runs your script.

```python
if __name__ == "__main__":
    main()
```

We are ready to move to the Raspberry Pi. If your Pi is accessible over the network, you can copy directory using the Unix `scp` tool or the Windows [WinSCP](https://winscp.net/eng/index.php) tool.

## Step 7: Build the Python module on the Raspberry Pi

Log into your Raspberry Pi, find the directory you just copied from your computer, and build the python module that wraps the ELL model.

```shell
cd pi3
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make
cd ../..
```

We just created a Python module named `model`, which includes functions that report the model's input and output dimensions and enables us to pass images to the model for classification.

## Step 8: Classify live video on the Raspberry Pi

If you followed the [Raspberry Pi Setup Instructions](/ELL/tutorials/Setting-up-your-Raspberry-Pi), you should have an anaconda environment named `py34`. Activate it and run the script that we wrote above.

```shell
source activate py34
python tutorial.py
```

If you have a camera and display connected to your Pi, you should see a window similar to the screenshot at the top of this page. Point your camera at different objects and see how the model classifies them. Look at `categories.txt` to see which categories the model is trained to recognize and try to show those objects to the model. For quick experimentation, point the camera to your computer screen and have your computer display images of different objects. For example, experiment with different dog breeds and other types of animals.

If you copied the full `tutorial.py` script from [here](/ELL/tutorials/Getting-started-with-image-classification-on-the-Raspberry-Pi/tutorial.py), you will also see the average time in milliseconds it takes the model to process each image. If you compare the displayed time with the time indicated in the [ELL gallery](/ELL/gallery/), you will notice that your model runs slower than it should. The slow down is caused by inefficiencies in the Python wrapper, and we are working to fix this problem. To run the model at its top speed, follow the [C++ version of this tutorial](/ELL/tutorials/Getting-started-with-image-classification-in-cpp).

## Next steps

The [ELL gallery](/ELL/gallery/) offers different models for image classification. Some are slow and accurate, while others are faster and less accurate. Different models can even lead to different power draw on the Raspberry Pi. Repeat the steps above with different models.

We used the `wrap` tool as a convenient way to compile the model and prepare for building its Python wrapper. To understand what `wrap` does under the hood, read the [wrap documentation](https://github.com/Microsoft/ELL/blob/master/tools/wrap/README.md).

## Troubleshooting

If you run into trouble, you can find some troubleshooting instructions at the bottom of the [Raspberry Pi Setup Instructions](/ELL/tutorials/Setting-up-your-Raspberry-Pi).