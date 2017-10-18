---
layout: default
title: Boosting classifier accuracy by grouping categories
permalink: /tutorials/Boosting-classifier-accuracy-by-grouping-categories/
---
# Boosting classifier accuracy by grouping categories

*by Chris Lovett, Byron Changuion, and Ofer Dekel*

In this tutorial, we will take an image classification model that was trained to recognize 1000 different image categories and use it to solve a simpler classification problem: distinguishing between *dogs*, *cats*, and *other* (anything that isn't a dog or a cat). We will see how a model with low classification accuracy on the original 1000-class problem can have a sufficiently high accuracy on the simpler 3-class problem. We will write a Python script that reads images from the camera, and prints `Woof!!` if it sees a dog and `Meow!!` if it sees a cat.

---

[![screenshot](/ELL/tutorials/Boosting-classifier-accuracy-by-grouping-categories/thumbnail.png)](https://youtu.be/SOmV8tzg_DU)

#### Materials

* Laptop or desktop computer
* Raspberry Pi 3
* Raspberry Pi camera or USB webcam
* *optional* - Active cooling attachment (see our [tutorial on cooling your Pi](/ELL/tutorials/Active-cooling-your-Raspberry-Pi-3/))

#### Prerequisites

* Install ELL on your computer ([Windows](https://github.com/Microsoft/ELL/blob/master/INSTALL-Windows.md), [Ubuntu Linux](https://github.com/Microsoft/ELL/blob/master/INSTALL-Ubuntu.md), [macOS](https://github.com/Microsoft/ELL/blob/master/INSTALL-Mac.md)).
* Follow the instructions for [setting up your Raspberry Pi](/ELL/tutorials/Setting-up-your-Raspberry-Pi).
* Complete the basic tutorial, [Getting started with image classification on Raspberry Pi](/ELL/tutorials/Getting-started-with-image-classification-on-the-Raspberry-Pi/), to learn how to produce a Python wrapper for an ELL model.

## Overview

The pre-trained models in the [ELL gallery](/ELL/gallery/) are trained to identify 1000 different image categories (see the category names [here](https://github.com/Microsoft/ELL-models/raw/master/models/ILSVRC2012/categories.txt)). Often times, we are only interested in a subset of these categories and we don't require the fine-grained categorization that the model was trained to provide. For example, we may want to distinguish between images of dogs versus images of cats, whereas the model is actually trained to distinguish between several different varieties of cats and over 100 different dog breeds.

The dogs versus cats classification problem is easier than the original 1000 class problem, so a model that isn't very accurate on the original problem may be perfectly adequate for the simpler problem. Specifically, we will use a model that has an error rate of 64% on the 1000-class problem, but only 5.7% on the 3-class problem. We will write a script that grabs a frame from a camera, plays a barking sound when it recognizes a dog, and plays a meow sound when it recognizes a cat.

## Step 1: Deploy a pre-trained model on the Raspberry Pi

Start by repeating the steps of the basic tutorial, [Getting Started with Image Classification on Raspberry Pi](/ELL/tutorials/Getting-started-with-image-classification-on-the-Raspberry-Pi/), but replace the model suggested in that tutorial with [this faster and less accurate model](https://github.com/Microsoft/ELL-models/raw/master/models/ILSVRC2012/d_I160x160x3CMCMBMBMBMBMC1AS/d_I160x160x3CMCMBMBMBMBMC1AS.ell.zip). Namely, download the model to your computer, use the `wrap` tool to compile it for the Raspberry Pi, copy the resulting CMake project to the Pi, and build it there. After completing these steps, you should have a Python module on your Pi named `model`.

Copy the following files to your Pi.
- [dogs.txt](/ELL/tutorials/Boosting-classifier-accuracy-by-grouping-categories/dogs.txt)
- [cats.txt](/ELL/tutorials/Boosting-classifier-accuracy-by-grouping-categories/cats.txt)
- [tutorial_helpers.py](/ELL/tutorials/shared/tutorial_helpers.py)

## Step 2: Write a script

We will write a Python script that invokes the model on a Raspberry Pi, groups the categories as described above, and takes action if a dog or cat is recognized. If you just want the code, copy the complete script from [here](/ELL/tutorials/Boosting-classifier-accuracy-by-grouping-categories/pets.py). Otherwise, create an empty text file named `pets.py` and copy in the code snippets below.

First, import the required modules.

```python
import cv2
import tutorial_helpers as helpers
```

Also, import the Python module for the compiled ELL model.

```python
import model
```

As in previous tutorials, define a helper function that reads images from the camera.

```python
def get_image_from_camera(camera):
    if camera:
        ret, frame = camera.read()
        if not ret:
            raise Exception("your capture device is not returning images")
        return frame
    return None
```

Next, define helper functions that check whether a category is contained in a category list. Since categories can sometimes have more than one text description, each category may contain several strings, separated by commas. Checking whether a category matches means checking whether any one of those elements is contained in the category name, and whether any match occurs in the set.

```python
def labels_match(a, b):
    x = [s.strip().lower() for s in a.split(",")]
    y = [s.strip().lower() for s in b.split(",")]
    for w in x:
        if w in y:
            return True
    return False

def label_in_set(label, label_set):
    for x in label_set:
        if labels_match(label, x):
            return True
    return False
```
When a prediction belonging to the dog group or the cat group is detected, we want to print the appropriate string. Define helper functions that print a woof or a meow.

```python
def take_action(group):
    if group == "Dog":
        print("Woof!!")
    elif group == "Cat":
        print("Meow!!")
```
Define the main entry point and start the camera.

```python
def main():
    camera = cv2.VideoCapture(0)
```

Read the category names from `categories.txt`, the list of dog breed categories from `dogs.txt`, and the list of cat breed categories from `cats.txt`.

```python
    with open("categories.txt", "r") as categories_file,\
            open("dogs.txt", "r") as dogs_file,\
            open("cats.txt", "r") as cats_file:
        categories = categories_file.read().splitlines()
        dogs = dogs_file.read().splitlines()
        cats = cats_file.read().splitlines()
```

Get the model input and output shapes and allocate an array to hold the model output.

```python
    input_shape = model.get_default_input_shape()

    predictions = model.FloatVector(model.get_default_output_shape().Size())
```

Declare a loop where we get an image from the camera and prepare it to be used as input to the model.
The preparation of the image involves cropping and resizing the image while maintaining the aspect ratio,
reordering the image channels (if needed), and returning the image data as a flat `numpy` array of
floats so that it can be provided as input to the model.

```python
    while (cv2.waitKey(1) & 0xFF) == 0xFF:
        image = get_image_from_camera(camera)

        input_data = helpers.prepare_image_for_model(
            image, input_shape.columns, input_shape.rows)
```

Send the processed image to the model get and its array of predictions.

```python
        model.predict(input_data, predictions)
```

Use the helper function to get the top prediction. The `threshold` parameter selects predictions with a 5% or higher confidence.

```python
        top_n = helpers.get_top_n(predictions, 1, threshold=0.05)
```

Check whether the prediction is part of a group.

```python
        group = ""
        label = ""
        if top_n:
            top = top_n[0]
            label = categories[top[0]]
            if label_in_set(label, dogs):
                group = "Dog"
            elif label_in_set(label, cats):
                group = "Cat"
```

If the prediction is in one of the define category groups, take the appropriate action.

```python
        header_text = ""
        if group:
            top = top_n[0]
            take_action(group)
            header_text = "({:.0%}) {}".format(top[1], group)
```

Finally, display the image and header text.

```python
        helpers.draw_header(image, header_text)

        cv2.imshow("Grouping", image)

if __name__ == "__main__":
    main()
```

## Step 3: Classify live video on the Raspberry Pi

If you followed the [Raspberry Pi Setup Instructions](/ELL/tutorials/Setting-up-your-Raspberry-Pi), you should have an anaconda environment named `py34`. Activate the environment and run the script.

```shell
source activate py34
python pets.py
```

Point your camera at different objects and see how the model classifies them. Look at `dogs.txt` and `cats.txt` to see which categories the model is trained to recognize and try to show those objects to the model. For quick experimentation, point the camera to your computer screen, have your computer display images of different animals, and see when it barks or meows. If you copied the full `pets.py` script from [here](/ELL/tutorials/Boosting-classifier-accuracy-by-grouping-categories/pets.py), you will also see the average time it takes for the model to process a single frame.

## Next steps

A fun next step would be to introduce the playing of sounds indicate whether a dog or cat was detected. For
example, a dog's bark can be downloaded [here](http://freesound.org/people/davidmenke/sounds/231762/) and a
cat's meow can be downloaded [here](https://freesound.org/people/tuberatanka/sounds/110011/).

These can be used with the `play_sound` function that's available in the `tutorial_helpers` module, to play
sounds on your computer or on the Raspberry Pi. More details on playing sounds can be found in [Notes on Playing Audio](/ELL/tutorials/Notes-on-Playing-Audio).

## Troubleshooting
If you run into trouble, you can find some troubleshooting instructions at the bottom of the [Raspberry Pi Setup Instructions](/ELL/tutorials/Setting-up-your-Raspberry-Pi).
