import sys
import os
import numpy as np
import cv2

import findEll
import darknet_to_ell
import modelHelper as mh

def get_ell_predictor(helper):
    """Imports a model and returns an ELL.Predictor."""
    return darknet_to_ell.predictor_from_darknet_model(helper.model_files[0], helper.model_files[1])

def main():
    # Check for model files
    if (not os.path.exists('darknet.cfg')):
        print("Please download the 'darknet.cfg' file, see README.md")
        sys.exit(1)
        
    if (not os.path.exists('darknet.weights')):
        print("Please download the 'darknet.weights' file, see README.md")
        sys.exit(1)

    # Pick the model you want to work with
    helper = mh.ModelHelper(sys.argv, "darknetReference", ["darknet.cfg", "darknet.weights"], "darknetImageNetLabels.txt")

    # Import the model
    model = get_ell_predictor(helper)

    # Save the model
    helper.save_ell_predictor_to_file(model, "darknetReference.map")

    # Initialize image source
    helper.init_image_source()
    
    lastPrediction = ""

    while (not helper.done()):
        # Grab next frame
        frame = helper.get_next_frame()

        # Prepare the image to send to the model.
        # This involves scaling to the required input dimension and re-ordering from BGR to RGB
        data = helper.prepare_image_for_predictor(frame)

        # Get the model to classify the image, by returning a list of probabilities for the classes it can detect
        predictions = model.Predict(data)

        # Get the (at most) top 5 predictions that meet our threshold. This is returned as a list of tuples,
        # each with the text label and the prediction score.
        top5 = helper.get_top_n(predictions, 5)

        # Turn the top5 into a text string to display
        text = "".join([str(element[0]) + "(" + str(int(100*element[1])) + "%)  " for element in top5])

        if (text != lastPrediction):
            print(text)
            lastPrediction = text

        # Draw the text on the frame
        frameToShow = frame
        helper.draw_label(frameToShow, text)
        helper.draw_fps(frameToShow)

        # Show the new frame
        helper.show_image(frameToShow)


if __name__ == "__main__":
    main()