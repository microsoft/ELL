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

import cv2
import numpy as np

# Import helper functions
import tutorial_helpers as helpers

# Load the wrapped model's Python module
import model

# Get the model wrapper in order to interact with the model
model_wrapper = model.ModelWrapper()

# Get the input and output shapes
input_shape = model_wrapper.GetInputShape()
output_shape = model_wrapper.GetOutputShape()

# Get the model-specific preprocessing metadata
preprocessing_metadata = helpers.get_image_preprocessing_metadata(model_wrapper)

print("Model input shape: [{0.rows}, {0.columns}, {0.channels}]".format(
    input_shape))
print("Model output shape: [{0.rows}, {0.columns}, {0.channels}]".format(
    output_shape))

# Read in the sample image
sample_image = cv2.imread("coffeemug.jpg")

# Prepare the image to send to the model
input_data = helpers.prepare_image_for_model(sample_image, input_shape.columns,
                                             input_shape.rows, preprocessing_metadata=preprocessing_metadata)

# Wrap the resulting numpy array in a FloatVector
input_data = model.FloatVector(input_data)

# Send the input to the predict function and get the prediction result
predictions = model.predict(input_data)

# Print the index of the highest confidence prediction
prediction_index = int(np.argmax(predictions))
print("Category index: {}".format(prediction_index))
print("Confidence: {}".format(predictions[prediction_index]))
