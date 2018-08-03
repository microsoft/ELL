#!/usr/bin/env python3
####################################################################################################
##
##  Project:  Embedded Learning Library (ELL)
##  File:     sample.py (importers)
##  Authors:  Byron Changuion
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

sys.path.append(os.path.join(os.path.dirname(os.path.abspath(__file__)), '../../utilities/pythonlibs'))
sys.path.append(os.path.join(os.path.dirname(os.path.abspath(__file__)), '..'))
import find_ell
import ell
import common.converters
import common.importer

_logger = logging.getLogger(__name__)

def main(argv):

    try:
        model = common.importer.ImporterModel()

        # Importers create ImporterNodes that mirror the structure and weights read from
        # their particular model format.
        # This file is a simple example showing how an Importer can create ImportNodes
        # for their model, and call the engine to transform those into an ELL model.

        # Add an Input node
        input_node = common.converters.ImporterNode(
            id="Node1",
            operation_type="Input",
            inputs=[],
            outputs=["Node1_output"],
            weights={},
            attributes={},
            padding={"size": 0, "scheme": ell.neural.PaddingScheme.zeros},
            input_shapes=[((3,64,64), "channel_row_column")],
            output_shapes=[((3,64,64), "channel_row_column")]
            )
        model.add_node(input_node.id, input_node)

        # Add a Convolution node
        # First, let's create a tensor representing the weights. 
        # Just like nodes, weights have unique names/id. They need to be registered
        # with the ImporterModel with a call to model.add_tensor(...)
        conv_weights = ("convolutional_weights1", np.random.rand(16,3,3,3), "filter_channel_row_column")
        model.add_tensor(conv_weights[0], conv_weights[1], conv_weights[2])
        conv_node = common.converters.ImporterNode(
            id="Node2",
            operation_type="Convolution",
            inputs=["Node1_output"],
            outputs=["Node2_output"],
            weights={"weights": conv_weights },
            attributes={"size": 3, "stride": 1},
            padding={"size": 1, "scheme": ell.neural.PaddingScheme.zeros},
            input_shapes=[((3,64,64), "channel_row_column")],
            output_shapes=[((16,64,64), "channel_row_column")]
            )
        model.add_node(conv_node.id, conv_node)

        engine = common.importer.ImporterEngine()

        ell_model = engine.convert_nodes(model)
        ell_model.Save("MyModel.ell")
    except BaseException as e:
        print(e)
    
if __name__ == "__main__":
    logging.basicConfig(level=logging.DEBUG, format="%(message)s")
    main(sys.argv[1:]) # drop the first argument (program name)
