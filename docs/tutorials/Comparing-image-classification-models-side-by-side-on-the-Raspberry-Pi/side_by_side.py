#!/usr/bin/env python3
###############################################################################
#
#  Project:  Embedded Learning Library (ELL)
#  File:     side_by_side.py
#  Authors:  Byron Changuion
#
#  Requires: Python 3.x
#
###############################################################################

import time
import numpy as np
import cv2
import tutorial_helpers as helpers

import model1
import model2


def get_image_from_camera(camera):
    """Function to return an image from our camera using OpenCV"""
    if camera:
        # if predictor is too slow frames get buffered, this is designed to
        # flush that buffer
        ret, frame = camera.read()
        if not ret:
            raise Exception("your capture device is not returning images")
        return frame
    return None


def main():
    # Open the video camera. To use a different camera, change the camera
    # index.
    camera = cv2.VideoCapture(0)

    # Read the category names
    with open("categories.txt", "r") as categories_file:
        categories = categories_file.read().splitlines()

    # Define the models we'll be using
    models = [model1.model1, model2.model2]

    # Get the models' input dimensions. We'll use this information later to
    # resize images appropriately.
    input_shapes = [model.get_default_input_shape() for model in models]

    # Create an array to hold the models' output predictions
    prediction_arrays = [None, None]

    # Declare a value to hold the prediction times
    prediction_times = [list(), list()]
    mean_time_to_predict = [0.0, 0.0]

    # Declare a tiled image used to compose our results
    tiled_image = helpers.TiledImage(len(models))

    while (cv2.waitKey(1) & 0xFF) == 0xFF:
        # Get an image from the camera. If you'd like to use a different image,
        # load the image from some other source.
        image = get_image_from_camera(camera)

        # Run through models in random order to get a fairer average of
        # evaluation time
        model_indices = np.arange(len(models))
        np.random.shuffle(model_indices)

        for model_index in model_indices:
            model = models[model_index]

            # Prepare the image to pass to the model. This helper:
            # - crops and resizes the image maintaining proper aspect ratio
            # - reorders the image channels if needed
            # - returns the data as a ravelled numpy array of floats so it can
            # be handed to the model
            input_data = helpers.prepare_image_for_model(
                image, input_shapes[model_index].columns,
                input_shapes[model_index].rows)

            # Get the predicted classes using the model's predict function on
            # the image input data. The predictions are returned as a vector
            # with the probability that the image # contains the class
            # represented by that index.
            start = time.time()
            prediction_arrays[model_index] = model.predict(input_data)
            end = time.time()

            # Let's grab the value of the top 5 predictions and their index,
            # which represents the top five most confident matches and the
            # class or category they belong to.
            top_5 = helpers.get_top_n(
                prediction_arrays[model_index], n=5, threshold=0.10)

            # Draw header text that represents the top five predictions
            header_text = "".join(["({:.0%}) {}  ".format(
                element[1], categories[element[0]]) for element in top_5])
            model_frame = np.copy(image)
            helpers.draw_header(model_frame, header_text)

            # Draw footer text representing the mean evaluation time
            mean_time_to_predict[model_index] = helpers.get_mean_duration(
                prediction_times[model_index], end - start)
            footer_text = "{:.0f}ms/frame".format(
                mean_time_to_predict[model_index] * 1000)
            helpers.draw_footer(model_frame, footer_text)

            # Set the image with the header and footer text as one of the tiles
            tiled_image.set_image_at(model_index, model_frame)
            tiled_image.show()


if __name__ == "__main__":
    main()
