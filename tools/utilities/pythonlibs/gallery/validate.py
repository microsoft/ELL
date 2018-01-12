#!/usr/bin/env python3
####################################################################################################
##
# Project:  Embedded Learning Library (ELL)
# File:     validate.py (gallery)
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
import numpy as np
import logging
import psutil
import subprocess
from itertools import islice
from functools import reduce
from os.path import basename

# note: to run this in headless mode on a Linux machine run the following from your terminal window
# export DISPLAY=:0

def run_validation(args):
    """Loops through each image in the validation set (or a subset of it),
       evaluates it for predictions, and saves the results
    """
    with open(args.validation_map, 'r') as valmap:
        top1 = 0
        top5 = 0
        results = {}
        results["results"] = []
        subset = list(islice(valmap, args.maxfiles))

        # subset contains the list to validate
        for entry in subset:
            f, c = entry.split()
            filename = os.path.join(args.validation_path, basename(f))
            if not os.path.isfile(filename):
                raise Exception("File not found: " + filename)

            # assume: the CNTK models we are validating are trained to expect input as BGR
            result = validate_image([args.labels,
                "--compiledModel", args.compiled_module, "--image", filename,
                "--iterations", "5", "--threshold", "0.0", "--bgr", "true"], filename)

            result["truth"] = int(c)
            results["results"].append(result)

            # each prediction is (label, probability)
            if len(result["predictions"]) > 0:
                top1 = top1 + int(result["truth"] == result["predictions"][0][0])
                top5 = top5 + int(reduce(lambda x, y: x or (y[0] == result["truth"]), result["predictions"], False))

        results["top1_percent"] = (top1 / args.maxfiles) * 100
        results["top5_percent"] = (top5 / args.maxfiles) * 100

        # write Windows line endings
        with open('validation.json', 'w', encoding='utf-8', newline='\r\n') as outfile:
            json.dump(results, outfile, ensure_ascii=False, indent=2, sort_keys=True)

def validate_image(args, filename):
    """Evaluates a single image to get predictions"""
    
    logger = logging.getLogger(__name__)

    helper = d.DemoHelper()
    helper.parse_arguments(args, helpString="Evaluates a series of images to get predictions and performance metrics")

    # Initialize image source
    helper.init_image_source()

    result = {}
    result["input"] = basename(filename)
    logger.info(result["input"])

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

        text = "".join(
            [str(element[0]) + "(" + str(int(100 * element[1])) + "%)  " for element in top5])

        logger.info("\tPrediction: " + text)

        # Ensure that numbers are JSON serializable
        result["predictions"] = [(element[0], np.float(element[1])) for element in top5]

    result["avg_time"] = helper.get_times()
    helper.report_times(node_level=False)

    return result


def monitor_resource_usage():
    """Launch a background process to monitor resource usage
       A separate process is used instead of a thread within this process
       because CPU usage is expected to peg during model evaluation
    """
    process = psutil.Process()
    subprocess.Popen(["nohup", "python", "procmon.py", str(process.pid), "--logfile", "procmon.json",
        "--interval", "0.25"])


if __name__ == "__main__":
    logging.basicConfig(level=logging.INFO, format="%(message)s")
    parser = argparse.ArgumentParser()

    # required arguments
    parser.add_argument("validation_map", help="val.map.txt file containing the filenames and classes")
    parser.add_argument("validation_path", help="path to the validation set")
    parser.add_argument("labels", help="path to the labels file for evaluating the model")
    parser.add_argument("compiled_module", help="path to the compiled model's Python module")

    # options
    parser.add_argument("--maxfiles", type=int, default=200, help="max number of files to validate")

    args = parser.parse_args()
    monitor_resource_usage()
    run_validation(args)
