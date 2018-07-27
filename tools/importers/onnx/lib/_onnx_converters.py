####################################################################################################
#
# Project:  Embedded Learning Library (ELL)
# File:     _onnx_converters.py (importers)
# Authors:  Iliass Tiendrebeogo
#
# Requires: Python 3.x, onnx-v1.22
#
####################################################################################################

from __future__ import absolute_import
from __future__ import division
from __future__ import print_function
from __future__ import unicode_literals

import os
import argparse
from typing import Text
import sys
import logging

import onnx

sys.path.append(os.path.join(os.path.dirname(os.path.abspath(__file__)), '../../utilities/pythonlibs'))
sys.path.append(os.path.join(os.path.dirname(os.path.abspath(__file__)), '..'))
import find_ell
import ell
import common.importer
import common.converters


_logger = logging.getLogger(__name__) 

class OnnxConverter(object):
    """
    Convert onnx node to importer Node
    """
    def __init__(self, onnx_node):
        self.id = onnx_node.name
        self.importer_op_name = onnx_node.name
        self.uid = onnx_node.name
        self.onnx_node = onnx_node 
        self.inputs = onnx_node.inputs
        self.outputs = onnx_node.outputs
        self.operation_type = self.onnx_node.op_type
        self.attributes = self.onnx_node.attribute
        self.input_shapes = self.onnx_node.input_shape
        self.output_shapes = self.onnx_node.output_shape
        self.padding = onnx_node.padding
        self.weights = self.onnx_node.weights

        self.activation = None

    def log(self):
        """
        Used for diagnostics
        """
        _logger.info("{}.{}, inputs {} -> outputs {}".format(self.importer_op_name, self.uid, self.inputs, self.outputs))
        _logger.info("    weights: {}".format("".join(["({}: {}{},order='{}')".format(w, self.weights[w][0], self.weights[w][1].shape, self.weights[w][2]) for w in self.weights.keys()])))
        _logger.info("    attributes: {}".format(self.attributes))
        _logger.info("    padding: {}".format(self.padding))
        
    def convert(self):
        return common.converters.ImporterNode( id = self.id, 
                                                operation_type = self.operation_type,
                                                weights = self.weights,
                                                inputs = self.inputs,
                                                outputs = self.outputs,
                                                attributes = self.attributes,
                                                padding = self.padding,
                                                input_shapes = self.input_shapes,
                                                output_shapes = self.output_shapes
                                                ) 
