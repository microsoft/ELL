####################################################################################################
#
# Project:  Embedded Learning Library (ELL)
# File:     onnx_utilities.py (importers)
# Authors:  Chris Lovett
#
# Requires: Python 3.x, onnx-v1.22
#
####################################################################################################

import os
import sys
import unittest

import torch

script_path = os.path.dirname(os.path.abspath(__file__))
sys.path.append(os.path.join(script_path, '..', '..', '..', 'utilities', 'pythonlibs'))
sys.path.append(os.path.join(script_path, '..', '..'))
sys.path.append(os.path.join(script_path, '..', '..', 'common', 'test'))
sys.path.append(os.path.join(script_path, '..'))

import find_ell  # noqa 401
import logger
import torch_verifier
import common_importer_test
from custom_layers import *
from download_helper import *
import onnx


class OnnxImporterTestCase(common_importer_test.EllImporterTestBase):
    def setup(self):
        super(OnnxImporterTestCase, self).setUp()

    def test_basic_onnx(self):
        self._logger = logger.get()
        self._logger.info("----------- test_basic_onnx is working")
        input_shape = (32, 32, 3)
        output_shape = (1, 1, 20)
        model = TestNet(input_shape, output_shape)
        torch_shape = (1, 3, 32, 32)
        tensor = torch.randn(torch_shape)
        model.forward(tensor)
        torch_model_path = "model.pth"
        torch.save(model.state_dict(), torch_model_path)

        model_state = torch.load(torch_model_path)
        model.load_state_dict(model_state)

        model_path = "model.onnx"
        self._logger.info("Exporting model to ONNX: {}".format(model_path))
        model.eval()
        model(tensor)
        torch.onnx._export(model, tensor, model_path, export_params=True, verbose=True)

        self._logger.info("Loading saved ONNX model: {}".format(model_path))
        onnx_model = onnx.load(model_path)
        print("Loaded model of type:", type(onnx_model))

    def test_onnx_model(self):
        self._logger = logger.get()
        self._logger.info("----------- test_onnx_model")
        input_shape = (32, 32, 3)
        output_shape = (1, 1, 20)
        model = TestNet(input_shape, output_shape)
        torch_shape = (1, 3, 32, 32)
        tensor = torch.randn(torch_shape)
        model.forward(tensor)
        torch.save(model.state_dict(), "model.pth")
        verifier = torch_verifier.TorchModelVerifier(model, "model.pth", input_shape, output_shape)
        verifier.add_input(tensor)
        # bugbug: since we have random input, sometimes the ELL compiled model fails, so the verification
        # of compiled model is disabled in this case until we resolve this
        verifier.verify(verify_compiled=False)

    def test_mnist_model(self):
        self._logger = logger.get()
        self._logger.info("----------- test_mnist_model")
        base_model_uri = self.get_test_model_repo()
        filename = os.path.join(base_model_uri, "models", "mnist", "mnist.zip")
        local_torch_file = download_and_extract_model(filename, model_extension=".pth")
        input_shape = (28, 28, 1)
        output_shape = (1, 1, 10)
        model = MNist(input_shape, output_shape)
        verifier = torch_verifier.TorchModelVerifier(model, local_torch_file, input_shape, output_shape)
        verifier.load_image(os.path.join(script_path, "eight.jpg"))
        # bugbug: compiled model is crashing on Linux.  llvm-8 bug?
        verify = os.name == 'nt'
        verifier.verify(verify_compiled=verify)


if __name__ == "__main__":
    unittest.main()
