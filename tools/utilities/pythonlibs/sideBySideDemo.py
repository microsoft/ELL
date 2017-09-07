####################################################################################################
##
##  Project:  Embedded Learning Library (ELL)
##  File:     sideBySideDemo.py
##  Authors:  Byron Changuion
##
##  Requires: Python 3.x
##
####################################################################################################

import sys
import os
import argparse
import numpy as np
import cv2
import demoHelper as d
import sideBySideHelper as s

# note: to run this in headless mode on a Linux machine run the following from your terminal window
# export DISPLAY=:0
# then add the '-save' argument to get tagged frames to be saved to disk.

def get_parsed_args(argv):
    arg_parser = argparse.ArgumentParser(
        "Runs a number of ELL models that predict the same categories, passing images from camera or static image file\n"
        "in a round-robin fashion. The output is a tiled image, where each tile is the result of one model."
        "Either the ELL model files, or the compiled models' Python modules must be given,\n"
        "using the --models or --compiled options respectively.\n"
        "Example:\n"
        "   python sideBySideDemo.py categories1.txt,categories2.txt --compiled models/pi3/model1,models/pi3/model2\n"
        "   python sideBySideDemo.py sameCategories.txt --models model3.ell,model4.ell\n"
        "This shows opencv window with image classified by the models using given labels")

    # required arguments
    arg_parser.add_argument("labels", help="path to the labels file for evaluating the model")

    # options
    arg_parser.add_argument("--iterations", type=int, help="limits how many times the model will be evaluated, the default is to loop forever")
    arg_parser.add_argument("--save", help="save images captured by the camera", action='store_true')
    arg_parser.add_argument("--threshold", type=float, help="threshold for the minimum prediction score. A lower threshold will show more prediction labels, but they have a higher chance of being completely wrong.", default=0.15)
    arg_parser.add_argument("--realLabels", help="returns the numeric labels as predictions instead of the text labels", action='store_true')

    # mutually exclusive options
    group = arg_parser.add_mutually_exclusive_group()
    group.add_argument("--camera", type=int, help="the camera id of the webcam", default=0)
    group.add_argument("--image", help="path to an image file. If set, evaluates the model using the image, instead of a webcam")

    group2 = arg_parser.add_mutually_exclusive_group()
    group2.add_argument("--models", help="list of comma separated paths to model files")
    group2.add_argument("--compiled", help="list of comma separated paths to the compiled models' Python modules")

    argv.pop(0) # when an args list is passed to parse_args, the first argument (program name) needs to be dropped
    args = arg_parser.parse_args(argv)
    
    return args

def get_demo_helpers(demoArgs):
    demoHelpers = []
    numModels = 0

    # Split the labels string on commas
    demoArgs.labelsList = [x.strip() for x in demoArgs.labels.split(',')]

    if demoArgs.models:
        # Split the models string on commas
        demoArgs.modelsList = [x.strip() for x in demoArgs.models.split(',')]
        numModels = len(demoArgs.modelsList)
        demoArgs.compiledList = [None] * numModels
    else:
        # Split the compiled string on commas
        demoArgs.compiledList = [x.strip() for x in demoArgs.compiled.split(',')]
        numModels = len(demoArgs.compiledList)
        demoArgs.modelsList = [None] * numModels
    # If the number of elements in the labelsList is 1, then use the same labels file
    # for all models
    if (len(demoArgs.labelsList) == 1):
        demoArgs.labelsList = demoArgs.labelsList * numModels

    helperArgs = demoArgs
    for i in range(numModels):
        helperArgs.labels = demoArgs.labelsList[i]
        helperArgs.model = demoArgs.modelsList[i]
        helperArgs.compiled = demoArgs.compiledList[i]        

        helper = d.DemoHelper()
        helper.initialize(helperArgs)
        helper.init_image_source()
        demoHelpers.append(helper)

    return demoHelpers

def main(args):

    demoArgs = get_parsed_args(args)
    modelHelpers = get_demo_helpers(demoArgs)
    if (len(modelHelpers) < 1):
        print('Found no models to run')
        sys.exit()

    helper = s.SideBySideHelper(modelHelpers)

    while (not helper.done()):
        # Grab next frame
        frame = helper.get_next_frame()

        # Process the frame with the side by side demo helper. This
        # passes the frame to each model in turn, and composes a tiled image
        # of each model's result + time to process that frame.
        helper.process_frame(frame)

if __name__ == "__main__":
    main(sys.argv)
