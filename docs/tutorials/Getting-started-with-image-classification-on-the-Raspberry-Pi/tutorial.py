#!/usr/bin/env python3
###############################################################################
#
#  Project:  Embedded Learning Library (ELL)
#  File:     tutorial.py
#  Authors:  Byron Changuion
#
#  Requires: Python 3.x
#
###############################################################################

import time
import cv2

# Import helper functions
import tutorial_helpers as helpers

# Import the Python wrapper for the ELL model
import model


def get_image_from_camera(camera):
    """Read an image from the camera"""
    if camera:
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

    # Get the model's input shape. We will use this information later to resize
    # images appropriately.
    input_shape = model.get_default_input_shape()

    # Get the model's output shape and create an array to hold the model's
    # output predictions
    output_shape = model.get_default_output_shape()
    predictions = model.FloatVector(output_shape.Size())

    # Declare a variable to hold the prediction times
    prediction_times = []
    mean_time_to_predict = 0.0

    while (cv2.waitKey(1) & 0xFF) == 0xFF:
        # Get an image from the camera.
        image = get_image_from_camera(camera)

        # Prepare an image for processing
        # - Resize and center-crop to the required width and height while
        #   preserving aspect ratio.
        # - OpenCV gives the image in BGR order. If needed, re-order the
        #   channels to RGB.
        # - Convert the OpenCV result to a std::vector<float>
        input_data = helpers.prepare_image_for_model(
            image, input_shape.columns, input_shape.rows)

        # Send the image to the compiled model and fill the predictions vector
        # with scores, measure how long it takes
        start = time.time()
        model.predict(input_data, predictions)
        end = time.time()

        # Get the value of the top 5 predictions
        top_5 = helpers.get_top_n(predictions, 5)

        # Generate header text that represents the top5 predictions
        header_text = ", ".join(["({:.0%}) {}".format(
            element[1], categories[element[0]]) for element in top_5])
        helpers.draw_header(image, header_text)

        # Generate footer text that represents the mean evaluation time
        mean_time_to_predict = helpers.get_mean_duration(
            prediction_times, end - start)
        footer_text = "{:.0f}ms/frame".format(mean_time_to_predict * 1000)
        helpers.draw_footer(image, footer_text)

        # Display the image
        cv2.imshow("ELL model", image)

    print("Mean prediction time: {:.0f}ms/frame".format(
        mean_time_to_predict * 1000))

if __name__ == "__main__":
    main()
