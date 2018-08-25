#!/usr/bin/env python3
####################################################################################################
#
# Project:  Embedded Learning Library (ELL)
# File:     onnx_importer.py (util)
# Authors:  Iliass Tiendrebeogo
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

_logger = logging.getLogger(__name__)

def convert(model, output=None):
    model_directory, filename = os.path.split(model)
    if output:
        output_directory = output
    else:
        output_directory = model_directory

    filename_base = os.path.splitext(filename)[0]
    model_file_name = filename_base + '.ell'
    model_file_path = os.path.join(output_directory, model_file_name)

    ell_map, _ = onnx_to_ell.convert_onnx_to_ell(model) 
 
    _logger.info("Saving model file: '" + model_file_name + "'")
    ell_map.Save(model_file_path)

def main(argv):
    """
    Load onnx model file and print the nodes ops
    """
    logging.basicConfig(level=logging.DEBUG, format="%(message)s")

    parser = argparse.ArgumentParser(description="Converts ONNX model to ELL model\n" 
        "Example:\n"
        "    onnx_to_ell.py model.onnx\n"
        "This outputs 'model.ell' which can be compiled with ELL's 'wrap' tool\n")

    parser.add_argument(
        "input",
        type=Text,
        help="Input model file (onnx or protobuf)"
)
    parser.add_argument('-o', '--output_directory',
        help='Path to output directory (default: input file directory)',
        default=None)

    model_options = parser.add_argument_group('model_options')
    model_options.add_argument("--step_interval",
        help="produce a steppable ELL model for a millisecond interval",
        default=0)
    model_options.add_argument("--lag_threshold",
        help="number of step intervals to fall behind before notifying the caller.\n"
             "used when step_interval is set\n",
        default=0)
    args = parser.parse_args()
    
    convert(args.input, args.output_directory)



if __name__ == "__main__":
    main(sys.argv[1:]) # skip the first argument(program name)