import sys
import os
import numpy as np
import cv2

import modelHelper as mh

# note: to run this in headless mode on a Linux machine run the following from your terminal window
# export DISPLAY=:0
# then add the '-save' argument to get tagged frames to be saved to disk.

# Import the compiled model wrapper
sys.path.append('build')
sys.path.append('build/Release')
import darknetReference as model

def main():
    # python somehow needs to know about the data vector type, so we provide it
    buffer = model.FloatVector(224 * 224 * 3)
    results = model.FloatVector(1000)

    # Pick the model you want to work with
    helper = mh.ModelHelper(sys.argv, "darknetReference", [
                            "darknet.cfg", "darknet.weights"], "darknetImageNetLabels.txt")

    # Initialize image source
    helper.init_image_source()

    lastPrediction = ""

    while (not helper.done()):
        # Grab next frame
        frame = helper.get_next_frame()

        # Prepare the image to send to the model.
        # This involves scaling to the required input dimension and re-ordering from BGR to RGB
        data = helper.prepare_image_for_predictor(frame)

        # Get the compiled model to classify the image, by returning a list of probabilities for the classes it can detect
        model.darknet_reference_predict(data, results)

        # Get the (at most) top 5 predictions that meet our threshold. This is returned as a list of tuples,
        # each with the text label and the prediction score.
        top5 = helper.get_top_n(results, 5)

        # Turn the top5 into a text string to display
        text = "".join(
            [str(element[0]) + "(" + str(int(100 * element[1])) + "%)  " for element in top5])

        if (text != lastPrediction):
            print(text)
            lastPrediction = text

        # Draw the text on the frame
        frameToShow = frame
        helper.draw_label(frameToShow, text)
        helper.draw_fps(frameToShow)

        # Show the new frame
        helper.show_image(frameToShow)


    # print profiling info if model is compiled with profiling on
    if hasattr(model, "darknet_reference_print_model_profiling_info"):
        print("model statistics:")
        model.darknet_reference_print_model_profiling_info()
    if hasattr(model, "darknet_reference_print_node_profiling_info"):
        print("node statistics:")
        model.darknet_reference_print_node_profiling_info()

if __name__ == "__main__":
    main()
