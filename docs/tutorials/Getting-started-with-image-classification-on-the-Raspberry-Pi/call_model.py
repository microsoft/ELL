#!/usr/bin/env python3
###############################################################################
#
#  Project:  Embedded Learning Library (ELL)
#  File:     call_model.py
#  Authors:  Byron Changuion
#
#  Requires: Python 3.x
#
###############################################################################

import sys
import os
import cv2
import numpy as np

# Add the appropriate paths so the wrapped model can be loaded.
# Typically, that would be the path of the wrapped model.py file, and the
# corresponding native module, typically in build/release
SCRIPT_PATH = os.path.dirname(os.path.abspath(__file__))
sys.path.append(SCRIPT_PATH)
sys.path.append(os.path.join(SCRIPT_PATH, "build"))
sys.path.append(os.path.join(SCRIPT_PATH, "build/Release"))

# Load the wrapped model's Python module
import model


def prepare_image_for_model(image, width, height, reorder_to_rgb=False):
    """ Prepare an image for use with a model. Typically, this involves:
        - Resize and center crop to the required width and height while
        preserving the image's aspect ratio. Simple resize may result in a
        stretched or squashed image which will affect the model's ability to
        classify images.
        - OpenCV gives the image in BGR order, so we may need to re-order the
        channels to RGB.
        - Convert the OpenCV result to a std::vector<float> for use with ELL
        model
    """
    if image.shape[0] > image.shape[1]:  # Tall (more rows than cols)
        row_start = int((image.shape[0] - image.shape[1]) / 2)
        row_end = row_start + image.shape[1]
        col_start = 0
        col_end = image.shape[1]
    else:  # Wide (more cols than rows)
        row_start = 0
        row_end = image.shape[0]
        col_start = int((image.shape[1] - image.shape[0]) / 2)
        col_end = col_start + image.shape[0]
    # Center crop the image maintaining aspect ratio
    cropped = image[row_start:row_end, col_start:col_end]
    # Resize to model's requirements
    resized = cv2.resize(cropped, (height, ))
    # Re-order if needed
    if reorder_to_rgb:
        resized = cv2.cvtColor(resized, cv2.COLOR_BGR2RGB)
    # Return as a vector of floats
    result = resized.astype(np.float).ravel()
    return result

# Get the input and output shapes
input_shape = model.get_default_input_shape()
output_shape = model.get_default_output_shape()

print("Model input shape: [{0.rows}, {0.columns}, {0.channels}]".format(
    input_shape))
print("Model output shape: [{0.rows}, {0.columns}, {0.channels}]".format(
    output_shape))

# Create a blank output of the appropriate size to hold the predictions
predictions = model.FloatVector(output_shape.Size())

# Read in the sample image
sample_image = cv2.imread("coffeemug.jpg")

# Prepare the image to send to the model
input_data = prepare_image_for_model(sample_image, input_shape.columns,
                                     input_shape.rows)

# Send the input to the predict function and get the prediction result
model.predict(input_data, predictions)

# Print the index of the highest confidence prediction
prediction_index = int(np.argmax(predictions))
print("Category index: {}".format(prediction_index))
print("Confidence: {}".format(predictions[prediction_index]))
