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
import numpy as np

# Add the appropriate paths so the wrapped model can be loaded.
# Typically, that would be the path of the wrapped model.py file, and the corresponding
# native module, typically in build/release
os.path.dirname(os.path.abspath(__file__))
sys.path.append(os.getcwd())
sys.path.append(os.path.join(os.getcwd(), 'build'))
sys.path.append(os.path.join(os.getcwd(), 'build/Release'))

# Load the wrapped model's Python module
import model

# Get the input and output shapes
input_shape = model.get_default_input_shape()
output_shape = model.get_default_output_shape()

print("Model input shape: " + str([input_shape.rows,input_shape.columns,input_shape.channels]))
print("Model output shape: " + str([output_shape.rows,output_shape.columns,output_shape.channels]))

# Create a blank input of the appropriate size
input = model.FloatVector(input_shape.Size())

# Create a blank output of the appropriate size to hold the prediction results
predictions = model.FloatVector(output_shape.Size())

# Send the input to the predict function and get the prediction result
model.predict(input, predictions)

# Print the indix of the highest confidence prediction
print(np.argmax(predictions))

