#!/usr/bin/env python3
####################################################################################################
#
# Project:  Embedded Learning Library (ELL)
# File:     onnx_importer.py (importers)
# Authors:  Iliass Tiendrebeogo, Chris Lovett
#
# Requires: Python 3.x, onnx-v1.22
#
####################################################################################################

import onnx 
import os
import argparse
from typing import Text
import sys
import logging  

sys.path.append(os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', '..', 'utilities/pythonlibs'))
sys.path.append(os.path.join(os.path.dirname(os.path.abspath(__file__)), '..'))
sys.path.append(os.path.dirname(os.path.abspath(__file__)))

import find_ell 
import ell
import common.importer
import common.converters
import onnx_to_ell 
import ziptools

_logger = logging.getLogger(__name__)

def convert(model, output=None, zip_ell_model=None, step_interval=None, lag_threshold=None):
    model_directory, filename = os.path.split(model)
    if output:
        output_directory = output
    else:
        output_directory = model_directory

    filename_base = os.path.splitext(filename)[0]
    model_file_name = filename_base + '.ell'
    model_file_path = os.path.join(output_directory, model_file_name)

    ell_map, _ = onnx_to_ell.convert_onnx_to_ell(model, step_interval_msec=step_interval, lag_threshold_msec=lag_threshold) 
 
    _logger.info("Saving model file: '" + model_file_path + "'")
    ell_map.Save(model_file_path)

    if zip_ell_model:
        _logger.info("Zipping model file: '" + model_file_name + ".zip'")
        zipper = ziptools.Zipper()
        zipper.zip_file(model_file_name, model_file_name + ".zip")
        os.remove(model_file_name)
    return model_file_path

def main():
    parser = argparse.ArgumentParser(description="Converts ONNX model to ELL model\n" 
        "Example:\n"
        "onnx_import.py model.onnx\n"
        "This outputs 'model.ell' which can be compiled with ELL's 'wrap' tool\n")
    parser.add_argument(
        "input",
        type=Text,
        help="Input model file (onnx or protobuf)")
    parser.add_argument("--zip_ell_model",
        help="zips the output ELL model if set", action="store_true")
    parser.add_argument("--verbose",
        help="print verbose output during the import. Helps to diagnose ", action="store_true")
    parser.add_argument('-o', '--output_directory',
        help='Path to output directory (default: input file directory)',
        default=None)

    model_options = parser.add_argument_group('model_options')
    model_options.add_argument("--step_interval",
        help="produce a steppable ELL model for a millisecond interval",
        default=None)
    model_options.add_argument("--lag_threshold",
        help="number of step intervals to fall behind before notifying the caller.\n"
             "used when step_interval is set\n",
        default=None)
    args = parser.parse_args()

    if args.verbose:
        logging.basicConfig(level=logging.DEBUG, format="%(message)s")
    else:
        logging.basicConfig(level=logging.INFO, format="%(message)s")
    
    convert(args.input, args.output_directory, args.zip_ell_model, args.step_interval, args.lag_threshold)

if __name__ == "__main__":
    main()
    