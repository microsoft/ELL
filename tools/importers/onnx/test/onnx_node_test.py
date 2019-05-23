#!/usr/bin/env python3
###############################################################################
#
# Project:  Embedded Learning Library (ELL)
# File:     common_importer_test.py (importers)
# Authors:  Chris Lovett
#
# Requires: Python 3.x, cntk-2.4
#
###############################################################################

import os
import sys

import onnx
import numpy as np
import unittest

script_path = os.path.dirname(os.path.abspath(__file__))
sys.path.append(os.path.join(script_path, '..', '..', '..', 'utilities', 'pythonlibs'))
sys.path.append(os.path.join(script_path, '..', '..'))
sys.path.append(os.path.join(script_path, '..', '..', 'common', 'test'))
sys.path.append(os.path.join(script_path, '..'))
sys.path.append(os.path.join(script_path, '..', 'lib'))

import find_ell  # noqa 401
import ell
import logger

_logger = logger.init(verbosity="WARN")

import common_importer_test
import common.importer
import common.converters
import onnx_converters as convert


class OnnxNodeTest(common_importer_test.EllImporterTestBase):
    def _get_value_info(self, arr, name):
        return onnx.helper.make_tensor_value_info(
            name=name,
            elem_type=onnx.mapping.NP_TYPE_TO_TENSOR_TYPE[arr.dtype],
            shape=arr.shape)

    def _build_graph(self, node, inputs, outputs):
        present_inputs = [x for x in node.input if (x != '')]
        present_outputs = [x for x in node.output if (x != '')]
        inputs_vi = [self._get_value_info(arr, arr_name)
                     for arr, arr_name in zip(inputs, present_inputs)]
        outputs_vi = [self._get_value_info(arr, arr_name)
                      for arr, arr_name in zip(outputs, present_outputs)]
        graph = onnx.helper.make_graph(
            nodes=[node],
            name="test",
            inputs=inputs_vi,
            outputs=outputs_vi)
        return graph

    def convert(self, graph):
        converter = convert.OnnxConverter()
        importer_model = converter.set_graph(graph)
        try:
            importer_engine = common.importer.ImporterEngine(step_interval_msec=None, lag_threshold_msec=None)
            ell_map = importer_engine.convert_nodes(importer_model)
        except Exception as e:
            _logger.error("Error occurred while attempting to convert the model: " + str(e))
            raise

        return ell_map

    def compute(self, graph, input):
        map = self.convert(graph)
        # put tensor into ELL format (rows, cols, channels)
        input = np.squeeze(input, 0)  # remove initial batch dimension
        input = np.rollaxis(input, 0, 3)  # move channel to the end
        input_vec = ell.math.FloatVector(input.ravel())
        output = map.ComputeFloat(input_vec)
        shape = map.GetOutputShape()
        shape = (shape.rows, shape.columns, shape.channels)
        output = np.array(output).reshape(shape)
        # put tensor back in ONNX format (channels, rows, cols)
        return np.rollaxis(output, 2)

    def test_global_average_pool(self):
        _logger.warning("----------- test_global_average_pool")
        node = onnx.helper.make_node(
            'GlobalAveragePool',
            inputs=['x'],
            outputs=['y'],
        )
        # ONNX is (batch, channels, rows, cols)
        x = np.random.randn(1, 3, 5, 5).astype(np.float32)
        # GlobalAveragePool consumes an input tensor X and applies average pooling
        # across the values in the same channel.  Therefore it will average the
        # 5,5 spatial dimensions across the 3 channels, returning a result of shape
        # 1,3,1,1.
        spatial_shape = np.ndim(x) - 2
        y = np.average(x, axis=tuple(range(spatial_shape, spatial_shape + 2)))
        for _ in range(spatial_shape):
            y = np.expand_dims(y, -1)

        graph = self._build_graph(node, x, y)
        output = self.compute(graph, x)
        output = np.expand_dims(output, 0)  # put batch dimension back (ell doesn't have that)
        self.AssertEqual(y, output, "test_globalaveragepool")


if __name__ == "__main__":
    unittest.main()
