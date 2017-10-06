####################################################################################################
##
##  Project:  Embedded Learning Library (ELL)
##  File:     callModel.py
##  Authors:  Byron Changuion
##
##  Requires: Python 3.x
##
####################################################################################################

import sys
import os
import cv2
import numpy as np

# Add the appropriate paths so the wrapped model can be loaded.
# Typically, that would be the path of the wrapped model.py file, and the corresponding
# native module, typically in build/release
scriptPath = os.path.dirname(os.path.abspath(__file__))
sys.path.append(scriptPath)
sys.path.append(os.path.join(scriptPath, 'build'))
sys.path.append(os.path.join(scriptPath, 'build/Release'))

# Load the wrapped model's Python module
import model

def prepare_image_for_model(image, requiredWidth, requiredHeight, reorderToRGB = False):
    """ Prepare an image for use with a model. Typically, this involves:
        - Resize and center crop to the required width and height while preserving the image's aspect ratio.
          Simple resize may result in a stretched or squashed image which will affect the model's ability
          to classify images.
        - OpenCV gives the image in BGR order, so we may need to re-order the channels to RGB.
        - Convert the OpenCV result to a std::vector<float> for use with ELL model
    """
    if image.shape[0] > image.shape[1]:  # Tall (more rows than cols)
        rowStart = int((image.shape[0] - image.shape[1]) / 2)
        rowEnd = rowStart + image.shape[1]
        colStart = 0
        colEnd = image.shape[1]
    else:  # Wide (more cols than rows)
        rowStart = 0
        rowEnd = image.shape[0]
        colStart = int((image.shape[1] - image.shape[0]) / 2)
        colEnd = colStart + image.shape[0]
    # Center crop the image maintaining aspect ratio
    cropped = image[rowStart:rowEnd, colStart:colEnd]
    # Resize to model's requirements
    resized = cv2.resize(cropped, (requiredHeight, requiredWidth))
    # Re-order if needed
    if not reorderToRGB:
        resized = cv2.cvtColor(resized, cv2.COLOR_BGR2RGB)
    # Return as a vector of floats
    result = resized.astype(np.float).ravel()
    return result

# Get the input and output shapes
input_shape = model.get_default_input_shape()
output_shape = model.get_default_output_shape()

print("Model input shape: " + str([input_shape.rows,input_shape.columns,input_shape.channels]))
print("Model output shape: " + str([output_shape.rows,output_shape.columns,output_shape.channels]))

# Create a blank output of the appropriate size to hold the prediction results
predictions = model.FloatVector(output_shape.Size())

# Read in the sample image
image = cv2.imread("coffeemug.jpg")

# Prepare the image to send to the model
input = prepare_image_for_model(image, input_shape.columns, input_shape.rows)

# Send the input to the predict function and get the prediction result
model.predict(input, predictions)

# Print the index of the highest confidence prediction
predictionIndex = int(np.argmax(predictions))
print("Category index: " + str(predictionIndex))
print("Confidence: " + str(predictions[predictionIndex]))

