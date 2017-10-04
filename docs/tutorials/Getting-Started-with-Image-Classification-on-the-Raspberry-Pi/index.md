---
layout: default
title: Getting started with image classification on the Raspberry Pi
permalink: /tutorials/Getting-started-with-image-classification-on-the-Raspberry-Pi/
---

# Getting started with image classification on Raspberry Pi

In this tutorial, we will download a pre-trained image classifier from the [ELL gallery](/ELL/gallery/) to a laptop or desktop computer. We will then compile the classifier and wrap it in a Python module. Finally, we will write a simple Python script that captures images from the Raspberry Pi's camera and sends them to the Python module for classification.

---

![screenshot](/ELL/tutorials/Getting-Started-with-Image-Classification-on-the-Raspberry-Pi/Screenshot.png)

#### Materials

* Laptop or desktop computer
* Raspberry Pi
* Raspberry Pi camera or USB webcam
* *optional* - Active cooling attachment (see our [tutorial on cooling your Pi](/ELL/tutorials/Active-cooling-your-Raspberry-Pi-3/))

#### Prerequisites

* Install ELL on your computer ([Windows](https://github.com/Microsoft/ELL/blob/master/INSTALL-Windows.md), [Ubuntu Linux](https://github.com/Microsoft/ELL/blob/master/INSTALL-Ubuntu.md), [Mac](https://github.com/Microsoft/ELL/blob/master/INSTALL-Mac.md)). Specifically, this tutorial requires ELL, CMake, SWIG, and Python 3.6. 
* Follow the instructions for [setting up your Raspberry Pi](/ELL/tutorials/Setting-up-your-Raspberry-Pi).

## Step 1: Activate your environment and create a tutorial directory

If you followed the setup instructions, you should have an environment named `py36`. Open a terminal window and activate your Anaconda environment. 

```
[Unix] source activate py36
[Windows] activate py36
```

Then, cd into the directory where you built ELL and create a `tutorials` directory. In that directory, create another directory named `tutorial1`.

```
cd ELL/build
mkdir tutorials
cd tutorials
mkdir tutorial1
cd tutorial1
```

## Step 2: Download pre-trained model

Download this [compressed ELL model file](https://github.com/Microsoft/ELL-models/raw/master/models/ILSVRC2012/d_I224x224x3CMCMCMCMCMCMC1A/d_I224x224x3CMCMCMCMCMCMC1A.ell.zip) into the `tutorial1` directory. The model file contains a pre-trained Deep Neural Network for image classification, and is one of the models available from the [ELL gallery](/ELL/gallery). The file's long name indicates the Neural Network's architecture, but don't worry about that for now and save it locally as `model.ell.zip`. 

```
curl --location -o model.ell.zip https://github.com/Microsoft/ELL-models/raw/master/models/ILSVRC2012/d_I224x224x3CMCMCMCMCMCMC1A/d_I224x224x3CMCMCMCMCMCMC1A.ell.zip
```

Unzip the compressed file.

```
unzip model.ell.zip
```

Rename the `d_I224x224x3CMCMCMCMCMCMC1A.ell` model file to `model.ell`:

```
[Unix] mv d_I224x224x3CMCMCMCMCMCMC1A.ell model.ell
[Windows] ren d_I224x224x3CMCMCMCMCMCMC1A.ell model.ell
```

(One Windows, unzip is part of the Git distribution, for example, in `\Program Files\Git\usr\bin`.)
Next, download the file with the [category names](https://github.com/Microsoft/ELL-models/raw/master/models/ILSVRC2012/categories.txt) for this model into the `tutorial1` directory. The 1000 categories in this file are the types of objects that the model is trained to recognize.

```
curl --location -o categories.txt https://github.com/Microsoft/ELL-models/raw/master/models/ILSVRC2012/categories.txt
```

There should now be a `model.ell` file and a `categories.txt` file in the `tutorial1` directory.

## Step 3: Compile and run the model on your laptop or desktop computer 

Before deploying the model to the Raspberry Pi, we will practice deploying it to the laptop or desktop computer. Deploying an ELL model requires two steps. First, we run a tool named `wrap`, which both compiles `model.ell` into machine code and generates a CMake project to build a Python wrapper for it. Second, we call CMake to build the Python library. 

Run `wrap` as follows.

```
python "../../tools/wrap/wrap.py" categories.txt model.ell -lang python -target host
```

Note that we gave `wrap` the command line option `-target host`, which tells it to generate machine code for execution on the laptop or desktop computer, rather than machine code for the Raspberry Pi. If all goes well, you should see the following output.

```
compiling model...
generating python interfaces for model in host
running llc...
success, now you can build the 'host' folder
```

The `wrap` tool creates a `cmake` project in a new directory named `host`. Create a `build` directory inside the `host` directory and change to that directory

```
cd host
mkdir build
cd build
```

To finish creating the Python wrapper, build the `cmake` project. 

```
[Unix] cmake .. && make
[Windows] cmake -G "Visual Studio 14 2015 Win64" -DPROCESSOR_HINT=haswell .. && cmake --build . --config release
```

We have just created a Python module named `model`. This module provides functions that report the shapes of the model's input and output, as well as the `predict` function, which invokes the classifier. 

## Step 4: Invoke the model on your computer

The next step is to create a Python script that loads the model, sends images to it, and interprets the model's output. If you just want the full script, copy it from [here](/ELL/tutorials/Getting-Started-with-Image-Classification-on-the-Raspberry-Pi/callModel.py) into the `host` directory. Otherwise, create an empty text file named `callModel.py` in the `host` directory and copy in the code snippets below. 

First, import a few dependencies and add directories to the path, to allow Python to find the module that we created above.  

```python
import sys
import os
import cv2
import numpy as np

scriptPath = os.path.dirname(os.path.abspath(__file__))
sys.path.append(scriptPath)
sys.path.append(os.path.join(scriptPath, 'build'))
sys.path.append(os.path.join(scriptPath, 'build/Release'))
```

Next, load the Python module representing the wrapped ELL model.

```python
import model
```

Print the model's input and output shapes.

```python
input_shape = model.get_default_input_shape()
output_shape = model.get_default_output_shape()
print("Model input shape: " + str([input_shape.rows,input_shape.columns,input_shape.channels]))
print("Model output shape: " + str([output_shape.rows,output_shape.columns,output_shape.channels]))
```

You should see output similar to this.

```
Model input shape: [224, 224, 3]
Model output shape: [1, 1, 1000]
```

The input to the model is a 3-channel image of height 224 and width 224, which can also be represented as an array of size 224 * 224 * 3 = 150528. The shape of the output is 1 x 1 x 1000, which can be represented as an array of 1000 elements. Allocate an array to store the model's output. 

```python
predictions = model.FloatVector(output_shape.Size())
```

Choose an image file to send to the model. For example, use our coffee mug image, [coffeemug.jpg](/ELL/tutorials/shared/coffeemug.jpg). Use OpenCV to read the image:

```python
image = cv2.imread("coffeemug.jpg")
```

The image stored in the `image` variable cannot be sent to the model as-is, because the model takes its input as an array of `float` values. Moreover, the model expects the input image to have a certain shape and a specific ordering of the color channels (which, in this case, is Blue-Green-Red). Since preparing images for the model is such a common operation, we created a helper function for it named `prepare_image_for_model`. Copy and paste the code for [prepare_image_for_model](/ELL/tutorials/Getting-Started-with-Image-Classification-on-the-Raspberry-Pi/imageHelper.py) and put it at the top of the file, right after the `import model` statement. Then, call the helper function to prepare your image.

```python
input = prepare_image_for_model(image, input_shape.columns, input_shape.rows)
```

Finally, invoke the model by calling its `predict` method.

```python
model.predict(input, predictions)
```

The `predict` method fills the `predictions` array with probability scores, which sum to 1. Each element of this array corresponds to one of the 1000 image classes recognized by the model. Print the index of the highest confidence category.

```python
categories = open('categories.txt', 'r').readlines()
predictionIndex = int(np.argmax(predictions))
print("Category index: " + str(predictionIndex))
print("Category text: " + categories[predictionIndex])
print("Confidence: " + str(predictions[predictionIndex]))
```

This code also looks up the category name by reading the corresponding line in `categories.txt`. For example, if the highest confidence category is 504, line 504 of `categories.txt` is `coffee mug`. The value at `predictions[504]` is the model's confidence in this prediction. For example, a value of `0.514353` means that the model is 51% confident that the image contains a coffee mug.

## Step 5: Compile the model for execution on the Raspberry Pi

We are ready to cross-compile the model for deployment on the Raspberry Pi. As before, run the `wrap` tool on your laptop or desktop computer, but this time specify the target platform as `pi3`. This tells the ELL compiler to generate machine code for the Raspberry Pi's ARM Cortex A53 processor.

```
python "../../tools/wrap/wrap.py" categories.txt model.ell -lang python -target pi3   
```

The `wrap` tool creates a new directory named `pi3`, which contains a CMake project that can be used to build the desired Python module. This time, we need to build this project on the Raspberry Pi. Before moving to the Pi, we also want to copy over some Python helper code: 

```
[Unix] cp ../../../docs/tutorials/shared/tutorialHelpers.py pi3
[Windows] copy ..\..\..\docs\tutorials\shared\tutorialHelpers.py pi3
```

At this point, you should now have a `pi3` folder that contains a `cmake` project that builds the Python wrapper and some helpful Python utilities.

## Step 6: Write code to invoke the model on the Raspberry Pi

We will write a Python script that invokes the model and runs the demo on a Raspberry Pi. The script will load the Python wrapper that we created above, read images from the camera, pass these images to the model, and display the classification results. If you just want the full script, copy it from [here](/ELL/tutorials/shared/tutorial.py). Otherwise, create an empty text file named `tutorial.py` and copy in the code snippets below. 

First, import a few dependencies, including system utilities, opencv, numpy and the `modelHelper` code that we copied over.

```python
import sys
import os
import time
import numpy as np
import cv2
```
For convenience, include the helper code that we copied earlier.

```python
import tutorialHelpers as helpers
```

Import the module that contains the compiled ELL model.

```python
import model
```

The following function helps us read an image from the camera.

```python
def get_image_from_camera(camera):
    if camera is not None:
        ret, frame = camera.read()
        if (not ret):
            raise Exception('your capture device is not returning images')
        return frame
    return None
```

Define the main entry point and use the camera as the image source.

```python
def main():
    camera = cv2.VideoCapture(0)
```

Read the file of category names.

```python
    categories = open('categories.txt', 'r').readlines()
```

The model expects its input in a certain shape. Get this shape and store it for use later on. 

```python
    inputShape = model.get_default_input_shape()
```

Allocate an array to store the model's output. 

```python
    outputShape = model.get_default_output_shape()
    predictions = model.FloatVector(outputShape.Size())
```

Next, set up a loop that keeps going until OpenCV indicates it is done, which is when the user hits any key. At the start of every loop iteration, read an image from the camera.

```python
    while (cv2.waitKey(1) == 0xFF):
        image = get_image_from_camera(camera)
```

As mentioned above, the image stored in the `image` variable cannot be sent to the model as-is and needs to be processed using the [prepare_image_for_model](/ELL/tutorials/Getting-Started-with-Image-Classification-on-the-Raspberry-Pi/imageHelper.py) helper function.

```python
        input = helpers.prepare_image_for_model(image, inputShape.columns, inputShape.rows)
```

With the processed image input handy, call the `predict` method to invoke the model. 

```python
        model.predict(input, predictions)
```

As before, the `predict` method fills the `predictions` array with the model output. Each element of this array corresponds to one of the 1000 image classes recognized by the model. Extract the top 5 predicted categories by calling the helper function `get_top_n`.

```python
        top5 = helpers.get_top_n(predictions, 5)
```

`top5` is an array of tuples, where the first element is the category index and the second element is the probability of that category. Match the category indices in `top5` with the category names in `categories`.

```python
        headerText = "".join(["(" + str(int(element[1]*100)) + "%) " + categories[element[0]] for element in top5])
```

Use the `draw_header` helper function to display the predicted category on the Raspberry Pi's display. Also display the camera image. 

```python
        helpers.draw_header(image, headerText)
        cv2.imshow('ELL model', image)
```
Finally, write the code that invokes the `main` function and runs your script.

```python
if __name__ == "__main__":
    main()
```

We are ready to move to the Raspberry Pi. You can copy the `pi3` folder to the Pi using the Unix `scp` tool or the Windows [WinSCP](https://winscp.net/eng/index.php) tool.

## Step 7: Build the Python module on the Raspberry Pi

Log into your Raspberry Pi, either remotely using SSH or directly if you have a keyboard and display connected. Find the `pi3` folder you just copied from your computer and build the python module that wraps the ELL model.

```
cd pi3
mkdir build
cd build
cmake ..
make
cd ..
```

We just created a Python module named `model`, which includes functions that report the model's input and output dimensions and enables us to pass images to the model for classification. 

## Step 8: Classify live video on the Raspberry Pi

If you followed the [Raspberry Pi Setup Instructions](/ELL/tutorials/Setting-up-your-Raspberry-Pi), you should have an anaconda environment named `py34`. Activate it and run the script that we wrote above. 

```
source activate py34
python tutorial.py
```

If you have a camera and display connected to your Pi, you should see a window similar to the screenshot at the top of this page. Point your camera at different objects and see how the model classifies them. Look at `categories.txt` to see which categories the model is trained to recognize and try to show those objects to the model. For quick experimentation, point the camera to your computer screen and have your computer display images of different objects. For example, experiment with different dog breeds and different types of African animals. 

If you copied the full source for [tutorial.py](/ELL/tutorials/shared/tutorial.py), you will also see the average time in milliseconds it takes the model to process each image. If you compare the displayed time with the time indicated in the [ELL gallery](/ELL/gallery/), you will notice that your model runs slower than it should. The slow down is caused by inefficiencies in the Python wrapper, and we are working to fix this problem. To run the model at its top speed, follow the [C++ version of this tutorial](/ELL/tutorials/Getting-started-with-image-classification-in-cpp).

## Next steps

The [ELL gallery](/ELL/gallery/) offers different models for image classification. Some are slow but accurate, while others are faster and less accurate. Different models can even lead to different power draw on the Raspberry Pi. Repeat the steps above with different models. 

We used the `wrap` tool as a convenient way to compile the model and prepare for building its Python wrapper. To understand what `wrap` does under the hood, read the [wrap documentation](https://github.com/Microsoft/ELL/blob/master/tools/wrap/README.md).

## Troubleshooting

If you run into trouble, you can find some troubleshooting instructions at the bottom of the [Raspberry Pi Setup Instructions](/ELL/tutorials/Setting-up-your-Raspberry-Pi).