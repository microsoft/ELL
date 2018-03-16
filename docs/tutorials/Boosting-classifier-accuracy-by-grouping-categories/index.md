---
layout: default
title: Boosting classifier accuracy by grouping categories
permalink: /tutorials/Boosting-classifier-accuracy-by-grouping-categories/
---
# Boosting classifier accuracy by grouping categories

*by Chris Lovett, Byron Changuion, Ofer Dekel, and Lisa Ong*

The pretrained models in the [Embedded Learning Library (ELL) gallery](/ELL/gallery/) are trained to identify 1,000 different image categories (see the category names [here](https://github.com/Microsoft/ELL-models/raw/master/models/ILSVRC2012/categories.txt)). This tutorial uses that image classification model to help solve a simpler classification problem: distinguishing among *dogs*, *cats*, and *other* (anything that isn't a dog or a cat). This classification model has an error rate of 64% on the 1,000-class problem, but it has a low error rate of just 5.7% on the 3-class problem. 

The tutorial includes instructions for a Python script that reads images from the camera and prints `Woof!` for dog recognition and `Meow!` for cat recognition while showing the class *Dog* or *Cat* as the window header text. In addition, the **Next steps** section describes how to include sound clips and also how to use callbacks with the model.

---

![screenshot](/ELL/tutorials/Boosting-classifier-accuracy-by-grouping-categories/Screenshot.jpg)

## Before you begin
Complete the following steps before starting the tutorial.
* Install ELL on your computer ([Windows](https://github.com/Microsoft/ELL/blob/master/INSTALL-Windows.md), [Ubuntu Linux](https://github.com/Microsoft/ELL/blob/master/INSTALL-Ubuntu.md), [macOS](https://github.com/Microsoft/ELL/blob/master/INSTALL-Mac.md)).
* Follow the instructions for [setting up your Raspberry Pi](/ELL/tutorials/Setting-up-your-Raspberry-Pi).
* Optional: Complete the tutorial [Getting started with image classification on Raspberry Pi](/ELL/tutorials/Getting-started-with-image-classification-on-the-Raspberry-Pi/) to learn how to produce a Python wrapper for an ELL model.

## What you will need

* Laptop or desktop computer
* Raspberry Pi 3
* Raspberry Pi camera or USB webcam
* Optional: active cooling attachment (refer to the [tutorial on cooling your Pi](/ELL/tutorials/Active-cooling-your-Raspberry-Pi-3/))


## Deploy a pretrained model on the Raspberry Pi device

You'll start by repeating the steps of [Getting Started with Image Classification on Raspberry Pi](/ELL/tutorials/Getting-started-with-image-classification-on-the-Raspberry-Pi/), but replace the model suggested in that tutorial with [this faster and less accurate model](https://github.com/Microsoft/ELL-models/raw/master/models/ILSVRC2012/d_I160x160x3CMCMBMBMBMBMC1AS/d_I160x160x3CMCMBMBMBMBMC1AS.ell.zip). Use this general process:

1. Download [the model](https://github.com/Microsoft/ELL-models/raw/master/models/ILSVRC2012/d_I160x160x3CMCMBMBMBMBMC1AS/d_I160x160x3CMCMBMBMBMBMC1AS.ell.zip) to your computer.
2. Use the `wrap` tool to compile it for the Raspberry Pi device.
3. Copy the resulting CMake project to the Pi device.
4. Build the project on the Pi device. 

Now, a Python module named `model` is on your Pi device.

Copy the following files to your Pi device.
- [dogs.txt](/ELL/tutorials/Boosting-classifier-accuracy-by-grouping-categories/dogs.txt)
- [cats.txt](/ELL/tutorials/Boosting-classifier-accuracy-by-grouping-categories/cats.txt)
- [tutorial_helpers.py](/ELL/tutorials/shared/tutorial_helpers.py)
- [categories.txt](https://github.com/Microsoft/ELL-models/raw/master/models/ILSVRC2012/categories.txt)

## Write a script

Next, you'll write a Python script that invokes the model on your Raspberry Pi device, groups the categories as described above, and takes action when a dog or cat is recognized. If you just want the code, copy the complete script from [here](/ELL/tutorials/Boosting-classifier-accuracy-by-grouping-categories/pets.py). Otherwise, create an empty text file named `pets.py` and copy in the code snippets below.

Import the required modules.

```python
import cv2
import tutorial_helpers as helpers
```

Import the Python module for the compiled ELL model.

```python
import model
```

Define a helper function that reads images from the camera.

```python
def get_image_from_camera(camera):
    if camera:
        ret, frame = camera.read()
        if not ret:
            raise Exception("your capture device is not returning images")
        return frame
    return None
```

Next, define helper functions that check whether a category is contained in a category list. The predicted category is the numeric index of the prediction, so the helper function verifies that the prediction index is contained in the set. (The index is based on which line that category is within the categories.txt file.)

```python
def prediction_index_in_set(prediction_index, set):
    for x in set:
        if prediction_index == int(x):
            return True
    return False
```
Define helper functions that print *woof* or  *meow* appropriately.  

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

Read the list of dog breed categories from `dogs.txt`, and the list of cat breed categories from `cats.txt`.

```python
    with open("dogs.txt", "r") as dogs_file,\
         open("cats.txt", "r") as cats_file:
        dogs = dogs_file.read().splitlines()
        cats = cats_file.read().splitlines()
```

Get the model input shape, which we will use to prepare the input data.

```python
    input_shape = model.get_default_input_shape()
```

Declare a loop so that the camera can get an image and prepare it to be used as input to the model. The preparation of the image involves cropping and resizing the image while maintaining the aspect ratio,
reordering the image channels (if needed), and returning the image data as a flat **NumPy** array of floats so that it can be provided as input to the model.

```python
    while (cv2.waitKey(1) & 0xFF) == 0xFF:
        image = get_image_from_camera(camera)

        input_data = helpers.prepare_image_for_model(
            image, input_shape.columns, input_shape.rows)
```

Send the processed image to the model to get a **NumPy** array of predictions.

```python
        predictions = model.predict(input_data)
```

Use the helper function to get the top prediction. The *threshold* parameter selects predictions with a 5% or higher confidence.

```python
        top_n = helpers.get_top_n(predictions, 1, threshold=0.05)
```

Check whether the prediction is part of a group.

```python
        group = ""
        label = ""
        if top_n:
            top = top_n[0][0]
            if prediction_index_in_set(top, dogs):
                group = "Dog"
            elif prediction_index_in_set(top, cats):
                group = "Cat"
```

If the prediction is in one of the defined category groups, take the appropriate action.

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

## Classify live video on the Raspberry Pi device

By following the [Raspberry Pi Setup Instructions](/ELL/tutorials/Setting-up-your-Raspberry-Pi), you now have an anaconda environment named `py34`. Activate the environment and run the script.

```shell
source activate py34
python pets.py
```

Point the camera at different objects and see how the model classifies them. Look at `dogs.txt` and `cats.txt` to see which categories the model is trained to recognize and try to show those objects to the model. For quick experimentation, point the camera to your computer screen, have your computer display images of different animals, and see when it barks or meows. If you copied the full `pets.py` script from [here](/ELL/tutorials/Boosting-classifier-accuracy-by-grouping-categories/pets.py), you will also see the average time it takes for the model to process a single frame.

## Next steps
### Playing sounds
A fun next step would be to introduce the playing of sounds to indicate the detection of either a dog or cat. To do this, download a dog's bark [here](http://freesound.org/people/davidmenke/sounds/231762/) and a
cat's meow [here](https://freesound.org/people/tuberatanka/sounds/110011/). These can be used with the **play_sound** function that's available in the `tutorial_helpers` module, to play sounds on your computer or on your Raspberry Pi. Find more details on playing sounds at [Notes on Playing Audio](/ELL/tutorials/Notes-on-playing-audio).

Here's an example of the classifier running with the audio barks and meows. 

[![screenshot](/ELL/tutorials/Boosting-classifier-accuracy-by-grouping-categories/thumbnail.png)](https://youtu.be/SOmV8tzg_DU)

### Using callbacks
The **predict** function on the ELL model provides a direct way to send input to the model and get predictions as parameters to the function.

Instead of using the **predict** function, you can implement input and output callbacks that the ELL model calls when necessary.

The `pets_callback.py` script from [here](/ELL/tutorials/Boosting-classifier-accuracy-by-grouping-categories/pets_callback.py) demonstrates how to provide callbacks for the input image and the output predictions.

First, define a class called **CatsDogsPredictor** that extends the **model.Model** class.

```python
class CatsDogsPredictor(model.Model):
    """Class that implements input and output callbacks for the ELL model
    by deriving from the Model base class.
    """
```

**CatsDogsPredictor.__init__** performs initialization for implementing the callbacks later.

```python
    def __init__(self, camera, cats, dogs):
        """Initializes this object with the camera source and model-related
        information"""
        model.Model.__init__(self)

        self.camera = camera
        self.dogs = dogs
        self.cats = cats

        self.input_shape = model.get_default_input_shape()
        self.image = None
```

**CatsDogsPredictor.input_callback** gets an image from the camera, processes it, and returns it to the ELL model. The ELL model will call this when it is ready to get input. 

```python
    def input_callback(self):
        """The input callback that returns an image to the model"""
        self.image = get_image_from_camera(self.camera)

        return helpers.prepare_image_for_model(
            self.image, self.input_shape.columns, self.input_shape.rows)
```

**CatsDogsPredictor.output_callback** receives predictions from the ELL model and determines which group the top prediction belongs to. This prints *Woof!* and *Meow!* appropriately while showing the class *Dog* or *Cat* as the window header text. Alternatively, you can choose to play a sound by following the instructions in [Playing sounds](#playing-sounds)).


```python
    def output_callback(self, predictions):
        """The output callback that the model calls when predictions are ready"""

        header_text = ""
        group, probability = self.get_group(predictions)

        if group:
            if group == "Dog":
                print("Woof!")
            elif group == "Cat":
                print("Meow!")            
            header_text = "({:.0%}) {}".format(probability, group)

        helpers.draw_header(self.image, header_text)
        cv2.imshow("Grouping (with callbacks)", self.image)
```

As a final step, the main entry point creates a `CatsDogsPredictor` object and calls its **predict** method in a loop. Each call to **predict** runs one iteration of the ELL model and invokes the callbacks.

```python
def main():
    """Entry point for the script when called directly"""
    camera = cv2.VideoCapture(0)

    with open("dogs.txt", "r") as dogs_file,\
         open("cats.txt", "r") as cats_file:
        dogs = dogs_file.read().splitlines()
        cats = cats_file.read().splitlines()

    predictor = CatsDogsPredictor(camera, cats, dogs)

    while (cv2.waitKey(1) & 0xFF) == 0xFF:
        predictor.predict()
```

## Troubleshooting
Find tips in the Troubleshooting section of the [Raspberry Pi Setup Instructions](/ELL/tutorials/Setting-up-your-Raspberry-Pi).

