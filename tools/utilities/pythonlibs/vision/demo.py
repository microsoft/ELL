#!/usr/bin/env python3
####################################################################################################
##
##  Project:  Embedded Learning Library (ELL)
##  File:     demo.py
##  Authors:  Chris Lovett
##
##  Requires: Python 3.x
##
####################################################################################################

import sys
import os
import numpy as np
import cv2
import demoHelper as d

# note: to run this in headless mode on a Linux machine run the following from your terminal window
# export DISPLAY=:0
# then add the '-save' argument to get tagged frames to be saved to disk.

def main(args):
    helper = d.DemoHelper()
    helper.parse_arguments(args,
            "Runs the given ELL model passing images from camera or static image file\n"
            "Either the ELL model file, or the compiled model's Python module must be given,\n"
            "using the --model or --compiledModel options respectively.\n"
            "Example:\n"
            "   python demo.py categories.txt --compiledModel tutorial1/pi3/model1\n"
            "   python demo.py categories.txt --model model1.ell\n"
            "This shows opencv window with image classified by the model using given labels")

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
        predictions = helper.predict(data)

        # Get the (at most) top 5 predictions that meet our threshold. This is returned as a list of tuples,
        # each with the text label and the prediction score.
        top5 = helper.get_top_n_predictions(predictions, 5)

        # Turn the top5 into a text string to display
        text = ", ".join(["(" + str(int(element[1]*100)) + "%) " + helper.get_label(element[0]) for element in top5])

        save = False
        if (text != lastPrediction):
            print(text)
            save = True
            lastPrediction = text

        # Draw the text on the frame
        if not helper.nogui:
            frameToShow = frame
            helper.draw_label(frameToShow, text)
            helper.draw_fps(frameToShow)
            # Show the new frame
            helper.show_image(frameToShow, save)

    helper.report_times()

if __name__ == "__main__":
    args = sys.argv
    args.pop(0) # when an args list is passed to parse_args, the first argument (program name) needs to be dropped
    main(args)
