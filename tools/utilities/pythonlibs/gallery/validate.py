####################################################################################################
##
# Project:  Embedded Learning Library (ELL)
# File:     validate.py
# Authors:  Chris Lovett, Lisa Ong
##
# Requires: Python 3.x
##
####################################################################################################

import sys
import os
import argparse
import demoHelper as d
import json
from itertools import islice
from os.path import basename

# note: to run this in headless mode on a Linux machine run the following from your terminal window
# export DISPLAY=:0
# then add the '-save' argument to get tagged frames to be saved to disk.


def main(args):
    results = []
    with open(args.validation_map, 'r') as vm:
        subset = list(islice(vm, args.maxfiles))

        # subset contains the list to validate
        for entry in subset:
            f, c = entry.split()
            filename = os.path.join(args.validation_path, basename(f))
            if (not os.path.isfile(filename)):
                raise Exception("File not found: " + filename)

            result = validate_image(["validate.py", args.config_file, args.labels, "--image",
                                     filename, "--iterations", "5", "--threshold", "0.02", "--realLabels"], filename)

            result["truth"] = c
            results.append(result)

    with open('validation.json', 'w', encoding='utf-8') as o:
        json.dump(results, o, ensure_ascii=False)


def validate_image(args, filename):
    helper = d.DemoHelper()
    helper.parse_arguments(args)
    print(args)

    # Initialize image source
    helper.init_image_source()

    lastPrediction = ""

    result = {}
    result["input"] = basename(filename)
    print(result["input"])

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
        top5 = helper.get_top_n(predictions, 5)

        text = "".join(
            [str(element[0]) + "(" + str(int(100 * element[1])) + "%)  " for element in top5])

        save = False
        if (text != lastPrediction):
            print("\t" + text)
            save = True
            lastPrediction = text
            result["predictions"] = top5

    result["avg_time"] = helper.get_times()
    print("\t" + str(result["avg_time"]))
    return result


if __name__ == "__main__":
    parser = argparse.ArgumentParser()

    # required arguments
    parser.add_argument(
        "validation_map", help="val.map.txt file containing the filenames and classes")
    parser.add_argument("validation_path", help="path to the validation set")
    parser.add_argument(
        "config_file", help="path to the model configuration file")
    parser.add_argument(
        "labels", help="path to the labels file for evaluating the model")

    # options
    parser.add_argument("--maxfiles", type=int, default=200,
                        help="max number of files to validate")

    args = parser.parse_args()

    print(args)
    main(args)
