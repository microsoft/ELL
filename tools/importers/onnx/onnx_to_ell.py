####################################################################################################
#
# Project:  Embedded Learning Library (ELL)
# File:     onnx_to_ell.py (importers)
# Authors:  Iliass Tiendrebeogo
#
# Requires: Python 3.x, onnx-v1.22
#
####################################################################################################

import argparse
import os
import logging
import sys

from typing import Text

import onnx

sys.path.append(os.path.join(os.path.dirname(os.path.abspath(__file__)), '../../utilities/pythonlibs'))
sys.path.append(os.path.join(os.path.dirname(os.path.abspath(__file__)), '..'))

import find_ell 
import ell
import common.importer
import common.converters
import lib.onnx_utilities as utils 
from lib._graph_parser import Graph
import lib._onnx_converters as convert


_logger = logging.getLogger(__name__)


def get_onnx_nodes(path):
    """ Return a list of ONNX nodes """

    onnx_nodes = utils.ONNX()._parse_onnx_model(path)
    return onnx_nodes

def make_importer_model(onnx_nodes):
    """
    Make an Importer Model for ELL from onnx_nodes.
    Tranform onnx nodes to ImporterNode which will be converted to ELL
    """
    if not isinstance(onnx_nodes, list):
        onnx_nodes = list(onnx_nodes.values())
        
    Importer_model = common.importer.ImporterModel()

    for node in onnx_nodes:
        uid = node.name      
        weights = node.weights       
        if node:
            node = convert.OnnxConverter(node).convert()
            Importer_model.add_node(uid, node)
            print(node)
            if weights is not None:
                for w in weights:
                    weight = node.weights[w]
                    if len(weight) > 2:
                        Importer_model.add_tensor(weight[0], weight[1], weight[2])

    return Importer_model

def convert_onnx_to_ell(path):
    """
    convert the importer model into a ELL model
    """
    _logger.info("Pre-processing... ")
    onnx_nodes = get_onnx_nodes(path)
    importer_model = make_importer_model(onnx_nodes)
    _logger.info("\n Done pre-processing.")
    try:
        importer_engine = common.importer.ImporterEngine(step_interval_msec=0, lag_threshold_msec=0)     
        ell_map = importer_engine.convert_nodes(importer_model, apply_ordering=False)
    except :
        _logger.error("Error occurred while attempting to convert the model")
        raise 

    return ell_map, onnx_nodes
