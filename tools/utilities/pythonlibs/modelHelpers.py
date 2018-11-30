####################################################################################################
##
##  Project:  Embedded Learning Library (ELL)
##  File:     modelHelpers.py
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


def prepare_image_for_model(image, requiredWidth, requiredHeight,
                            reorder_to_rgb=False, convert_to_float=True):
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
    if reorder_to_rgb:
        resized = cv2.cvtColor(resized, cv2.COLOR_BGR2RGB)
    if convert_to_float:
        # Return as a vector of floats
        result = resized.astype(np.float).ravel()
    else:
        result = resized.ravel()
    return result

def get_top_n_predictions(predictions, N = 5, threshold = 0.20):
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
