####################################################################################################
##
# Project:  Embedded Learning Library (ELL)
# File:     darknet_to_ell_impporter_test.py (importers)
# Authors:  Byron Changuion
#
# Requires: Python 3.x
##
####################################################################################################
import sys
import getopt
import os
import configparser
import re
import struct
import numpy as np
import traceback
import inspect
import unittest

sys.path.append('./../../../../interfaces/python')
sys.path.append('./../../../../interfaces/python/Release')
sys.path.append('./../../../../interfaces/python/Debug')
import ELL

currentdir = os.path.dirname(os.path.abspath(
    inspect.getfile(inspect.currentframe())))
parentdir = os.path.dirname(currentdir)
sys.path.insert(0, parentdir)

import darknet_to_ell

# Load a test darknet model and verify its output.
# The unit test model verifies the most common darknet layers:
# - convolutional
# - maxpool
# - avgpool
# - fully connected
# - softmax


class DarknetModelTestCase(unittest.TestCase):
    def test_darknet_model(self):
        # Create synthetic input data
        input1 = np.arange(28 * 28, dtype=np.float).reshape(28, 28, 1) / 255
        # Create an ELL predictor from the darknet model files
        predictor = darknet_to_ell.predictor_from_darknet_model(
            'unittest.cfg', 'unittest.weights')

        # Feed the input through the model
        result1 = predictor.Predict(input1.ravel())
        # Verify its what we expect
        expectedResult1 = [0.08592157065868378, 0.08800867199897766, 0.08507226407527924, 0.11691804975271225, 0.09915791451931000,
                           0.08514316380023956, 0.08144238591194153, 0.08417271077632904, 0.08590476959943771, 0.18825851380825043]
        np.testing.assert_array_almost_equal(
            result1, expectedResult1, 5, 'prediction of first input does not match expected results!')

        # Feed the next input through the model
        input2 = np.flipud(input1)
        result2 = predictor.Predict(input2.ravel())
        # Verify its what we expect
        expectedResult2 = [0.08536843955516815, 0.08800827711820602, 0.08450024574995041, 0.17433108389377594, 0.08736364543437958,
                           0.08676918596029282, 0.08128042519092560, 0.08372557163238525, 0.08511145412921906, 0.14354163408279420]
        np.testing.assert_array_almost_equal(
            result2, expectedResult2, 5, 'prediction of second input does not match expected results!')

        return


if __name__ == '__main__':
    unittest.main()
