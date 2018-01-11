#!/usr/bin/env python3
####################################################################################################
##
##  Project:  Embedded Learning Library (ELL)
##  File:     cntk_import.py (importers)
##  Authors:  Chris Lovett
##
##  Requires: Python 3.x
##
####################################################################################################

import sys
import os
import numpy as np
import cv2
import argparse
sys.path.append(os.path.join(os.path.dirname(os.path.abspath(__file__)), '../../utilities/pythonlibs'))
import find_ell
import cntk_to_ell
import ell_utilities
import json
import ziptools

def main(argv):
    arg_parser = argparse.ArgumentParser(
        "Converts CNTK model to ELL model\n"
        "Example:\n"
        "    cntk_import.py VGG16_ImageNet_Caffe.model\n"
        "This outputs 'VGG16_ImageNet_Caffe.ell' and 'VGG16_ImageNet_Caffe_config.json'\n")

    arg_parser.add_argument("cntk_model_file",
        help="path to a CNTK model file, or a zip archive of a CNTK model file")
    arg_parser.add_argument("--zip_ell_model",
        help="zips the output ELL model if set", action="store_true")

    model_options = arg_parser.add_argument_group('model_options')
    model_options.add_argument("--step_interval",
        help="produce a steppable ELL model for a millisecond interval",
        default=0)
    model_options.add_argument("--lag_threshold",
        help="millisecond time lag before notifying the caller.\n"
             "used when step_interval is set\n",
        default=5)

    args = vars(arg_parser.parse_args(argv))

    model_options = args.get('model_options', {})
    step_interval = model_options.get('step_interval', 0)
    lag_threshold = model_options.get('lag_threshold', 0)

    # extract the model if it's in an archive
    unzip = ziptools.Extractor(args['cntk_model_file'])
    success, filename = unzip.extract_file(".cntk")
    if success:
        print("extracted: " + filename)
    else:
        # not a zip archive
        filename = args['cntk_model_file']

    predictor = cntk_to_ell.predictor_from_cntk_model(filename)

    model_file_name = os.path.splitext(filename)[0] + '.ell'

    ell_map = ell_utilities.ell_map_from_float_predictor(predictor,
        step_interval, lag_threshold)

    print("Saving model file: '" + model_file_name + "'")
    ell_map.Save(model_file_name)

    if args['zip_ell_model']:
        print("Zipping model file: '" + model_file_name + ".zip'")
        zipper = ziptools.Zipper()
        zipper.zip_file(model_file_name, model_file_name + ".zip")
        os.remove(model_file_name)

if __name__ == "__main__":
    main(sys.argv[1:]) # drop the first argument (program name)
