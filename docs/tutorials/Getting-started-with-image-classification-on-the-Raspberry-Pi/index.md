---
layout: default
title: Getting started with image classification on the Raspberry Pi
permalink: /tutorials/Getting-started-with-image-classification-on-the-Raspberry-Pi/
---

# Getting started with image classification on Raspberry Pi

*by Chris Lovett, Byron Changuion, Ofer Dekel, and Kern Handa*

This tutorial guides you through the process of getting started with image classification on your Raspberry Pi device. You'll download a pretrained image classification model from the [Embedded Learning Library (ELL) gallery](/ELL/gallery/) to a laptop or desktop computer. You'll compile the model and wrap it in a Python module. Finally, you'll write a simple Python script that captures images from the Raspberry Pi's camera and classifies them.

---

![screenshot](/ELL/tutorials/Getting-started-with-image-classification-on-the-Raspberry-Pi/Screenshot.jpg)

## Before you begin

Complete the following steps before starting the tutorial.
* Install ELL on your computer ([Windows](https://github.com/Microsoft/ELL/blob/master/INSTALL-Windows.md), [Ubuntu Linux](https://github.com/Microsoft/ELL/blob/master/INSTALL-Ubuntu.md), [macOS](https://github.com/Microsoft/ELL/blob/master/INSTALL-Mac.md)).
* Follow the instructions for [setting up your Raspberry Pi](/ELL/tutorials/Raspberry-Pi-setup).

## What you will need

* Laptop or desktop computer
* Raspberry Pi 3
* Raspberry Pi camera or USB webcam
* Optional: active cooling attachment (see the [tutorial on cooling your Pi device](/ELL/tutorials/Active-cooling-your-Raspberry-Pi-3/))


## Activate your environment and create a tutorial directory

After following the setup instructions, you'll have an Anaconda environment named **py36**. Open a terminal window and activate your Anaconda environment.

```shell
[Linux/macOS] source activate py36
[Windows] activate py36
```

Create a directory for this tutorial anywhere on your computer and `cd` into it.

## Download a pretrained model

Download this [compressed ELL model file](https://github.com/Microsoft/ELL-models/raw/master/models/ILSVRC2012/d_I224x224x3CMCMCMCMCMCMC1AS/d_I224x224x3CMCMCMCMCMCMC1AS.ell.zip) into your directory. The model file contains a pretrained Deep Neural Network for image classification and is one of the models available from the [ELL gallery](/ELL/gallery). (The file's long name indicates the Neural Network's architecture.) Save the file locally as **model.ell.zip**.

```shell
curl --location -o model.ell.zip https://github.com/Microsoft/ELL-models/raw/master/models/ILSVRC2012/d_I224x224x3CMCMCMCMCMCMC1AS/d_I224x224x3CMCMCMCMCMCMC1AS.ell.zip
```

Unzip the compressed file.

**Note** On Windows computers, the unzip utility is distributed as part of Git. For example, in `\Program Files\Git\usr\bin`. On Linux computers, you can install unzip using the **apt-get install unzip** command.

```shell
unzip model.ell.zip
```

Rename the **d_I224x224x3CMCMCMCMCMCMC1AS.ell** model file to **model.ell**.

```shell
[Linux/macOS] mv d_I224x224x3CMCMCMCMCMCMC1AS.ell model.ell
[Windows] ren d_I224x224x3CMCMCMCMCMCMC1AS.ell model.ell
```

Next, download the **categories.txt** file from [here](https://github.com/Microsoft/ELL-models/raw/master/models/ILSVRC2012/categories.txt) and save it in your directory.

```shell
curl --location -o categories.txt https://github.com/Microsoft/ELL-models/raw/master/models/ILSVRC2012/categories.txt
```

This file contains the names of the 1,000 categories that the model is trained to recognize. For example, if the model recognizes an object of category 504, read line 504 of the categories.txt file to find out that the name of the recognized category is "coffee mug."

There should now be a **model.ell** file and a **categories.txt** file in your directory.

## Compile and run the model on your laptop or desktop computer

Before deploying the model to the Raspberry Pi device, practice deploying it to the laptop or desktop computer. Deploying an ELL model requires two steps. First, you'll run the **wrap** tool, which compiles `model.ell` into machine code and generates a CMake project that builds a Python wrapper for it. Then, you'll call **CMake** to build that Python wrapper.

Run **wrap** as follows, replacing `<ELL-root>` with the path to the ELL root directory (the directory where you cloned the ELL repository).

```shell
python <ELL-root>/tools/wrap/wrap.py --model_file model.ell --language python --target host
```

Here **wrap** uses the command line option of `--target host`, which tells it to generate machine code for execution on the laptop or desktop computer, rather than machine code for the Raspberry Pi device. This results in the following output.

```
compiling model...
generating python interfaces for model in host
running opt...
running llc...
success, now you can build the 'host' folder
```

The **wrap** tool creates a CMake project in a new directory named **host**. Create a **build** directory inside the **host** directory and change to that directory.

```shell
cd host
mkdir build
cd build
```

Finally, use CMake to finish building the Python wrapper.

```shell
[Linux/macOS] cmake .. -DCMAKE_BUILD_TYPE=Release && make && cd ../..
[Windows] cmake -G "Visual Studio 16 2019" -A x64 .. && cmake --build . --config release && cd ..\..
```

This creates a Python module named **model**. This module provides functions that report the shapes of the model's input and output as well as the **predict** function, which invokes the model.

## Invoke the model on your computer

The next step is to create a Python script that loads the model, sends images to it, and interprets the model's output. (You can view the full script [here](/ELL/tutorials/Getting-started-with-image-classification-on-the-Raspberry-Pi/call_model.py).) Before creating this script, copy over some Python helper code.

```shell
[Linux/macOS] cp <ELL-root>/docs/tutorials/shared/tutorial_helpers.py .
[Windows] copy <ELL-root>\docs\tutorials\shared\tutorial_helpers.py .
```

Create an empty text file named **call_model.py** and copy in the code snippets below. First, import a few dependencies.

```python
import cv2
import numpy as np
```

Import the helper code that you copied over.

**Note** Do this before importing the model because it helps find the requisite compiled model files.

```python
import tutorial_helpers as helpers
```

Next, load the Python module representing the wrapped ELL model.

```python
import model
```

Create a model wrapper to interact with the model
```python
model_wrapper = model.ModelWrapper()
```

Print the model's input and output shapes.

```python
input_shape = model_wrapper.GetInputShape()
output_shape = model_wrapper.GetOutputShape()

print("Model input shape: [{0.rows}, {0.columns}, {0.channels}]".format(
    input_shape))
print("Model output shape: [{0.rows}, {0.columns}, {0.channels}]".format(
    output_shape))
```

The output should be similar to this.

```
Model input shape: [224, 224, 3]
Model output shape: [1, 1, 1000]
```

Models may need specific preprocessing for particular datasets, get the preprocessing metadata for the model for use later.
```python
preprocessing_metadata = helpers.get_image_preprocessing_metadata(model_wrapper)
```

The input to the model is a 3-channel image of height 224 and width 224, which can also be represented as an array of size 224 * 224 * 3 = 150528. The shape of the output is 1 x 1 x 1000, which can be represented as an array of 1,000 elements. The model's **predict** method will receive the input array and return the output array.

Choose an image file to send to the model. For example, use this [coffee mug image](/ELL/tutorials/shared/coffeemug.jpg). Use OpenCV to read the image.

```python
sample_image = cv2.imread("coffeemug.jpg")
```

The image stored in the `sample_image` variable cannot be sent to the model as-is, because the model takes its input as an array of **float** values. Moreover, the model expects the input image to have a certain shape (in this case, 224 x 224 x 3) and a specific ordering of the color channels (in this case, Blue-Green-Red). Because preparing images for the model is such a common operation, there is a helper function for it named **prepare_image_for_model**.

```python
input_data = helpers.prepare_image_for_model(sample_image, input_shape.columns,
                                             input_shape.rows, preprocessing_metadata=preprocessing_metadata)
```

Wrap the input_data numpy array in a FloatVector
```python
input_data = model.FloatVector(input_data)
```

Invoke the model by calling its **Predict** method.

```python
predictions = model_wrapper.Predict(input_data)
```

The `Predict` method returns a `predictions` array with non-negative scores that sum to 1. Each element of this array corresponds to one of the 1000 image categories recognized by the model and represents the model's confidence that the image contains an object from that category. For example, recall that category 504 is **coffee mug** - the value of `predictions[504]` is the model's confidence that the image contains a coffee mug.

Print the index of the highest confidence category.

```python
prediction_index = int(np.argmax(predictions))
print("Category index: {}".format(prediction_index))
print("Confidence: {}".format(predictions[prediction_index]))
```

## Compile the model for execution on the Raspberry Pi device

You are ready to cross-compile the model for deployment on the Raspberry Pi device. First, create a new directory, which will be copied over to the Raspberry Pi. After creating the new directory, copy over the following files from the preceding directory:
* **tutorial_helpers.py**
* **categories.txt**
* **model.ell**

As before, run the **wrap** tool on your laptop or desktop computer, but this time specify the target platform as `pi3`. This tells the ELL compiler to generate machine code for the Raspberry Pi's ARM Cortex A53 processor. This step needs to be performed in the directory to be copied to the Raspberry Pi.

```shell
python <ELL-root>/tools/wrap/wrap.py --model_file model.ell --language python --target pi3
```

The **wrap** tool creates a new directory named **pi3**, which contains a CMake project that can be used to build the desired Python module. Don't build this project yet - this time, you need to build this project on the Raspberry Pi.

**Note**: To speed up the transfer of files to the Raspberry Pi,  delete the model.ell file first before copying the folder.

```shell
[Linux/macOS] rm -f model.ell
[Windows] del model.ell
```

Now, there's a **pi3** directory that contains a CMake project that builds the Python wrapper and some helpful Python utilities. This directory should be inside a directory that also contains **tutorial_helpers.py** and **categories.txt**.

## Write code to invoke the model on the Raspberry Pi device

Next, you'll write a Python script that invokes the model on the Raspberry Pi device. The script loads the Python wrapper that you created, reads images from the camera, passes these images to the model, and displays the classification results. If you just want the full script, copy it from [here](/ELL/tutorials/Getting-started-with-image-classification-on-the-Raspberry-Pi/tutorial.py). Otherwise, create an empty text file named **tutorial.py** and copy in the code snippets below.

First, import a few dependencies.

```python
import sys
import os
import time
import cv2
```

Import the helper code that you copied over.

**Note** The helper code helps find the compiled model files, so make sure to import it before importing the model.

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

The argument **0** in the function call above selects the default camera. If you have more than one camera connected to your Pi device, choose which camera to use by changing this argument. Next, read the category names from the **categories.txt** file.

```python
    with open("categories.txt", "r") as categories_file:
        categories = categories_file.read().splitlines()
```

Create a model wrapper to interact with the model
```python
    model_wrapper = model.ModelWrapper()
```

The model expects its input in a certain shape. Get this shape and store it for use later on.

```python
    input_shape = model_wrapper.GetInputShape()
```

Models may need specific preprocessing for particular datasets, get the preprocessing metadata for the model for use later.
```python
    preprocessing_metadata = helpers.get_image_preprocessing_metadata(model_wrapper)
```

Next, set up a loop that keeps going until OpenCV indicates it is done, which is when the user presses any key. At the start of each iteration, read an image from the camera.

```python
    while (cv2.waitKey(1) & 0xFF) == 0xFF:
        image = get_image_from_camera(camera)
```

As mentioned above, the image stored in the `image` variable cannot be sent to the model as-is. It must be processed using the **prepare_image_for_model** function, from the **helpers** module.

```python
        input_data = helpers.prepare_image_for_model(
            image, input_shape.columns, input_shape.rows, preprocessing_metadata=preprocessing_metadata)
```

Wrap the input_data numpy array in a FloatVector
```python
        input_data = model.FloatVector(input_data)
```

With the processed image input handy, call the **Predict** method to invoke the model.

```python
        predictions = model_wrapper.Predict(input_data)
```

As before, the **Predict** method fills the **predictions** array with the model output. Each element of this array corresponds to one of the 1000 image classes recognized by the model. Extract the top 5 predicted categories by calling the helper function `get_top_n`.

```python
        top_5 = helpers.get_top_n(predictions, 5)
```

**top_5** is an array of tuples, where the first element in each tuple is the category index and the second element is the probability of that category. Match the category indices in `top_5` with the corresponding names in `categories` and construct a string that represents the model output.

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

## Build the Python module on the Raspberry Pi

For this step, you'll be working with your the Raspberry Pi device. If your Pi device is accessible over the network, copy the directory using the Unix `scp` tool or the Windows [WinSCP](https://winscp.net/eng/index.php) tool.

Log in to your Raspberry Pi, find the directory you just copied from your computer, and build the python module that wraps the ELL model.

```shell
cd pi3
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make
cd ../..
```

You just created a Python module named **model**, which includes functions that report the model's input and output dimensions and makes it possible to pass images to the model for classification.

## Classify live video on the Raspberry Pi device

After following the [Raspberry Pi setup instructions](/ELL/tutorials/Raspberry-Pi-setup), you'll have an Anaconda environment named **py36.** Open a terminal window, and activate your Anaconda environment, and run the script that you wrote above.

```shell
source activate py34
python tutorial.py
```

If you have a camera and display connected to your Pi, you should see a window similar to the screenshot at the beginning of this tutorial. Point your camera at different objects and see how the model classifies them. Look at the **categories.txt** file to see which categories the model is trained to recognize and try to show those objects to the model. For quick experimentation, point the camera to your computer screen and have your computer display images of different objects. For example, experiment with different dog breeds and other types of animals.


## Next steps

The [ELL gallery](/ELL/gallery/) offers different models for image classification. Some are slow and accurate, while others are faster and less accurate. Different models can even lead to different power draw on the Raspberry Pi device. Repeat the steps above with different models.

To learn more about how the **wrap** tool works, read the [wrap documentation](https://github.com/Microsoft/ELL/blob/master/tools/wrap/README.md).

## Troubleshooting

**Image processing is slow.** You might notice that the average time it takes the model to process each image is slower than indicated in  the [ELL gallery](/ELL/gallery/). This is because the Python wrapper adds some overhead that slows the model down. To run the model at its top speed, follow the [C++ version of this tutorial](/ELL/tutorials/Getting-started-with-image-classification-in-cpp).

**Look for more troubleshooting tips** at the end of the [Raspberry Pi setup instructions](/ELL/tutorials/Raspberry-Pi-setup).
