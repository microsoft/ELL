####################################################################################################
##
##  Project:  Embedded Learning Library (ELL)
##  File:     pets.py
##  Authors:  Chris Lovett
##            Byron Changuion
##
##  Requires: Python 3.x
##
####################################################################################################

import sys
import os
import numpy as np
import cv2
import time
import tutorialHelpers as helpers
import subprocess
if (os.name == "nt"):
    import winsound

# import the ELL model's Python module
import model

# Function to return an image from our camera using OpenCV
def get_image_from_camera(camera):
    if camera is not None:
        # if predictor is too slow frames get buffered, this is designed to flush that buffer
        ret, frame = camera.read()
        if (not ret):
            raise Exception('your capture device is not returning images')
        return frame
    return None

# Return an array of strings corresponding to the model's recognized categories or classes.
# The order of the strings in this file are expected to match the order of the
# model's output predictions.
def get_categories_from_file(fileName):
    labels = []
    with open(fileName) as f:
        labels = f.read().splitlines()
    return labels

# Returns True if an element of the comma separated label `a` is an element of the comma separated label `b`
def labels_match(a, b):
    x = [s.strip().lower() for s in a.split(',')]
    y = [s.strip().lower() for s in b.split(',')]
    for w in x:
        if (w in y):
            return True
    return False

# Returns True if the label is in the set of labels
def label_in_set(label, label_set):
    for x in label_set:
        if labels_match(label, x):
            return True
    return False

# Declare variables that define where to find the sounds files we will play
script_path = os.path.dirname(os.path.abspath(__file__))
woofSound = os.path.join(script_path, "woof.wav")
meowSound = os.path.join(script_path, "meow.wav")

# Helper function to play a sound
def play(filename):
    if (os.name == "nt"):
        winsound.PlaySound(filename, winsound.SND_FILENAME | winsound.SND_ASYNC)
    else:
        command = ["aplay", filename]
        proc = subprocess.Popen(command, stdout=subprocess.PIPE, stderr=subprocess.PIPE, bufsize=0, universal_newlines = True)
        proc.wait()

# Helper function to decide what action to take when we detect a group
def take_action(group):
    if group == "Dog":
        # A prediction in the dog category group was detected, play a `woof` sound
        play(woofSound)
    elif group == "Cat":
        # A prediction in the cat category group was detected, play a `meow` sound
        play(meowSound)

def main(args):
    if (len(args) < 1):
        print("usage: python pets.py categories.txt")
        exit()

    # Open the video camera. To use a different camera, change the camera index.
    camera = cv2.VideoCapture(0)

    # Read the category labels
    categories = get_categories_from_file(args[0])
    dogs = get_categories_from_file("dogLabels.txt")
    cats = get_categories_from_file("catLabels.txt")

    # Get the model's input dimensions. We'll use this information later to resize images appropriately.
    inputShape = model.get_default_input_shape()

    # Create a vector to hold the model's output predictions
    outputShape = model.get_default_output_shape()
    predictions = model.FloatVector(outputShape.Size())

    # Declare variables to hold state that we use to ensure we play sounds on new recognition
    lastHist = None
    significantDiff = 5000
    lastPredictionTime = 0
    headerText = ""

    while (cv2.waitKey(1) == 0xFF):
        # Get an image from the camera. If you'd like to use a different image, load the image from some other source.
        image = get_image_from_camera(camera)

        # Prepare the image to pass to the model. This helper:
        # - crops and resizes the image maintaining proper aspect ratio
        # - reorders the image channels if needed
        # - returns the data as a ravelled numpy array of floats so it can be handed to the model
        input = helpers.prepare_image_for_model(image, inputShape.columns, inputShape.rows)

        # Get a histogram using OpenCV as a quick way to detect whether the image has changed significantly
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
            # Get the predicted classes using the model's predict function on the image input data. 
            # The predictions are returned as a vector with the probability that the image
            # contains the class represented by that index.
            model.predict(input, predictions)

            # Let's grab the value of the top prediction and its index, which represents the top most 
            # confident match and the class or category it belongs to.
            topN = helpers.get_top_n_predictions(predictions, 1)

            # See whether the prediction is in one of our groups
            group = ""
            caption = ""
            label = ""
            if len(topN) > 0:
                top = topN[0]
                label = categories[top[0]]
                if label_in_set(label, dogs):
                    group = "Dog"
                elif label_in_set(label, cats):
                    group = "Cat"
            
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
            
        if now - lastPredictionTime > 2:
            # Reset the header text
            headerText = ""

        helpers.draw_header(image, headerText)
        # Display the image using opencv
        cv2.imshow('Grouping', image)

if __name__ == "__main__":
    args = sys.argv
    args.pop(0)
    main(args)
