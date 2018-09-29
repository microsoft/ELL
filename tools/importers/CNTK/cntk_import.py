#!/usr/bin/env python3
####################################################################################################
##
##  Project:  Embedded Learning Library (ELL)
##  File:     cntk_import.py (importers)
##  Authors:  Chris Lovett
##
##  Requires: Python 3.5+
##
####################################################################################################

import argparse
import os
import sys
import json
import logging

import numpy as np
from cntk import load_model

sys.path.append(os.path.join(os.path.dirname(os.path.abspath(__file__)), '../../utilities/pythonlibs'))
sys.path.append(os.path.join(os.path.dirname(os.path.abspath(__file__)), '..'))
sys.path.append(os.path.join(os.path.dirname(os.path.abspath(__file__)), 'lib'))
import find_ell
import cntk_to_ell
import ell
from cntk_utilities import Utilities
import ziptools

_logger = logging.getLogger(__name__)

def main(argv):
    arg_parser = argparse.ArgumentParser(
        description="Converts CNTK model to ELL model\n"
        "Example:\n"
        "    cntk_import.py model.cntk\n"
        "This outputs 'model.ell' which can be compiled with ELL's 'wrap' tool\n")

    arg_parser.add_argument("cntk_model_file",
        help="path to a CNTK model file, or a zip archive of a CNTK model file")
    arg_parser.add_argument("--zip_ell_model",
        help="zips the output ELL model if set", action="store_true")
    arg_parser.add_argument("--use_legacy_importer",
        help="specifies whether to use the new importer engine or the legacy importer", action="store_true")
    arg_parser.add_argument("--plot_model",
        help="specifies whether to plot the model using SVG to cntk_model_file.svg", action="store_true")
    arg_parser.add_argument("--verify_vision_model",
        help="verifies the imported vision ELL model produces the same output as the original CNTK model", action="store_true")
    arg_parser.add_argument("--verify_audio_model",
        help="verifies the imported audio ELL model produces the same output as the original CNTK model", action="store_true")
    arg_parser.add_argument("--verbose",
        help="print verbose output during the import. Helps to diagnose ", action="store_true")

    model_options = arg_parser.add_argument_group('model_options')
    model_options.add_argument("--step_interval",
        help="produce a steppable ELL model for a millisecond interval",
        default=0)
    model_options.add_argument("--lag_threshold",
        help="millisecond time lag before notifying the caller.\n"
             "used when step_interval is set\n",
        default=5)

    args = vars(arg_parser.parse_args(argv))

    if args["verbose"]:
        logging.basicConfig(level=logging.DEBUG, format="%(message)s")
    else:
        logging.basicConfig(level=logging.INFO, format="%(message)s")

    model_options = args.get('model_options', {})
    step_interval = model_options.get('step_interval', None)
    lag_threshold = model_options.get('lag_threshold', None)
    plot_model = args["plot_model"]
    verify_model = { "vision": args["verify_vision_model"],
                     "audio": args["verify_audio_model"]}

    # extract the model if it's in an archive
    unzip = ziptools.Extractor(args['cntk_model_file'])
    success, filename = unzip.extract_file(".cntk")
    if success:
        _logger.info("Extracted: " + filename)
    else:
        # not a zip archive
        filename = args['cntk_model_file']

    if not args["use_legacy_importer"]:
        _logger.info("-- Using new importer engine --")
        ell_map = cntk_to_ell.map_from_cntk_model_using_new_engine(filename, step_interval, lag_threshold, plot_model, verify_model)
    else:
        _logger.info("-- Using legacy importer --")
        predictor = cntk_to_ell.predictor_from_cntk_model(filename)
        ell_map = ell.neural.utilities.ell_map_from_float_predictor(predictor,
            step_interval, lag_threshold)

    model_file_name = os.path.splitext(filename)[0] + ".ell"

    _logger.info("\nSaving model file: '" + model_file_name + "'")
    ell_map.Save(model_file_name)

    if args["zip_ell_model"]:
        _logger.info("Zipping model file: '" + model_file_name + ".zip'")
        zipper = ziptools.Zipper()
        zipper.zip_file(model_file_name, model_file_name + ".zip")
        os.remove(model_file_name)

if __name__ == "__main__":
    main(sys.argv[1:]) # drop the first argument (program name)
