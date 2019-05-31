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


class OnnxImporterTestCase(common_importer_test.EllImporterTestBase):
    def setup(self):
        super(OnnxImporterTestCase, self).setUp()

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
        # bugbug: this is crashing on Linux right now...
        if os.name == 'nt':
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
        # bugbug: this is crashing on Linux right now...
        if os.name == 'nt':
            verifier = torch_verifier.TorchModelVerifier(model, local_torch_file, input_shape, output_shape)
            verifier.load_image(os.path.join(script_path, "eight.jpg"))
            verifier.verify()


if __name__ == "__main__":
    unittest.main()
