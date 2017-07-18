####################################################################################################
#
# Project:  Embedded Learning Library (ELL)
# File:     cntk_to_ell_impporter_test.py (importers)
# Authors:  Byron Changuion
#
# Requires: Python 3.x, cntk-2.0-cp35
#
####################################################################################################
from __future__ import print_function

# Try to import CNTK and ELL. If either don't exist it means they have not being built,
# so don't run the tests.
SkipTests = False
try:
    import unittest
    import sys
    sys.path.append('./..')
    sys.path.append('./../../../../interfaces/python')
    sys.path.append('./../../../../interfaces/python/Release')
    sys.path.append('./../../../../interfaces/python/Debug')
    sys.path.append('./../../../../interfaces/python/utilities')

    import ELL
    import ell_utilities
    import cntk_to_ell
    from cntk.layers import Convolution, MaxPooling, AveragePooling, Dropout, BatchNormalization, Dense
    from cntk.ops import *
    from cntk.initializer import glorot_uniform, he_normal
    from cntk import load_model
    import cntk.layers.blocks
    from cntk.logging.graph import *
    import getopt
    import os
    import configparser
    import re
    import struct
    import numpy as np
    import traceback
    import inspect
except Exception:
    SkipTests = True


def dump(obj):
    for attr in dir(obj):
        try:
            print("obj.%s = %s" % (attr, getattr(obj, attr)))
        except:
            print("error:")
    return


class CntkLayersTestCase(unittest.TestCase):
    def setUp(self):
        if SkipTests:
            self.skipTest('Module not tested, CNTK or ELL module missing')

    def test_dense_layer(self):
        # Test a model with a single CNTK Dense layer against the equivalent ELL predictor
        # This verifies that the import functions reshape and reorder values appropriately and
        # that the equivalent ELL layer produces comparable output

        # Create a Dense CNTK layer with no bias or activation
        denseLayer = Dense(5, bias=False)
        x = input((2, 3, 4))  # Input order for CNTK is channels, rows, columns
        cntkModel = denseLayer(x)

        # Create a test set of weights to use for both CNTK and ELL layers
        # CNTK has these in channels, rows, columns, [output shape] order
        weightValues = np.arange(120, dtype=np.float_).reshape(2, 3, 4, 5)

        # Set the weights
        denseLayer.parameters[0].value = weightValues

        # create an ELL Tensor from the cntk weights, which re-orders the weights and produces an appropriately dimensioned tensor
        weightTensor = cntk_to_ell.get_float_tensor_from_cntk_dense_weight_parameter(
            denseLayer.parameters[0])

        # Create the equivalent ELL predictor
        layerParameters = ELL.LayerParameters(ELL.LayerShape(3, 4, 2),  # Input order for ELL is rows, columns, channels
                                              ELL.NoPadding(),
                                              ELL.LayerShape(1, 1, 5),
                                              ELL.NoPadding())

        layer = ELL.FloatFullyConnectedLayer(layerParameters, weightTensor)
        predictor = ELL.FloatNeuralNetworkPredictor([layer])

        # Get the results for both
        inputValues = np.arange(24, dtype=np.float32).reshape(2, 3, 4)
        orderedInputValues = cntk_to_ell.get_float_vector_from_cntk_array(
            inputValues)
        cntkResults = cntkModel(inputValues)
        orderedCntkResults = cntk_to_ell.get_float_vector_from_cntk_array(
            cntkResults)
        ellResults = predictor.Predict(orderedInputValues)

        # Compare the results
        np.testing.assert_array_equal(
            orderedCntkResults, ellResults, 'results for Dense layer do not match!')

        return

    def test_max_pooling_layer(self):
        # Test a model with a single CNTK MaxPooling layer against the equivalent ELL predictor
        # This verifies that the import functions reshape and reorder values appropriately and
        # that the equivalent ELL layer produces comparable output

        # Create a MaxPooling CNTK layer
        poolingLayer = MaxPooling((2, 2), strides=2)
        # Input order for CNTK is channels, rows, columns
        x = input((3, 12, 12))
        cntkModel = poolingLayer(x)

        # Create the equivalent ELL predictor
        layerParameters = ELL.LayerParameters(ELL.LayerShape(12, 12, 3),  # Input order for ELL is rows, columns, channels
                                              ELL.NoPadding(),
                                              ELL.LayerShape(6, 6, 3),
                                              ELL.NoPadding())

        poolingParameters = ELL.PoolingParameters(2, 2)
        layer = ELL.FloatPoolingLayer(
            layerParameters, poolingParameters, ELL.PoolingType.max)
        predictor = ELL.FloatNeuralNetworkPredictor([layer])

        # Get the results for both
        inputValues = np.arange(432, dtype=np.float32).reshape(3, 12, 12)
        cntkResults = cntkModel(inputValues)
        orderedCntkResults = cntk_to_ell.get_float_vector_from_cntk_array(
            cntkResults)  # Note that cntk inserts an extra dimension of 1 in the front
        orderedInputValues = cntk_to_ell.get_float_vector_from_cntk_array(
            inputValues)
        ellResults = predictor.Predict(orderedInputValues)

        # Compare them
        np.testing.assert_array_equal(
            orderedCntkResults, ellResults, 'results for MaxPooling layer do not match!')

        return

    def test_convolution_layer(self):
        # Test a model with a single CNTK Convolution layer against the equivalent ELL predictor
        # This verifies that the import functions reshape and reorder values appropriately and
        # that the equivalent ELL layer produces comparable output

        # Create a Convolution CNTK layer with no bias or activation, auto-padding, stride of 1
        convolutionLayer = Convolution((3, 3), 5, pad=(
            True, True), strides=1, bias=False, init=0)
        x = input((2, 3, 4))  # Input order for CNTK is channels, rows, columns
        cntkModel = convolutionLayer(x)

        # Create a test set of weights to use for both CNTK and ELL layers
        # CNTK has these in filters, channels, rows, columns order
        weightValues = np.arange(90, dtype=np.float_).reshape(5, 2, 3, 3)

        # Set the weights
        convolutionLayer.parameters[0].value = weightValues

        # create an ELL Tensor from the cntk weights, which re-orders the weights and produces an appropriately dimensioned tensor
        weightTensor = cntk_to_ell.get_float_tensor_from_cntk_convolutional_weight_parameter(
            convolutionLayer.parameters[0])

        # Create the equivalent ELL predictor
        layerParameters = ELL.LayerParameters(ELL.LayerShape(3 + 2, 4 + 2, 2),  # Input order for ELL is rows, columns, channels. Account for padding.
                                              ELL.ZeroPadding(1),
                                              ELL.LayerShape(3, 4, 5),
                                              ELL.NoPadding())

        convolutionalParameters = ELL.ConvolutionalParameters(3, 1, 0, 5)

        layer = ELL.FloatConvolutionalLayer(
            layerParameters, convolutionalParameters, weightTensor)
        predictor = ELL.FloatNeuralNetworkPredictor([layer])

        # Get the results for both
        inputValues = np.arange(24, dtype=np.float32).reshape(2, 3, 4)
        cntkResults = cntkModel(inputValues)
        orderedCntkResults = cntk_to_ell.get_float_vector_from_cntk_array(
            cntkResults)
        orderedInputValues = cntk_to_ell.get_float_vector_from_cntk_array(
            inputValues)
        ellResults = predictor.Predict(orderedInputValues)

        # Compare the results
        np.testing.assert_array_equal(
            orderedCntkResults, ellResults, 'results for Convolution layer do not match!')

        return

    def test_batch_normalization_layer(self):
        # Test a model with a single CNTK BatchNormalization layer against the equivalent ELL predictor
        # This verifies that the import functions reshape and reorder values appropriately and
        # that the equivalent ELL layer produces comparable output

        # Create a BatchNormalization CNTK layer
        batchNorm = BatchNormalization(map_rank=1)

        # Input order for CNTK is channels, rows, columns
        x = input((16, 10, 10))
        cntkModel = batchNorm(x)

        # Create a test set of scales and biases to use for both CNTK and ELL layers
        scaleValues = np.linspace(0.1, 0.5, num=16, dtype=np.float_)
        batchNorm.parameters[0].value = scaleValues
        scaleVector = ELL.FloatVector(scaleValues)

        biasValues = np.linspace(1, 2, num=16, dtype=np.float_)
        batchNorm.parameters[1].value = biasValues
        biasVector = ELL.FloatVector(biasValues)

        # CNTK's BatchNormalization does not support overriding the running mean and variance,
        # so we use zeros, which are the default values
        meanVector = ELL.FloatVector(16)
        varianceVector = ELL.FloatVector(16)

        # Create the equivalent ELL predictor
        layers = []
        layerParameters = ELL.LayerParameters(ELL.LayerShape(10, 10, 16),  # Input order for ELL is rows, columns, channels
                                              ELL.NoPadding(),
                                              ELL.LayerShape(10, 10, 16),
                                              ELL.NoPadding())

        # CNTK BatchNorm = ELL's BatchNorm + Scaling + Bias
        # 1e-5 is the default epsilon for CNTK's BatchNormalization Layer
        epsilon = 1e-5
        layers.append(ELL.FloatBatchNormalizationLayer(
            layerParameters, meanVector, varianceVector, epsilon, ELL.EpsilonSummand_variance))
        layers.append(ELL.FloatScalingLayer(layerParameters, scaleVector))
        layers.append(ELL.FloatBiasLayer(layerParameters, biasVector))

        predictor = ELL.FloatNeuralNetworkPredictor(layers)

        # Compare the results
        inputValues = np.linspace(
            -5, 5, num=16 * 10 * 10, dtype=np.float32).reshape(16, 10, 10)
        cntkResults = cntkModel(inputValues)

        orderedCntkResults = cntk_to_ell.get_float_vector_from_cntk_array(
            cntkResults)  # Note that cntk inserts an extra dimension of 1 in the front
        orderedInputValues = cntk_to_ell.get_float_vector_from_cntk_array(
            inputValues)
        ellResults = predictor.Predict(orderedInputValues)

        # Compare them (precision is 1 less decimal place from epsilon)
        np.testing.assert_array_almost_equal(
            orderedCntkResults, ellResults, 4, 'results for BatchNormalization layer do not match!')

        return


class CntkXorModelTestCase(unittest.TestCase):
    def setUp(self):
        if SkipTests:
            self.skipTest('Module not tested, CNTK or ELL module missing')

    def test_simple_xor_model(self):
        predictor = cntk_to_ell.predictor_from_cntk_model('xorModel1.dnn')
        result = predictor.Predict([0, 0])
        self.assertAlmostEqual(
            result[0], 0, msg='incorrect prediction for [0, 0]')
        result = predictor.Predict([0, 1])
        self.assertAlmostEqual(
            result[0], 1, msg='incorrect prediction for [0, 1]')
        result = predictor.Predict([1, 0])
        self.assertAlmostEqual(
            result[0], 1, msg='incorrect prediction for [1, 0]')
        result = predictor.Predict([1, 1])
        self.assertAlmostEqual(
            result[0], 0, msg='incorrect prediction for [1, 1]')

        # create a map and save to file
        ell_map = ell_utilities.ell_map_from_float_predictor(predictor)
        ell_map.Save("xor_test.map")

        # create a steppable map and save to file
        ell_steppable_map = ell_utilities.ell_steppable_map_from_float_predictor(
            predictor, 100, "XorInputCallback", "XorOutputCallback")
        ell_steppable_map.Save("xor_steppable_test.map")

        return


if __name__ == '__main__':
    if not SkipTests:
        unittest.main()
