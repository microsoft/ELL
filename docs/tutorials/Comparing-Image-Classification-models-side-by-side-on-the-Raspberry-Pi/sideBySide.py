####################################################################################################
##
##  Project:  Embedded Learning Library (ELL)
##  File:     sideBySide.py
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
import tutorialHelpers as helpers

# Import models. Since they are contained in different directories, add the relative paths so Python can find them
sys.path.append("model1")
sys.path.append("model1/build/Release")
sys.path.append("model2")
sys.path.append("model2/build/Release")
import model1
import model2

# Function to return an image from our camera using OpenCV
def get_image_from_camera(camera):
    if camera is not None:
        # if predictor is too slow frames get buffered, this is designed to flush that buffer
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

    # Define the models we'll be using
    models = [model1, model2]    

    # Get the models' input dimensions. We'll use this information later to resize images appropriately.
    inputShapes = []
    inputShapes.append(models[0].get_default_input_shape())
    inputShapes.append(models[1].get_default_input_shape())

    # Create vectors to hold the models' output predictions
    predictionArrays = []
    outputShape = models[0].get_default_output_shape()
    predictionArrays.append(models[0].FloatVector(outputShape.Size()))
    outputShape = models[1].get_default_output_shape()
    predictionArrays.append(models[1].FloatVector(outputShape.Size()))

    # Declare a value to hold the prediction times
    predictionTimes = []
    predictionTimes.append([])
    predictionTimes.append([])
    meanTimeToPredict = [0.0, 0.0]

    # Declare a tiled image used to compose our results
    tiledImage = helpers.TiledImage(len(models))
    
    while (cv2.waitKey(1) == 0xFF):
        # Get an image from the camera. If you'd like to use a different image, load the image from some other source.
        image = get_image_from_camera(camera)

        # Run through models in random order to get a fairer average of evaluation time
        modelIndexes = np.arange(len(models))
        np.random.shuffle(modelIndexes)

        for modelIndex in modelIndexes:
            model = models[modelIndex]

            # Prepare the image to pass to the model. This helper:
            # - crops and resizes the image maintaining proper aspect ratio
            # - reorders the image channels if needed
            # - returns the data as a ravelled numpy array of floats so it can be handed to the model
            input = helpers.prepare_image_for_model(image, inputShapes[modelIndex].columns, inputShapes[modelIndex].rows)

            # Get the predicted classes using the model's predict function on the image input data. 
            # The predictions are returned as a vector with the probability that the image
            # contains the class represented by that index.
            start = time.time()
            model.predict(input, predictionArrays[modelIndex])
            end = time.time()

            # Let's grab the value of the top 5 predictions and their index, which represents the top five most 
            # confident matches and the class or category they belong to.
            top5 = helpers.get_top_n(predictionArrays[modelIndex], 5)

            # Draw header text that represents the top5 predictions
            modelFrame = np.copy(image)
            headerText = "".join(["(" + str(int(element[1]*100)) + "%) " + categories[element[0]] + "  " for element in top5])
            helpers.draw_header(modelFrame, headerText)

            # Draw footer text representing the mean evaluation time
            meanTimeToPredict[modelIndex] = helpers.get_mean_duration(predictionTimes[modelIndex], end - start)
            footerText = '{:.0f}'.format(meanTimeToPredict[modelIndex] * 1000) + 'ms/frame'
            helpers.draw_footer(modelFrame, footerText)

            # Set the image with the header and footer text as one of the tiles
            tiledImage.set_image_at(modelIndex, modelFrame)
            tiledImage.show()


if __name__ == "__main__":
    main()
