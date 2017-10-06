---
layout: default
title: Boosting classifier accuracy by grouping categories
permalink: /tutorials/Boosting-classifier-accuracy-by-grouping-categories/
---
# Boosting classifier accuracy by grouping categories

*by Chris Lovett, Byron Changuion, and Ofer Dekel*

In this tutorial, we will take an image classification model that was trained to recognize 1000 different image categories and use it to solve a simpler classification problem: distinguishing between *dogs*, *cats*, and *other* (anything that isn't a dog or a cat). We will see how a model with low classification accuracy on the original 1000-class problem can have a sufficiently high accuracy on the simpler 3-class problem. We will write a Python script that reads images from the camera, barks if it sees a dog, and meows if it sees a cat.

---

[![screenshot](/ELL/tutorials/Boosting-classifier-accuracy-by-grouping-categories/thumbnail.png)](https://youtu.be/SOmV8tzg_DU)

#### Materials

* Laptop or desktop computer
* Raspberry Pi 3
* Headphones or speakers for your Raspberry Pi
* Raspberry Pi camera or USB webcam
* *optional* - Active cooling attachment (see our [tutorial on cooling your Pi](/ELL/tutorials/Active-cooling-your-Raspberry-Pi-3/))

#### Prerequisites

* Install ELL on your computer ([Windows](https://github.com/Microsoft/ELL/blob/master/INSTALL-Windows.md), [Ubuntu Linux](https://github.com/Microsoft/ELL/blob/master/INSTALL-Ubuntu.md), [Mac](https://github.com/Microsoft/ELL/blob/master/INSTALL-Mac.md)).
* Follow the instructions for [setting up your Raspberry Pi](/ELL/tutorials/Setting-up-your-Raspberry-Pi).
* Complete the basic tutorial, [Getting started with image classification on Raspberry Pi](/ELL/tutorials/Getting-started-with-image-classification-on-the-Raspberry-Pi/), to learn how to produce a Python wrapper for an ELL model.

## Overview

The pre-trained models in the [ELL gallery](/ELL/gallery/) are trained to identify 1000 different image categories (see the category names [here](https://github.com/Microsoft/ELL-models/raw/master/models/ILSVRC2012/categories.txt)). Often times, we are only interested in a subset of these categories and we don't require the fine-grained categorization that the model was trained to provide. For example, we may want to distinguish between images of dogs versus images of cats, whereas the model is actually trained to distinguish between several different varieties of cats and over 100 different dog breeds.

The dogs versus cats classification problem is easier than the original 1000 class problem, so a model that isn't very accurate on the original problem may be perfectly adequate for the simpler problem. Specifically, we will use a model that has an error rate of 64% on the 1000-class problem, but only 5.7% on the 3-class problem. We will write a script that grabs a frame from a camera, plays a barking sound when it recognizes one of the dog varieties, and plays a meow sound when it recognizes one of the cat varieties.

## Step 1: Deploy a pre-trained model on the Raspberry Pi

Start by repeating the steps of the basic tutorial, [Getting Started with Image Classification on Raspberry Pi](/ELL/tutorials/Getting-started-with-image-classification-on-the-Raspberry-Pi/), but replace the model suggested in that tutorial with [this faster and less accurate model](https://github.com/Microsoft/ELL-models/raw/master/models/ILSVRC2012/d_I160x160x3NCMNCMNBMNBMNBMNBMNC1A/d_I160x160x3NCMNCMNBMNBMNBMNBMNC1A.ell.zip). Namely, download the model to your computer, use the `wrap` tool to compile it for the Raspberry Pi, copy the resulting CMake project to the Pi, and build it there. After completing these steps, you should have a Python module on your Pi named `model`.

Copy the following files to your Pi.
- [dogs.txt](/ELL/tutorials/Boosting-classifier-accuracy-by-grouping-categories/dogs.txt)
- [cats.txt](/ELL/tutorials/Boosting-classifier-accuracy-by-grouping-categories/cats.txt)
- [tutorialHelpers.py](/ELL/tutorials/shared/tutorialHelpers.py)

Additionally, download or record `.wav` sound files of a dog bark and a cat meow (for example, try this [bark](http://freesound.org/people/davidmenke/sounds/231762/) and this [meow](http://freesound.org/people/blimp66/sounds/397661/) ). Name the bark sound file `woof.wav` and the meow sound file `meow.wav`.

## Step 2: Write a script 

We will write a Python script that invokes the model on a Raspberry Pi, groups the categories as described above, and takes action if a dog or cat is recognized. If you just want the code, copy the complete script from [here](/ELL/tutorials/Boosting-classifier-accuracy-by-grouping-categories/pets.py). Otherwise, create an empty text file named `pets.py` and copy in the code snippets below. 

First, import the required modules.

```python
import sys
import os
import numpy as np
import cv2
import time
import subprocess
if (os.name == "nt"):
    import winsound
import tutorialHelpers as helpers
```

Also, import the Python module for the compiled ELL model.

```python
import model
```

As in previous tutorials, define a helper functions that reads images from the camera.

```python
def get_image_from_camera(camera):
    if camera is not None:
        ret, frame = camera.read()
        if (not ret):
            raise Exception('your capture device is not returning images')
        return frame
    return None
```

Next, define helper functions that check whether a category is contained in a category list. Since categories can sometimes have more than one text description, each category may contain several strings, separated by commas. Checking whether a category matches means checking whether any one of those elements is contained in the category name, and whether any match occurs in the set.

```python
def labels_match(a, b):
    x = [s.strip().lower() for s in a.split(',')]
    y = [s.strip().lower() for s in b.split(',')]
    for w in x:
        if (w in y):
            return True
    return False

def label_in_set(label, label_set):
    for x in label_set:
        if labels_match(label, x):
            return True
    return False
```

When a prediction belonging to the dog group or the cat group is detected, we want to play the appropriate sound file. Define helper functions that play a bark or a meow.

```python
script_path = os.path.dirname(os.path.abspath(__file__))
woofSound = os.path.join(script_path, "woof.wav")
meowSound = os.path.join(script_path, "meow.wav")

def play(filename):
    if (os.name == "nt"):
        winsound.PlaySound(filename, winsound.SND_FILENAME | winsound.SND_ASYNC)
    else:
        command = ["aplay", filename]
        proc = subprocess.Popen(command, stdout=subprocess.PIPE, stderr=subprocess.PIPE, bufsize=0, universal_newlines = True)
        proc.wait()

def take_action(group):
    if group == "Dog":
        play(woofSound)
    elif group == "Cat":
        play(meowSound)
```

Define the main entry point and start the camera.

```python
 def main():
    camera = cv2.VideoCapture(0)
```

Read the category names from `categories.txt`, the list of dog breed categories from `dogs.txt`, and the list of cat breed categories from `cats.txt`.

```python
    categories = open('categories.txt', 'r').readlines()
    dogs = open('dogs.txt', 'r').readlines()
    cats = open('cats.txt', 'r').readlines()
```

Get the model input and output shapes and allocate an array to hold the model output. 

```python
    inputShape = model.get_default_input_shape()

    outputShape = model.get_default_output_shape()
    predictions = model.FloatVector(outputShape.Size())
```

For this tutorial, we'll keep some state to ensure we don't keep taking the same action over and over for the same image. Initialize the state as follows.

```python
    lastHist = None
    significantDiff = 5000
    lastPredictionTime = 0
    headerText = ""
```

Declare a loop where we get an image from the camera and prepare it to be used as input to the model.

```python
    while (cv2.waitKey(1) == 0xFF):
        image = get_image_from_camera(camera)

        # Prepare the image to pass to the model. This helper:
        # - crops and resizes the image maintaining proper aspect ratio
        # - reorders the image channels if needed
        # - returns the data as a ravelled numpy array of floats so it can be handed to the model
        input = helpers.prepare_image_for_model(image, inputShape.columns, inputShape.rows)
```

We'll use OpenCV to get a histogram using OpenCV as a quick way to detect whether the image has changed significantly. This is to create a better experience than having the same action be taken on the same prediction over and over. We'll also ensure that enough time has passed for the sound file to have fully played out.

```python
        hist = np.histogram(input,16,[0,256])[0]
        diff = 1
        if lastHist is None:
            lastHist = hist           
        else:
            diff = max(lastHist - hist)

        # Check whether the image has changed significantly and that enough time has passed
        # since our last prediction to decide whether to predict again
        now = time.time()
        if diff >= significantDiff and now - lastPredictionTime > 2:
```

Send the processed image to the model and its array of predictions.

```python
            model.predict(input, predictions)
```

Use the helper function to get the top prediction.

```python
            topN = helpers.get_top_n(predictions, 1)
```

Check whether the prediction is part of a group.

```python
            group = ""
            label = ""
            if len(topN) > 0:
                top = topN[0]
                label = categories[top[0]]
                if label_in_set(label, dogs):
                    group = "Dog"
                elif label_in_set(label, cats):
                    group = "Cat"
```

If the prediction is in one of the define category groups, take the appropriate action.

```python
            if not group == "":
                # A group was detected, so take action
                top = topN[0]
                take_action(group)
                headerText = "(" + str(int(top[1]*100)) + "%) " + group
                lastPredictionTime = now
                lastHist = hist
            else:
                # No group was detected
                headerText = ""
```

Finally, update the state if enough time has passed and display the image and header text.

```python
        if now - lastPredictionTime > 2:
            # Reset the header text
            headerText = ""

        helpers.draw_header(image, headerText)
        # Display the image using opencv
        cv2.imshow('Grouping', image)

if __name__ == "__main__":
    main()
```

## Step 3: Classify live video on the Raspberry Pi

If you followed the [Raspberry Pi Setup Instructions](/ELL/tutorials/Setting-up-your-Raspberry-Pi), you should have an anaconda environment named `py34`. Activate the environment and run the script.   

```
source activate py34
python pets.py
```

Point your camera at different objects and see how the model classifies them. Look at `dogs.txt` and `cats.txt` to see which categories the model is trained to recognize and try to show those objects to the model. For quick experimentation, point the camera to your computer screen, have your computer display images of different animals, and see when it barks or meows. If you copied the full `pets.py` script from [here](/ELL/tutorials/Boosting-classifier-accuracy-by-grouping-categories/pets.py), you will also see the average time it takes for the model to process a single frame.

## Troubleshooting
If you run into trouble, you can find some troubleshooting instructions at the bottom of the [Raspberry Pi Setup Instructions](/ELL/tutorials/Setting-up-your-Raspberry-Pi).
