####################################################################################################
##
##  Project:  Embedded Learning Library (ELL)
##  File:     tutorialHelpers.py
##  Authors:  Chris Lovett
##            Byron Changuion
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

script_path = os.path.dirname(os.path.abspath(__file__))
sys.path.append(script_path)
sys.path.append(os.path.join(script_path, 'build'))
sys.path.append(os.path.join(script_path, 'build/Release'))

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

def get_top_n(predictions, N = 5, threshold = 0.20):
    """Return at most the top N predictions as a list of tuples that meet the threshold.
       The first of element of each tuple represents the index or class of the prediction and the second 
       element represents that probability or confidence value.
    """
    map = [(i,predictions[i]) for i in range(len(predictions)) if predictions[i] >= threshold]
    map.sort(key=lambda tup: tup[1], reverse=True)
    result = map[:N]
    return result

def get_mean_duration(accumulated, duration, maxAccumulatedEntries = 30):
    """ Add a duration to an array and calculate the mean duration. """

    accumulated.append(duration)
    if (len(accumulated) > maxAccumulatedEntries):
        accumulated.pop(0)
    durations = np.array(accumulated)
    mean = np.mean(durations)
    return mean

def draw_header(image, text):
    """Helper to draw header text block onto an image"""
    draw_text_block(image, text, (0, 0), (50, 200, 50))
    return

def draw_footer(image, text):
    """Helper to draw footer text block onto an image"""
    draw_text_block(image, text, (0, image.shape[0] - 40), (200, 100, 100))
    return

def draw_text_block(image, text, blockTopLeft=(0,0), blockColor=(50, 200, 50), blockHeight=40):
    """Helper to draw a filled rectangle with text onto an image"""
    fontScale = 0.7
    cv2.rectangle(
        image, blockTopLeft, (image.shape[1], blockTopLeft[1] + blockHeight), blockColor, cv2.FILLED)
    cv2.putText(image, text, (blockTopLeft[0] + int(blockHeight / 4), blockTopLeft[1] + int(blockHeight * 0.667)),
                    cv2.FONT_HERSHEY_COMPLEX_SMALL, fontScale, (0, 0, 0), 1, cv2.LINE_AA)

class TiledImage:
    def __init__(self, numImages=2, outputHeightAndWidth=(600, 800)):
        """ Helper class to create a tiled image out of many smaller images.
        The class calculates how many horizontal and vertical blocks are needed to fit the requested number of images 
        and fills in unused blocks as blank. For example, to fit 4 images, the number of tiles is 2x2, to fit 5 images,
        the number of tiles is 3x2, with the last tile being blank.
        numImages - the maximum number of images that need to be composed into the tiled image. Note that the
                    actual number of tiles is equal to or larger than this number.
        outputHeightAndWidth - a list of two values giving the rows and columns of the output image. The output tiled image 
                            is a composition of sub images.
        """
        self.composed_image_shape = self.get_composed_image_shape(numImages)
        self.number_of_tiles = self.composed_image_shape[0] * self.composed_image_shape[1]
        self.output_height_and_width = outputHeightAndWidth
        self.images = None
        self.window_name = 'ELL side by side'
        cv2.namedWindow(self.window_name, cv2.WINDOW_NORMAL) # Ensure the window is resizable
        # The aspect ratio of the composed image is now self.composed_image_shape[0] : self.composed_image_shape[1]
        # Adjust the height of the window to account for this, else images will look distorted
        cv2.resizeWindow(self.window_name, outputHeightAndWidth[1], int(outputHeightAndWidth[0] * (self.composed_image_shape[0] / self.composed_image_shape[1])))

    def get_composed_image_shape(self, numImages):
        """Returns a tuple indicating the (rows,cols) of the required number of tiles to hold numImages."""
        # Split the image horizontally
        numHorizontal = math.ceil(math.sqrt(numImages))
        # Split the image vertically
        numVertical = math.ceil(numImages / numHorizontal)

        return (numVertical, numHorizontal)

    def resize_to_same_height(self, images):
        minHeight = min([i.shape[0] for i in images])
        for i in range(len(images)):
            shape = images[i].shape
            h = shape[0]
            if h > minHeight:
                scale = minHeight / h
                newSize = (int(shape[1] * scale), int(shape[0] * scale))
                images[i] = cv2.resize(images[i], newSize)
        return images

    def compose(self):
        """Composes an image made by tiling all the sub-images set with `set_image_at`. """
        yElements = []
        for verticalIndex in range(self.composed_image_shape[0]):
            xElements = []
            for horizontalIndex in range(self.composed_image_shape[1]):
                currentIndex = verticalIndex * self.composed_image_shape[1] + horizontalIndex
                xElements.append(self.images[currentIndex])
            # np.hstack only works if the images are the same height 
            xElements = self.resize_to_same_height(xElements)
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

    def set_image_at(self, imageIndex, frame):
        """Sets the image at the specified index. Once all images have been set, the tiled image result can be retrieved with `compose`."""
        # Ensure self.images is initialized.
        if self.images is None:
            self.images = [None] * self.number_of_tiles
            for i in range(self.number_of_tiles):
                self.images[i] = np.zeros((frame.shape), np.uint8)

        # Update the image at the specified index
        if (imageIndex < self.number_of_tiles):
            self.images[imageIndex] = frame
            return True
        return False

    def show(self):
        """Shows the final result of the tiled image. Returns True if the user indicates they are done viewing by pressing `Esc`. """
        # Compose the tiled image
        imageToShow = self.compose()
        # Show the tiled image
        cv2.imshow(self.window_name, imageToShow)
