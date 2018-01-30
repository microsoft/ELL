#!/usr/bin/env python3
###############################################################################
#
#  Project:  Embedded Learning Library (ELL)
#  File:     pets_callback.py
#  Authors:  Chris Lovett
#            Byron Changuion
#            Lisa Ong
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


class CatsDogsPredictor(model.Model):
    """Class that implements input and output callbacks for the ELL model
    by deriving from the Model base class.
    """
    def __init__(self, camera, cats, dogs):
        """Initializes this object with the camera source and model-related
        information"""
        model.Model.__init__(self)

        self.camera = camera
        self.dogs = dogs
        self.cats = cats

        # Get the model's input dimensions. We'll use this information later to
        # resize images appropriately.
        self.input_shape = model.get_default_input_shape()

        # Holds the image from the camera or other sources
        self.image = None

    def input_callback(self):
        """The input callback that returns an image to the model"""

        # Get an image from the camera. If you'd like to use a different image,
        # load the image from some other source.
        self.image = get_image_from_camera(self.camera)

        # Prepare the image to pass to the model. This helper:
        # - crops and resizes the image maintaining proper aspect ratio
        # - reorders the image channels if needed
        # - returns the data as a ravelled numpy array of floats so it can be
        # handed to the model
        return helpers.prepare_image_for_model(
            self.image, self.input_shape.columns, self.input_shape.rows)

    def output_callback(self, predictions):
        """The output callback that the model calls when predictions are ready"""

        header_text = ""
        group, probability = self.get_group(predictions)

        if group:
            # A group was detected, so take action
            if group == "Dog":
                # A prediction in the dog category group was detected, print a `woof`
                print("Woof!")
            elif group == "Cat":
                # A prediction in the cat category group was detected, print a `meow`
                print("Meow!")
            header_text = "({:.0%}) {}".format(probability, group)

        helpers.draw_header(self.image, header_text)
        cv2.imshow("Grouping (with callbacks)", self.image)

    def get_group(self, predictions):
        """Returns the group and proability for the top prediction"""
        top_n = helpers.get_top_n(predictions, 1, threshold=0.05)

        if top_n:
            top = top_n[0]
            if prediction_index_in_set(top[0], self.dogs):
                return "Dog", top[1]
            elif prediction_index_in_set(top[0], self.cats):
                return "Cat", top[1]

        return None


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

    # Create a predictor
    predictor = CatsDogsPredictor(camera, cats, dogs)

    while (cv2.waitKey(1) & 0xFF) == 0xFF:

        # Run the predictor. The ELL model will call the input callback
        # to get an image, and call the output callback when predictions
        # are available
        predictor.predict()

if __name__ == "__main__":
    main()
