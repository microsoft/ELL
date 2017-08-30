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
sys.path.append(os.path.join(os.path.dirname(os.path.abspath(__file__)), '../../utilities/pythonlibs'))
import find_ell
import cntk_to_ell
import ell_utilities
import extractor
import json

def main(args):
    if (len(args) != 2):
        print("Usage cntk_import cntkModelFile")
        print("Converts CNTK model to ELL model")
        print("Example:")
        print("    cntk_import VGG16_ImageNet_Caffe.model")
        print("This outputs 'VGG16_ImageNet_Caffe.ellmodel' and 'VGG16_ImageNet_Caffe_config.json'")
        return
    
    # extract the model if it's in an archive
    unzip = extractor.Extractor(args[1])
    success, filename = unzip.extract_file(".cntk")
    if (success):
        print("extracted: " + filename)
    else:
        # not a zip archive
        filename = args[1]

    predictor = cntk_to_ell.predictor_from_cntk_model(filename)

    input_shape = predictor.GetInputShape()
    output_shape = predictor.GetOutputShape()
    
    model_file_name = os.path.splitext(filename)[0]+'.ellmodel'
    head, tail = os.path.split(model_file_name)

    ell_map = ell_utilities.ell_map_from_float_predictor(predictor)
    print("Saving model file: '" + model_file_name + "'")
    ell_map.Save(model_file_name)

    model_config = {
        'model': tail,
        'input_rows': input_shape.rows,
        'input_columns': input_shape.columns,
        'input_channels': input_shape.channels,
        'output_rows': output_shape.rows,
        'output_columns': output_shape.columns,
        'output_channels': output_shape.channels,
        'input_scale': 1
    }

    config_file_name = os.path.splitext(filename)[0]+'_config.json'
    config_json = json.dumps(model_config, indent=2, sort_keys=True)

    print("Saving config file: '" + config_file_name + "'")
    with open(config_file_name, 'w') as f:
        f.write(config_json)
        f.close()

if __name__ == "__main__":
    main(sys.argv)


