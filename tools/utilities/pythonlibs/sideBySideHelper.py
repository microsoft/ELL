####################################################################################################
##
##  Project:  Embedded Learning Library (ELL)
##  File:     sideBySideHelper.py
##  Authors:  Byron Changuion
##
##  Requires: Python 3.x
##
####################################################################################################

import os
import sys
import argparse
import cv2
import numpy as np
import time
import math
import demoHelper

class SideBySideHelper:
    def __init__(self, demoHelpers=[None], outputHeightAndWidth=(600, 800)):
        """ Helper class to run models in a round robin and compare mean time per frame.
        demoHelpers - a list of DemoHelper instances, each representing a model to be evaluated,
        outputHeightAndWidth - a list of two values giving the rows and columns of the output image. This image 
                            is a composition of up to 4 images, each produced from a ModelHelper instance.
        """
        
        self.composed_image_shape = self.get_composed_image_shape(len(demoHelpers))
        self.number_of_tiles = self.composed_image_shape[0] * self.composed_image_shape[1]
        self.model_helpers = [None] * self.number_of_tiles
        for helperIndex in range(len(self.model_helpers)):
            if helperIndex < len(demoHelpers):
                self.model_helpers[helperIndex] = demoHelpers[helperIndex]
        self.output_height_and_width = outputHeightAndWidth
        self.evaluation_times = [[] for _ in range(self.number_of_tiles)]
        self.save_images = None
        self.window_name = 'ell_sideBySide'
        self.previous_images = None
        self.current_position = 0
        self.stop = False
        cv2.namedWindow(self.window_name, cv2.WINDOW_NORMAL) # Ensure the window is resizable
        # The aspect ratio of the composed image is now self.composed_image_shape[0] : self.composed_image_shape[1]
        # Adjust the height of the window to account for this, else images will look distorted
        cv2.resizeWindow(self.window_name, outputHeightAndWidth[1], int(outputHeightAndWidth[0] * (self.composed_image_shape[0] / self.composed_image_shape[1])))
    
    def get_composed_image_shape(self, numberOfModels):
        """ Returns a tuple indicating the (rows,cols) of the image composed of
            all the model's results """
        # Split the image horizontally
        numHorizontal = math.ceil(math.sqrt(numberOfModels))
        # Split the image vertically
        numVertical = math.ceil(numberOfModels / numHorizontal)

        return (numVertical, numHorizontal)
                
    def compose_model_frame(self, modelHelperNumber, frame):
        """ Runs the frame through a specific modelHelper instance, returns a frame showing results for that model. """
        modelHelper = self.model_helpers[modelHelperNumber]

        # Prepare the image to send to the model.
        # This involves scaling to the required input dimension and re-ordering from BGR to RGB
        data = modelHelper.prepare_image_for_predictor(frame)

        # Get the compiled model to classify the image, by returning a list of probabilities for the classes it can detect
        # Run through the frame twice, only measuring the second one. This helps get a more accurate reading since
        # the model would be loaded.
        startTime = time.clock()
        modelHelper.predict(data)
        endTime = time.clock()

        self.evaluation_times[modelHelperNumber].append((endTime - startTime) * 1000) # time in ms

        if (len(self.evaluation_times[modelHelperNumber]) > 10):
            self.evaluation_times[modelHelperNumber].pop(0)

        # Get the (at most) top 5 predictions that meet our threshold. This is returned as a list of tuples,
        # each with the text label and the prediction score.
        top5 = modelHelper.get_top_n(modelHelper.results, 5)

        # Turn the top5 into a text string to display
        header_text = "".join(
            [modelHelper.get_label(element[0]) + "(" + str(int(100 * element[1])) + "%)  " for element in top5])

        # Draw the text on the frame
        frameToShow = np.copy(frame)
        modelHelper.draw_header(frameToShow, header_text)

        # Calculate mean evaluation time per frame
        if (len(self.evaluation_times[modelHelperNumber]) > 0):
            meanEvaluationTime = sum(self.evaluation_times[modelHelperNumber]) / len(self.evaluation_times[modelHelperNumber])
            # Draw the footer text on the frame
            footerText = self.model_helpers[modelHelperNumber].model_name + ', ' + '{:.0f}'.format(meanEvaluationTime) + 'ms/frame'
            modelHelper.draw_footer(frameToShow, footerText)

        return frameToShow

    def show_image(self, imageToShow):
        cv2.imshow(self.window_name, imageToShow)
        if (not self.save_images is None):
            name = self.window_name + str(self.save_images) + ".png"
            cv2.imwrite(name, imageToShow)
            self.save_images = self.save_images + 1
    
    def compose_image(self, frames):
        """ Composes an image made of tiles from each model helper into a single image which is returned. """
        yElements = []
        for verticalIndex in range(self.composed_image_shape[0]):
            xElements = []
            for horizontalIndex in range(self.composed_image_shape[1]):
                currentIndex = verticalIndex * self.composed_image_shape[1] + horizontalIndex
                xElements.append(frames[currentIndex])
            horizontalImage = np.hstack(tuple(xElements))
            yElements.append(horizontalImage)
        composedImage = np.vstack(tuple(yElements))

        # Draw separation lines
        yStep = int(composedImage.shape[0] / self.composed_image_shape[0])
        xStep = int(composedImage.shape[1] / self.composed_image_shape[1])
        y = yStep
        x = xStep
        for horizontalIndex in range(1, self.composed_image_shape[1]):
            cv2.line(composedImage, (x, 0), (x, composedImage.shape[0]), (0, 0, 0), 3)
            x = x + xStep
        for verticalIndex in range(1, self.composed_image_shape[0]):
            cv2.line(composedImage, (0, y), (composedImage.shape[1], y), (0, 0, 0), 3)
            y = y + yStep
        
        return composedImage

    def process_frame(self, frame):
        """ Runs the frame through each model helper, computes the mean evaluation time,
            and shows a tiled image composed from each model. """

        if self.previous_images is None:
            self.previous_images = [None] * self.number_of_tiles
            for helperIndex in range(len(self.model_helpers)):
                self.previous_images[helperIndex] = np.zeros((frame.shape), np.uint8)

        # Run through models in random order to get a fairer average of evaluation time
        modelIndex = np.arange(len(self.model_helpers))
        np.random.shuffle(modelIndex)

        for helperIndex in modelIndex:
            if self.stop:
                break
            if (self.model_helpers[helperIndex] is not None):
                self.previous_images[helperIndex] = self.compose_model_frame(helperIndex, frame)
                imageToShow = self.compose_image(self.previous_images)
                self.show_image(imageToShow)
                if cv2.waitKey(1) & 0xFF == 27:
                    self.stop = True
                    break
        return (not self.stop)

    def get_next_frame(self):
        if len(self.model_helpers) > 0:
            return self.model_helpers[0].get_next_frame()
        return None        

    def done(self):
        if (not self.stop) and (len(self.model_helpers) > 0):
            return self.model_helpers[0].done()
        return True
            
