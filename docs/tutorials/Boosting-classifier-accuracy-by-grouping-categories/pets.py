#!/usr/bin/env python3
###############################################################################
#
#  Project:  Embedded Learning Library (ELL)
#  File:     pets.py
#  Authors:  Chris Lovett
#            Byron Changuion
#
#  Requires: Python 3.x
#
###############################################################################

import cv2
import tutorial_helpers as helpers

# import the ELL model's Python module
import model


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


def prediction_index_in_set(prediction_index, category_set):
    """Returns True if the prediction index is in the set"""
    for x in category_set:
        if prediction_index == int(x):
            return True
    return False


def take_action(group):
    """Helper function to decide what action to take when we detect a group"""
    if group == "Dog":
        # A prediction in the dog category group was detected, print a `woof`
        print("Woof!")
    elif group == "Cat":
        # A prediction in the cat category group was detected, print a `meow`
        print("Meow!")


def main():
    """Entry point for the script when called directly"""
    # Open the video camera. To use a different camera, change the camera
    # index.
    camera = cv2.VideoCapture(0)

    # Read the category names
    with open("dogs.txt", "r") as dogs_file,\
            open("cats.txt", "r") as cats_file:
        dogs = dogs_file.read().splitlines()
        cats = cats_file.read().splitlines()

    # Get the model's input dimensions. We'll use this information later to
    # resize images appropriately.
    input_shape = model.get_default_input_shape()

    while (cv2.waitKey(1) & 0xFF) == 0xFF:
        # Get an image from the camera. If you'd like to use a different image,
        # load the image from some other source.
        image = get_image_from_camera(camera)

        # Prepare the image to pass to the model. This helper:
        # - crops and resizes the image maintaining proper aspect ratio
        # - reorders the image channels if needed
        # - returns the data as a ravelled numpy array of floats so it can be
        # handed to the model
        input_data = helpers.prepare_image_for_model(
            image, input_shape.columns, input_shape.rows)

        # Get the predicted classes using the model's predict function on the
        # image input data. The predictions are returned as a numpy array with the
        # probability that the image # contains the class represented by that
        # index.
        predictions = model.predict(input_data)

        # Let's grab the value of the top prediction and its index, which
        # represents the top most confident match and the class or category it
        # belongs to.
        top_n = helpers.get_top_n(predictions, 1, threshold=0.05)

        # See whether the prediction is in one of our groups
        group = ""
        label = ""
        if top_n:
            top = top_n[0][0]
            if prediction_index_in_set(top, dogs):
                group = "Dog"
            elif prediction_index_in_set(top, cats):
                group = "Cat"

        header_text = ""
        if group:
            # A group was detected, so take action
            top = top_n[0]
            take_action(group)
            header_text = "({:.0%}) {}".format(top[1], group)

        helpers.draw_header(image, header_text)

        # Display the image using opencv
        cv2.imshow("Grouping", image)

if __name__ == "__main__":
    main()
