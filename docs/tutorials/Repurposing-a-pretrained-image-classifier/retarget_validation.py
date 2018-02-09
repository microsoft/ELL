#!/usr/bin/env python3
###############################################################################
#
#  Project:  Embedded Learning Library (ELL)
#  File:     retarget_validation.py
#  Authors:  Byron Changuion
#
#  Requires: Python 3.x
#
###############################################################################

import time
import cv2
import sys
import os
import logging
import numpy as np
import tutorial_helpers as helpers

# Import the Python wrapper for the ELL model
sys.path.append(os.path.join(os.path.dirname(os.path.abspath(__file__)),
    '<ELL_root>/tools/utilities/pythonlibs'))
import retarget as model
import find_ell
import ell

def save_confusion_matrix(categories, confusion_matrix_filename,
        confusion_matrix, num_correct, num_total,
        mean_time_to_predict = None):
    """ Save a confusion matrix as a tab seperated file with headers. The rows
    are the predicted values, the columns are the expected values.
    """
    with open(confusion_matrix_filename, "w") as f:
        # Write the header
        f.write("\t")
        f.write("\t".join(categories))
        f.write("\n")
        # Write the rows
        for i in range(confusion_matrix.shape[0]):
            f.write("{}\t".format(categories[i]))
            for value in confusion_matrix[i]:
                f.write("{}\t".format(int(value)))
            f.write("\n")
        f.write("\n")
        f.write("\n")
        f.write("\n")
        if mean_time_to_predict:
            f.write("Mean prediction time:\t\t\t {:.0f}ms/frame\n".format(
                mean_time_to_predict * 1000))
        f.write("Accuracy:\t\t\t {}/{} = {:.1%}%\n".format(num_correct,
            num_total, num_correct / num_total))

def main(args):
    """Entry point for the script when called directly"""
    categories_filename = 'categories.txt'
    validation_filename = 'fruit_validate.gsdf'
    confusion_matrix_filename = 'confusion_matrix.txt'

    # Read the category names
    with open(categories_filename, "r") as categories_file:
        categories = categories_file.read().splitlines()

    # Get the model's input shape. We will use this information later to resize
    # images appropriately.
    input_shape = model.get_default_input_shape()

    # Get the model's output shape and create an array to hold the model's
    # output predictions
    output_shape = model.get_default_output_shape()

    # Declare a variable to hold the prediction times
    prediction_times = []
    mean_time_to_predict = 0.0

    print("Loading dataset")
    dataset = ell.data.AutoSupervisedDataset()
    dataset.Load(validation_filename)
    num = dataset.NumExamples()
    print("Number of Examples:", num)
    features = dataset.NumFeatures()
    print("Number of Features:", features)

    num_classes = len(categories)
    confusion_matrix = np.zeros((num_classes,num_classes), dtype=np.int32)

    num_correct = 0
    for i in range(num):
        example = dataset.GetExample(i)
        av = example.GetData()

        x = np.asarray(av.ToArray()).astype(np.float_, copy=False)

        start = time.time()
        predictions = model.predict(x)
        end = time.time()
        mean_time_to_predict = helpers.get_mean_duration(
            prediction_times, end - start)
        predicted = np.argmax(predictions)
        expected = int(example.GetLabel())
        print("Predict {}, expected {}".format(predicted, expected))
        confusion_matrix[predicted, expected] += 1
        if predicted == expected:
            num_correct += 1

    print("Mean prediction time: {:.0f}ms/frame".format(
        mean_time_to_predict * 1000))
    print("Accuracy: {}/{} = {:.1%}%".format(num_correct, num,
        num_correct / num))

    save_confusion_matrix(categories, confusion_matrix_filename,
        confusion_matrix, num_correct, num, mean_time_to_predict)

if __name__ == "__main__":
    logging.basicConfig(level=logging.DEBUG)
    logger = logging.getLogger(__name__)
    try:
        main(sys.argv[1:])
    except Exception as e:
        logger.exception(e)
