####################################################################################################
##
##  Project:  Embedded Learning Library (ELL)
##  File:     darknet_import.py (importers)
##  Authors:  Chris Lovett
##
##  Requires: Python 3.x
##
####################################################################################################

import sys
import os
import argparse
import numpy as np
import cv2
sys.path.append(os.path.join(os.path.dirname(os.path.abspath(__file__)), '../../utilities/pythonlibs'))
import find_ell
import darknet_to_ell
import ell_utilities
import json

class DarknetImporter():
    
    def __init__(self, args):
        self.weights_file = args['weights_file']
        self.config_file = args['config_file']
        self.output_directory = args['output_directory']

    def run(self):
        predictor = darknet_to_ell.predictor_from_darknet_model(self.config_file, self.weights_file)

        input_shape = predictor.GetInputShape()
        output_shape = predictor.GetOutputShape()
        
        weights_directory, weights_filename = os.path.split(self.weights_file)
        if self.output_directory is None:
            output_directory = weights_directory
        else:
            output_directory = self.output_directory

        filename_base = os.path.splitext(weights_filename)[0]
        model_file_name = filename_base + '.ell'
        model_file_path = os.path.join(output_directory, model_file_name)
        ell_map = ell_utilities.ell_map_from_float_predictor(predictor)
        print("Saving model file: '" + model_file_name + "'")
        ell_map.Save(model_file_path)


if __name__ == "__main__":
    parser = argparse.ArgumentParser(
        formatter_class=argparse.RawDescriptionHelpFormatter,
        description='Converts darknet model to ELL model', 
        epilog="Example:\n\n    darknet_import darknet.cfg darknet.weights\n\nThis outputs 'darknet.model' and 'darknet_config.json")
    parser.add_argument('config_file', help='Path to darknet configuration file')
    parser.add_argument('weights_file', help='Path to darknet weights file')
    parser.add_argument('-o', '--output_directory', help='Path to output directory (default: input weights file directory)', required=False)
    args = vars(parser.parse_args())

    importer = DarknetImporter(args)
    importer.run()


