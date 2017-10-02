####################################################################################################
##
##  Project:  Embedded Learning Library (ELL)
##  File:     tutorial.py
##  Authors:  Byron Changuion
##
##  Requires: Python 3.x
##
####################################################################################################

import sys
import os
import time
import numpy as np
import cv2

# Include a helper that has useful functions such as preparing data for the model
import tutorialHelpers as helpers

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

def main(args):
    if (len(args) < 1):
        print("usage: python tutorial.py categories.txt")
        exit()
    
    # Open the video camera. To use a different camera, change the camera index.
    camera = cv2.VideoCapture(0)

    # Read the category labels
    categories = get_categories_from_file(args[0])

    # Get the model's input dimensions. We'll use this information later to resize images appropriately.
    inputShape = model.get_default_input_shape()

    # Create a vector to hold the model's output predictions
    outputShape = model.get_default_output_shape()
    predictions = model.FloatVector(outputShape.Size())

    # Declare a value to hold the prediction times
    predictionTimes = []
    meanTimeToPredict = 0.0

    while (cv2.waitKey(1) == 0xFF):
        # Get an image from the camera. If you'd like to use a different image, load the image from some other source.
        image = get_image_from_camera(camera)

        # Prepare the image to pass to the model. This helper:
        # - crops and resizes the image maintaining proper aspect ratio
        # - reorders the image channels if needed
        # - returns the data as a ravelled numpy array of floats so it can be handed to the model
        input = helpers.prepare_image_for_model(image, inputShape.columns, inputShape.rows)

        # Get the predicted classes using the model's predict function on the image input data. 
        # The predictions are returned as a vector with the probability that the image
        # contains the class represented by that index.
        start = time.time()
        model.predict(input, predictions)
        end = time.time()

        # Let's grab the value of the top 5 predictions and their index, which represents the top five most 
        # confident matches and the class or category they belong to.
        top5 = helpers.get_top_n_predictions(predictions, 5)

        # Draw header text that represents the top5 predictions
        headerText = "".join(["(" + str(int(element[1]*100)) + "%) " + categories[element[0]] for element in top5])
        helpers.draw_header(image, headerText)

        # Draw footer text representing the mean evaluation time
        meanTimeToPredict = helpers.get_mean_duration(predictionTimes, end - start)
        footerText = '{:.0f}'.format(meanTimeToPredict * 1000) + 'ms/frame'
        helpers.draw_footer(image, footerText)

        # Display the image using opencv
        cv2.imshow('ELL model', image)

    print('Mean time to predict: ' + '{:.0f}'.format(meanTimeToPredict) + 'ms/frame')

if __name__ == "__main__":
    args = sys.argv
    args.pop(0)
    main(args)
