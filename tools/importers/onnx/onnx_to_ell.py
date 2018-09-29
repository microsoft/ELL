####################################################################################################
#
# Project:  Embedded Learning Library (ELL)
# File:     onnx_to_ell.py (importers)
# Authors:  Iliass Tiendrebeogo, Chris Lovett
#
# Requires: Python 3.x, onnx-v1.22
#
####################################################################################################

import argparse
import os
import logging
import sys

import typing
from typing import Text
import numpy as np
import onnx

sys.path.append(os.path.join(os.path.dirname(os.path.abspath(__file__)), '../../utilities/pythonlibs'))
sys.path.append(os.path.join(os.path.dirname(os.path.abspath(__file__)), '..'))
sys.path.append(os.path.join(os.path.dirname(os.path.abspath(__file__)), 'lib'))

import find_ell 
import ell
import common.importer
import common.converters
import onnx_converters as convert

_logger = logging.getLogger(__name__)

def convert_onnx_to_ell(path, step_interval_msec=None, lag_threshold_msec=None):
    """
    convert the importer model into a ELL model, optionally a steppable model if step_interval_msec
    and lag_threshold_msec are provided.
    """
    _logger.info("Pre-processing... ")
    converter = convert.OnnxConverter()
    importer_model = converter.load_model(path)
    _logger.info("\n Done pre-processing.")
    try:
        importer_engine = common.importer.ImporterEngine(step_interval_msec=step_interval_msec, lag_threshold_msec=lag_threshold_msec)     
        ell_map = importer_engine.convert_nodes(importer_model)
        ordered_importer_nodes, node_mapping = importer_engine.get_importer_node_to_ell_mapping()
    except Exception as e:
        _logger.error("Error occurred while attempting to convert the model: " + str(e))
        raise 

    return ell_map, ordered_importer_nodes
