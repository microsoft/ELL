#!/usr/bin/env python3
####################################################################################################
##
##  Project:  Embedded Learning Library (ELL)
##  File:     darknet_import.py (importers)
##  Authors:  Chris Lovett
##
##  Requires: Python 3.x
##
####################################################################################################

import argparse
import json
import os
import sys
import logging
import numpy as np
import cv2

sys.path.append(os.path.join(os.path.dirname(os.path.abspath(__file__)), '../../utilities/pythonlibs'))
import find_ell
import darknet_to_ell
import ell
import logger

class DarknetImporter:
    def __init__(self, args):
        self.weights_file = args['weights_file']
        self.config_file = args['config_file']
        self.output_directory = args['output_directory']

        model_options = args.get('model_options', {})
        self.step_interval = model_options.get('step_interval', 0)
        self.lag_threshold = model_options.get('lag_threshold', 0)
        self.logger = logger.get()

    def run(self):
        predictor = darknet_to_ell.predictor_from_darknet_model(
            self.config_file, self.weights_file)

        weights_directory, weights_filename = os.path.split(self.weights_file)
        if self.output_directory:
            output_directory = self.output_directory
        else:
            output_directory = weights_directory

        filename_base = os.path.splitext(weights_filename)[0]
        model_file_name = filename_base + '.ell'
        model_file_path = os.path.join(output_directory, model_file_name)
        ell_map = ell.neural.utilities.ell_map_from_float_predictor(predictor,
            self.step_interval, self.lag_threshold)
        self.logger.info("Saving model file: '" + model_file_name + "'")
        ell_map.Save(model_file_path)


if __name__ == "__main__":
    
    logging.basicConfig(level=logging.INFO, format="%(message)s")
    
    parser = argparse.ArgumentParser(
        formatter_class=argparse.RawDescriptionHelpFormatter,
        description='Converts darknet model to ELL model',
        epilog="Example:\n\n    darknet_import darknet.cfg darknet.weights\n"
            "\nThis outputs 'darknet.model' and 'darknet_config.json")
    parser.add_argument('config_file', help='Path to darknet configuration file')
    parser.add_argument('weights_file', help='Path to darknet weights file')

    parser.add_argument('-o', '--output_directory',
        help='Path to output directory (default: input weights file directory)')

    model_options = parser.add_argument_group('model_options')
    model_options.add_argument("--step_interval",
        help="produce a steppable ELL model for a millisecond interval",
        default=0)
    model_options.add_argument("--lag_threshold",
        help="number of step intervals to fall behind before notifying the caller.\n"
             "used when step_interval is set\n",
        default=5)

    parser_args = vars(parser.parse_args())

    importer = DarknetImporter(parser_args)
    importer.run()
