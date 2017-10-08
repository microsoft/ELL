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

scriptPath = os.path.dirname(os.path.abspath(__file__))
sys.path.append(scriptPath)
sys.path.append(os.path.join(scriptPath, 'build'))
sys.path.append(os.path.join(scriptPath, 'build/Release')) # required for Windows

# Import the Python wrapper for the ELL model
import model

# Import helper functions
import tutorialHelpers as helpers

# Read an image from the camera
def get_image_from_camera(camera):
    if camera is not None:
        ret, frame = camera.read()
        if (not ret):
            raise Exception('your capture device is not returning images')
        return frame
    return None

def main():
    # Open the video camera. To use a different camera, change the camera index.
    camera = cv2.VideoCapture(0)

    # Read the category names
    categories = open('categories.txt', 'r').readlines()

    # Get the model's input shape. We will use this information later to resize images appropriately.
    input_shape = model.get_default_input_shape()

    # Get the model's output shape and create an array to hold the model's output predictions
    output_shape = model.get_default_output_shape()
    predictions = model.FloatVector(output_shape.Size())

    # Declare a variable to hold the prediction times
    predictionTimes = []
    meanTimeToPredict = 0.0

    while (cv2.waitKey(1) == 0xFF):
        # Get an image from the camera. 
        image = get_image_from_camera(camera)

        # Prepare an image for processing
        # - Resize and center-crop to the required width and height while preserving aspect ratio.
        # - OpenCV gives the image in BGR order. If needed, re-order the channels to RGB.
        # - Convert the OpenCV result to a std::vector<float>
        input = helpers.prepare_image_for_model(image, input_shape.columns, input_shape.rows)

        # Invoke the model, measure how long it takes
        start = time.time()
        model.predict(input, predictions)
        end = time.time()

        # Get the value of the top 5 predictions 
        top5 = helpers.get_top_n(predictions, 5)

        # Generate header text that represents the top5 predictions
        headerText = "".join(["(" + str(int(element[1]*100)) + "%) " + categories[element[0]] for element in top5])
        helpers.draw_header(image, headerText)

        # Generate footer text that represents the mean evaluation time
        meanTimeToPredict = helpers.get_mean_duration(predictionTimes, end - start)
        footerText = '{:.0f}'.format(meanTimeToPredict * 1000) + 'ms/frame'
        helpers.draw_footer(image, footerText)

        # Display the image
        cv2.imshow('ELL model', image)

    print('Mean prediction time: ' + '{:.0f}'.format(meanTimeToPredict) + 'ms/frame')

if __name__ == "__main__":
    main()
