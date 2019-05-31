#!/usr/bin/env python3
####################################################################################################
#
#  Project:  Embedded Learning Library (ELL)
#  File:     darknet_import.py (importers)
#  Authors:  Chris Lovett
#
#  Requires: Python 3.x
#
####################################################################################################

import argparse
import os
import sys

sys.path.append(os.path.join(os.path.dirname(os.path.abspath(__file__)), '../../utilities/pythonlibs'))
import find_ell  # noqa 401
import ell
import logger
import darknet_to_ell


class DarknetImporter:
    def __init__(self, args):
        self.weights_file = args['weights_file']
        self.config_file = args['config_file']
        self.output_directory = args['output_directory']

        self.step_interval = args['step_interval']
        self.lag_threshold = args['lag_threshold']
        if self.step_interval is not None and self.lag_threshold is None:
            self.lag_threshold = self.step_interval * 2

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
        ell_map = ell.neural.utilities.ell_map_from_predictor(
            predictor, self.step_interval, self.lag_threshold)
        self.logger.info("Saving model file: '" + model_file_name + "'")
        ell_map.Save(model_file_path)


if __name__ == "__main__":

    parser = argparse.ArgumentParser(
        formatter_class=argparse.RawDescriptionHelpFormatter,
        description='Converts darknet model to ELL model',
        epilog="Example:\n\n    darknet_import darknet.cfg darknet.weights\n"
               "\nThis outputs 'darknet.model' and 'darknet_config.json")
    parser.add_argument('config_file', help='Path to darknet configuration file')
    parser.add_argument('weights_file', help='Path to darknet weights file')

    parser.add_argument(
        '-o', '--output_directory',
        help='Path to output directory (default: input weights file directory)')

    model_options = parser.add_argument_group('model_options')
    model_options.add_argument(
        "--step_interval",
        help="produce a steppable ELL model for a millisecond interval",
        type=float)
    model_options.add_argument(
        "--lag_threshold",
        help="number of step intervals to fall behind before notifying the caller.\n"
             "used when step_interval is set\n",
        type=float)

    logger.add_logging_args(parser)
    args = parser.parse_args()
    _logger = logger.setup(args)

    parser_args = vars(args)

    importer = DarknetImporter(parser_args)
    importer.run()
