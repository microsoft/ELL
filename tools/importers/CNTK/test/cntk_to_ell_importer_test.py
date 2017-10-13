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
import os
script_path = os.path.dirname(os.path.abspath(__file__))
# Try to import CNTK and ELL. If either don't exist it means they have not being built,
# so don't run the tests.
SkipTests = False
SkipFullModelTests = False

try:
    import unittest
    import sys

    sys.path.append(os.path.join(script_path, '../../../utilities/pythonlibs'))
    sys.path.append(os.path.join(script_path, '..'))
    import find_ell
    import ELL
    import ell_utilities
    import cntk_to_ell
    import demoHelper as dh
    import ziptools
    import lib.cntk_converters as cntk_converters
    import lib.cntk_layers as cntk_layers
    import lib.cntk_utilities as cntk_utilities
    import cntk_import
    from cntk.initializer import glorot_uniform, he_normal
    from cntk.layers import Convolution, MaxPooling, AveragePooling, Dropout, BatchNormalization, Dense
    from cntk import constant, param_relu, load_model
    import cntk.layers.blocks
    from cntk.ops import *
    from custom_functions import BinaryConvolution, CustomSign
    import getopt
    import os
    import configparser
    import re
    import struct
    import numpy as np
    import traceback
    import inspect
    import requests
    import math
except Exception:
    SkipTests = True

def BatchNormalizationTester(map_rank=1,
                             init_scale=1,
                             init_bias=0,
                             normalization_time_constant=5000,
                             blend_time_constant=0,
                             epsilon=0.00001,
                             use_cntk_engine=True,
                             norm_shape=(),
                             init_mean=None,
                             init_variance=None,
                             name=''):
    """ Instantiates a batch normalization layer for testing purposes,
        where mean and variance can be set.
    """
    # parameters bound to this Function
    scale = parameter(shape=norm_shape, init=init_scale, name='scale')
    bias = parameter(shape=norm_shape, init=init_bias, name='bias')
    run_mean = constant(shape=norm_shape, value=init_mean,
                        name='aggregate_mean')
    run_variance = constant(
        shape=norm_shape, value=init_variance, name='aggregate_variance')
    run_count = constant(0, shape=(), name='aggregate_count')

    # expression
    def batch_normalize(x):
        return batch_normalization(x, scale, bias, run_mean, run_variance, running_count=run_count,
                                   spatial=map_rank == 1, normalization_time_constant=normalization_time_constant,
                                   blend_time_constant=blend_time_constant, epsilon=epsilon,
                                   use_cudnn_engine=not use_cntk_engine)

    return batch_normalize


def compare_predictor_output(modelFile, labels, modelTestInput=None, maxLayers=None):
    """Compares an ELL.NeuralNetworkPredictor against its equivalent CNTK model.

    Parameters:
    modelFile -- path to the CNTK model file
    labels -- array of labels
    modelTestInput -- input data in row, column, channel ordering
    maxLayers -- integer to indicate how many layers to run before stopping.
                Setting to None will run all layers and compare against the original model.

    """

    z = load_model(modelFile)
    modelLayers = cntk_utilities.get_model_layers(z)

    # Get the relevant CNTK layers that we will convert to ELL
    layersToConvert = cntk_layers.get_filtered_layers_list(
        modelLayers, maxLayers)

    if len(layersToConvert) == 0:
        raise RuntimeError("No layers are converted, nothing to test")

    # Create a list of ELL layers from the relevant CNTK layers
    print("\nCreating ELL predictor...")
    ellLayers = cntk_layers.convert_cntk_layers_to_ell_layers(
        layersToConvert)

    # Create an ELL neural network predictor from the relevant CNTK layers
    predictor = ELL.FloatNeuralNetworkPredictor(ellLayers)

    if modelTestInput is None:
        inputShape = predictor.GetInputShape()
        modelTestInput = (np.random.rand(inputShape.rows, inputShape.columns, inputShape.channels) * 255).astype(np.float_)
        
    ellTestInput = modelTestInput.ravel()  # rows, columns, channels
    ellResults = predictor.Predict(ellTestInput)

    # rows, columns, channels => channels, rows, columns
    cntkTestInput = np.moveaxis(modelTestInput, -1, 0).astype(np.float32)
    cntkTestInput = np.ascontiguousarray(cntkTestInput)

    # Get the equivalent CNTK model
    if (maxLayers is None):
        print("\nRunning original CNTK model...")

        _, out = z.forward(
           {z.arguments[0]: [cntkTestInput], z.arguments[1]: [list(range(len(labels)))]})
        for output in z.outputs:
           if (output.shape == (len(labels),)):
               out = out[output]
        cntkResults = cntk.ops.softmax(out[0]).eval()

        # For the full model, we compare prediction output instead of layers
        np.testing.assert_array_almost_equal(
            cntkResults, ellResults, 5, 'prediction outputs do not match! (for model ' + modelFile + ')')
    else:
        print("\nRunning partial CNTK model...")

        if (layersToConvert[-1].layer.op_name == 'CrossEntropyWithSoftmax' and len(layersToConvert) > 2):
            # ugly hack for CrossEntropyWithSoftmax
            from cntk.ops import softmax
            zz = as_composite(layersToConvert[-2].layer)
            zz = softmax(zz)
        else:
            zz = as_composite(layersToConvert[-1].layer)

        out = zz(cntkTestInput)
        orderedCntkModelResults = cntk_converters.get_float_vector_from_cntk_array(out)

        np.testing.assert_array_almost_equal(
            orderedCntkModelResults, ellResults, 5, 'prediction outputs do not match! (for partial model ' + modelFile + ')')

class CntkLayersTestCase(unittest.TestCase):
    def setUp(self):
        if SkipTests:
            self.skipTest('Module not tested, CNTK or ELL module missing')

    def verify_compiled(self, predictor, input, expectedOutput, module_name, method_name, precision=5):
        # now run same over ELL compiled model
        map = ell_utilities.ell_map_from_float_predictor(predictor)
        compiled = map.Compile("host", module_name, method_name)
        compiledResults = compiled.ComputeFloat(input)
        # Compare compiled results
        if precision > 0:
            np.testing.assert_array_almost_equal(
                expectedOutput, compiledResults, precision, 'results for %s layer do not match ELL compiled output !' % (module_name))
        else:
            np.testing.assert_array_equal(
                expectedOutput, compiledResults, 'results for %s layer do not match ELL compiled output !' % (module_name))

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
        weightTensor = cntk_converters.get_float_tensor_from_cntk_dense_weight_parameter(
            denseLayer.parameters[0])

        # Create the equivalent ELL predictor
        layerParameters = ELL.LayerParameters(ELL.TensorShape(3, 4, 2),  # Input order for ELL is rows, columns, channels
                                              ELL.NoPadding(),
                                              ELL.TensorShape(1, 1, 5),
                                              ELL.NoPadding())

        layer = ELL.FloatFullyConnectedLayer(layerParameters, weightTensor)
        predictor = ELL.FloatNeuralNetworkPredictor([layer])

        # Get the results for both
        inputValues = np.arange(24, dtype=np.float32).reshape(2, 3, 4)
        orderedInputValues = cntk_converters.get_float_vector_from_cntk_array(
            inputValues)
        cntkResults = cntkModel(inputValues)
        orderedCntkResults = cntk_converters.get_float_vector_from_cntk_array(
            cntkResults)
        ellResults = predictor.Predict(orderedInputValues)

        # Compare the results
        np.testing.assert_array_equal(
            orderedCntkResults, ellResults, 'results for Dense layer do not match!')

        # now run same over ELL compiled model
        self.verify_compiled(predictor, orderedInputValues, orderedCntkResults, "dense", "test")
        return

    def test_max_pooling_layer(self):
        # Test a model with a single CNTK MaxPooling layer against the equivalent ELL predictor
        # This verifies that the import functions reshape and reorder values appropriately and
        # that the equivalent ELL layer produces comparable output
        x = input((2, 15, 15))
        count = 0
        inputValues = (np.random.rand(2, 15, 15).astype(np.float32) * 10.0) - 5 # Random numbers between -5 and 5

        for pool_size in range(2, 4):
            for stride_size in range(2, 3):
                count += 1
                print("test pooling size (%d,%d) and stride %d" % (pool_size, pool_size, stride_size))

                # Create a MaxPooling CNTK layer
                poolingLayer = MaxPooling((pool_size, pool_size), pad=True, strides=stride_size)
                # Input order for CNTK is channels, rows, columns
                cntkModel = poolingLayer(x)
                # Get the results for both
                cntkResults = cntkModel(inputValues)[0]
                outputShape = cntkResults.shape

                padding = int((pool_size - 1) / 2)
                rows = int(inputValues.shape[1] + 2*padding)
                columns = int(inputValues.shape[2] + 2*padding)
                channels = int(inputValues.shape[0])

                # Create the equivalent ELL predictor
                layerParameters = ELL.LayerParameters(ELL.TensorShape(rows, columns, channels),  # Input order for ELL is rows, columns, channels
                                                    ELL.MinPadding(padding),
                                                    ELL.TensorShape(outputShape[1], outputShape[2], outputShape[0]),
                                                    ELL.NoPadding())

                poolingParameters = ELL.PoolingParameters(pool_size, stride_size)
                layer = ELL.FloatPoolingLayer(
                    layerParameters, poolingParameters, ELL.PoolingType.max)
                predictor = ELL.FloatNeuralNetworkPredictor([layer])

                orderedCntkResults = cntk_converters.get_float_vector_from_cntk_array(
                    cntkResults)  # Note that cntk inserts an extra dimension of 1 in the front
                orderedInputValues = cntk_converters.get_float_vector_from_cntk_array(
                    inputValues)
                ellResults = predictor.Predict(orderedInputValues)

                # Compare them
                np.testing.assert_array_almost_equal(
                    orderedCntkResults, ellResults, 5, 'results for MaxPooling layer do not match! (poolsize = ' + str(pool_size) + ', stride =' + str(stride_size))

                # now run same over ELL compiled model                
                self.verify_compiled(predictor, orderedInputValues, orderedCntkResults, 'max_pooling' + str(pool_size) + '_' + str(stride_size), 'test_' + str(count))

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
        weightTensor = cntk_converters.get_float_tensor_from_cntk_convolutional_weight_parameter(
            convolutionLayer.parameters[0])

        # Create the equivalent ELL predictor
        layerParameters = ELL.LayerParameters(ELL.TensorShape(3 + 2, 4 + 2, 2),  # Input order for ELL is rows, columns, channels. Account for padding.
                                              ELL.ZeroPadding(1),
                                              ELL.TensorShape(3, 4, 5),
                                              ELL.NoPadding())

        convolutionalParameters = ELL.ConvolutionalParameters(3, 1, 0, 5)

        layer = ELL.FloatConvolutionalLayer(
            layerParameters, convolutionalParameters, weightTensor)
        predictor = ELL.FloatNeuralNetworkPredictor([layer])

        # Get the results for both
        inputValues = np.arange(24, dtype=np.float32).reshape(2, 3, 4)
        cntkResults = cntkModel(inputValues)
        orderedCntkResults = cntk_converters.get_float_vector_from_cntk_array(
            cntkResults)
        orderedInputValues = cntk_converters.get_float_vector_from_cntk_array(
            inputValues)
        ellResults = predictor.Predict(orderedInputValues)

        # Compare the results
        np.testing.assert_array_equal(
            orderedCntkResults, ellResults, 'results for Convolution layer do not match!')

        # now run same over ELL compiled model
        self.verify_compiled(predictor, orderedInputValues, orderedCntkResults, "convolution", "test")
        return

    def test_binary_convolution_layer(self):
        # Test a model with a single CNTK Binary Convolution layer against the equivalent ELL predictor
        # This verifies that the import functions reshape and reorder values appropriately and
        # that the equivalent ELL layer produces comparable output

        # Create a test set of weights to use for both CNTK and ELL layers
        # CNTK has these in filters, channels, rows, columns order
        weightValues = (np.random.rand(5, 2, 3, 3).astype(np.float32) * 10.0) - 5 # Random numbers between -5 and 5

        # create an ELL Tensor from the cntk weights, which re-orders the weights and produces an appropriately dimensioned tensor
        weightTensor = cntk_converters.get_float_tensor_from_cntk_convolutional_weight_value_shape(
            weightValues, weightValues.shape)

        # Create a Binary Convolution CNTK layer with no bias, no activation, stride 1
        x = input((2, 10, 10))  # Input order for CNTK is channels, rows, columns
        cntkModel = CustomSign(x)

        cntkModel = BinaryConvolution((10, 10), num_filters=5, channels=2, init=weightValues,
                                      pad=True, bias=False, init_bias=0, activation=False)(cntkModel)

        # Create the equivalent ELL predictor
        layerParameters = ELL.LayerParameters(ELL.TensorShape(10 + 2, 10 + 2, 2),  # Input order for ELL is rows, columns, channels. Account for padding.
                                              ELL.ZeroPadding(1),
                                              ELL.TensorShape(10, 10, 5),
                                              ELL.NoPadding())

        convolutionalParameters = ELL.BinaryConvolutionalParameters(
            3, 1, ELL.BinaryConvolutionMethod.bitwise, ELL.BinaryWeightsScale.none)

        layer = ELL.FloatBinaryConvolutionalLayer(
            layerParameters, convolutionalParameters, weightTensor)

        predictor = ELL.FloatNeuralNetworkPredictor([layer])

        # Get the results for both
        inputValues = (np.random.rand(2, 10, 10).astype(np.float32) * 100.0) - 50 # Random numbers between -50 and 50

        cntkResults = cntkModel(inputValues)
        orderedCntkResults = cntk_converters.get_float_vector_from_cntk_array(
            cntkResults)

        orderedInputValues = cntk_converters.get_float_vector_from_cntk_array(
            inputValues)
        ellResults = predictor.Predict(orderedInputValues)

        # Compare the results
        np.testing.assert_array_equal(
            orderedCntkResults, ellResults, 'results for Binary Convolution layer do not match!')

        # now run same over ELL compiled model
        self.verify_compiled(predictor, orderedInputValues, orderedCntkResults, "binary_convolution", "test")
        return

    def test_batch_normalization_layer(self):
        # Test a model with a single CNTK BatchNormalization layer against the equivalent ELL predictor
        # This verifies that the import functions reshape and reorder values appropriately and
        # that the equivalent ELL layer produces comparable output

        # Create a test set of scales and biases to use for both CNTK and ELL layers
        scaleValues = np.linspace(0.1, 0.5, num=16, dtype=np.float32)
        scaleVector = cntk_converters.get_float_vector_from_cntk_array(
            scaleValues)

        biasValues = np.linspace(1, 2, num=16, dtype=np.float32)
        biasVector = cntk_converters.get_float_vector_from_cntk_array(
            biasValues)

        meanValues = np.linspace(-0.5, 0.5, num=16, dtype=np.float32)
        meanVector = cntk_converters.get_float_vector_from_cntk_array(
            meanValues)

        varianceValues = np.linspace(-1, 1, num=16, dtype=np.float32)
        varianceVector = cntk_converters.get_float_vector_from_cntk_array(
            varianceValues)

        # Create a BatchNormalization CNTK layer
        # CNTK's BatchNormalization layer does not support setting the running mean and variance,
        # so we use a wrapper function around the batch_normalization op
        batchNorm = BatchNormalizationTester(init_scale=scaleValues, norm_shape=scaleValues.shape,
                                             init_bias=biasValues, init_mean=meanValues, init_variance=varianceValues)

        # Input order for CNTK is channels, rows, columns
        x = input((16, 10, 10))
        cntkModel = batchNorm(x)

        # Create the equivalent ELL predictor
        layers = []
        layerParameters = ELL.LayerParameters(ELL.TensorShape(10, 10, 16),  # Input order for ELL is rows, columns, channels
                                              ELL.NoPadding(),
                                              ELL.TensorShape(10, 10, 16),
                                              ELL.NoPadding())

        # CNTK BatchNorm = ELL's BatchNorm + Scaling + Bias
        # 1e-5 is the default epsilon for CNTK's BatchNormalization Layer
        epsilon = 1e-5
        layers.append(ELL.FloatBatchNormalizationLayer(
            layerParameters, meanVector, varianceVector, epsilon, ELL.EpsilonSummand_variance))
        layers.append(ELL.FloatScalingLayer(layerParameters, scaleVector))
        layers.append(ELL.FloatBiasLayer(layerParameters, biasVector))

        predictor = ELL.FloatNeuralNetworkPredictor(layers)

        inputValues = np.linspace(
            -5, 5, num=16 * 10 * 10, dtype=np.float32).reshape(16, 10, 10)
        cntkResults = cntkModel(inputValues)

        orderedCntkResults = cntk_converters.get_float_vector_from_cntk_array(
            cntkResults)  # Note that cntk inserts an extra dimension of 1 in the front
        orderedInputValues = cntk_converters.get_float_vector_from_cntk_array(
            inputValues)
        ellResults = predictor.Predict(orderedInputValues)

        # Compare the results (precision is 1 less decimal place from epsilon)
        np.testing.assert_array_almost_equal(
            orderedCntkResults, ellResults, 6, 'results for BatchNormalization layer do not match!')

        # now run same over ELL compiled model
        self.verify_compiled(predictor, orderedInputValues, orderedCntkResults, "batch_norm", "test", precision=6)
        return

    def test_prelu_activation_layer(self):
        # Test a model with a single CNTK PReLU activation layer against the equivalent ELL predictor
        # This verifies that the import functions reshape and reorder values appropriately and
        # that the equivalent ELL layer produces comparable output

        # Create a test set of alpha parameters to use for both CNTK and ELL layers
        # Input order for CNTK is channels, rows, columns
        alphaValues = np.linspace(
            1, 2, num=16 * 10 * 10, dtype=np.float32).reshape(16, 10, 10)

        # create an ELL Tensor from the alpha parameters, which re-orders and produces an appropriately dimensioned tensor
        alphaTensor = cntk_converters.get_float_tensor_from_cntk_convolutional_weight_value_shape(
            alphaValues, alphaValues.shape)

        inputValues = np.linspace(
            -5, 5, num=16 * 10 * 10, dtype=np.float32).reshape(16, 10, 10)

        # Evaluate a PReLU CNTK layer
        x = input((16, 10, 10))
        p = parameter(shape=x.shape, init=alphaValues, name="prelu")
        cntkModel = param_relu(p, x)

        # Create the equivalent ELL predictor
        layerParameters = ELL.LayerParameters(ELL.TensorShape(10, 10, 16),  # Input order for ELL is rows, columns, channels
                                              ELL.NoPadding(),
                                              ELL.TensorShape(10, 10, 16),
                                              ELL.NoPadding())
        layer = ELL.FloatPReLUActivationLayer(layerParameters, alphaTensor)
        predictor = ELL.FloatNeuralNetworkPredictor([layer])

        cntkResults = cntkModel(inputValues)
        orderedCntkResults = cntk_converters.get_float_vector_from_cntk_array(
            cntkResults)
        orderedInputValues = cntk_converters.get_float_vector_from_cntk_array(
            inputValues)
        ellResults = predictor.Predict(orderedInputValues)

        # Compare the results
        np.testing.assert_array_equal(
            orderedCntkResults, ellResults, 'results for PReLU Activation layer do not match!')

        # now run same over ELL compiled model
        self.verify_compiled(predictor, orderedInputValues, orderedCntkResults, "prelu_activation", "test")
        return


class CntkXorModelTestCase(unittest.TestCase):
    def setUp(self):
        if SkipTests or SkipFullModelTests:
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


class CntkBinarizedModelTestCase(unittest.TestCase):
    def setUp(self):
        if SkipTests or SkipFullModelTests:
            self.skipTest('Module not tested, CNTK or ELL module missing')

        # TODO: get a smaller model so that these can be checked in
        if (not os.path.exists('cntkDarknetBinarized.model') or
                not os.path.exists('cntkDarknetBinarizedImageNetLabels.txt')):
            self.skipTest('Model files are missing, skipping test')

        self.labels = open(
            "cntkDarknetBinarizedImageNetLabels.txt").readlines()
        self.model_file = "cntkDarknetBinarized.model"

    def test_binarized_model(self):

        rgb = np.array([174, 198, 207])
        data = np.repeat(rgb, 227 * 227).reshape(227, 227, 3).astype(np.float)

        compare_predictor_output(self.model_file,
                                 self.labels, data, maxLayers=38)

        compare_predictor_output(self.model_file,
                                 self.labels, data)
        return


class CntkModelsTestCase(unittest.TestCase):
    def get_model_name(self, url):
        fileName = os.path.basename(url)
        modelFileName, ext = os.path.splitext(fileName)
        if (not ext == '.zip'):
            modelFileName = fileName
        modelName, ext = os.path.splitext(modelFileName)
        return modelName

    def download_file(self, url):
        fileName = os.path.basename(url)
        # Download the file
        response = requests.get(url, stream=True)
        # Write the file to disk
        with open(fileName, "wb") as handle:
            handle.write(response.content)

    def download_and_extract_model(self, url, model_extension='.cntk'):
        fileName = os.path.basename(url)
        modelName = self.get_model_name(url)

        # Download the file
        self.download_file(url)

        # Extract the file if it's a zip
        unzip = ziptools.Extractor(fileName)
        success, modelFileName = unzip.extract_file(model_extension)
        if success:
            print("extracted zipped model: " + modelFileName)
        else:
            print("non-zipped model: " + fileName)
            modelFileName = fileName

        return modelName

    def setUp(self):
        if SkipTests or SkipFullModelTests:
            self.skipTest('Module not tested, CNTK or ELL module missing')

        self.download_file('https://raw.githubusercontent.com/Microsoft/ELL-models/master/models/ILSVRC2012/categories.txt')
        with open('categories.txt') as categories:
            self.categories = categories.readlines()

        # These are the featured models from the gallery
        self.model_urls = [
            'https://github.com/Microsoft/ELL-models/raw/master/models/ILSVRC2012/d_I160x160x3NCMNCMNBMNBMNBMNBMNB1A/d_I160x160x3NCMNCMNBMNBMNBMNBMNB1A.cntk.zip',
            'https://github.com/Microsoft/ELL-models/raw/master/models/ILSVRC2012/d_I160x160x3CMCMCMCMCMCMC1A/d_I160x160x3CMCMCMCMCMCMC1A.cntk.zip'
        ]

        self.model_names = []
        for url in self.model_urls:
            modelName = self.download_and_extract_model(url)
            self.model_names.append(modelName)

    def test_import(self):
        for modelName in self.model_names:
            args = [modelName + '.cntk']
            cntk_import.main(args)
            # Verify that the ELL model file was produced
            self.assertTrue(os.path.exists(modelName + '.ell'), 'Failed to successfully import model: ' + modelName + '.cntk')
            print('Successfully imported ' + modelName + '.cntk')

    def test_model(self):
        # Test the model against the CNTK output for the following cases:
        # - imported predictor 
        # - imported Map/Model (reference implementation)
        # - unarchived Map/Model (reference implementation)
        # - imported Map/Model (compiled implementation)
        # - unarchived Map/Model (compiled implementation)
        for modelName in self.model_names:
            print('Testing ' + modelName + '.cntk vs ELL (' + modelName + ')' )
            # Load the cntk model
            cntkModel = load_model(modelName + '.cntk')
            cntk_utilities.plot_model(cntkModel, modelName + '.png')

            # Import the model into an ELL map live, without unarchiving
            predictor = cntk_to_ell.predictor_from_cntk_model(modelName + '.cntk')
            ellMap = ell_utilities.ell_map_from_float_predictor(predictor)

            # Load the map from archive
            ellMapFromArchive = ELL.ELL_Map(modelName + '.ell')

            inputShape = ellMap.GetInputShape()
            outputShape = ellMap.GetOutputShape()

            # Compile the live map
            ellCompiledMap = ellMap.Compile('host', modelName, 'predict')
            
            # Compile the unarchived map
            ellCompiledMapFromArchive = ellMapFromArchive.Compile('host', modelName, 'predict')

            cntkInput = (np.random.rand(inputShape.channels, inputShape.rows, inputShape.columns) * 255).astype(np.float)
            ellOrderedInput = cntk_converters.get_float_vector_from_cntk_array(cntkInput)
            cntkInput = np.ascontiguousarray(cntkInput)

            # Get the CNTK results
            _, out = cntkModel.forward(
                {cntkModel.arguments[0]: [cntkInput], cntkModel.arguments[1]: [list(range(len(self.categories)))]})
            for output in cntkModel.outputs:
                if (output.shape == (len(self.categories),)):
                    out = out[output]
            cntkResults = cntk.ops.softmax(out[0]).eval()

            print('Comparing predictor output (reference)')
            sys.stdout.flush()

            ellPredictorResults = predictor.Predict(ellOrderedInput)
            # Verify CNTK results and predictor results match
            np.testing.assert_array_almost_equal(
            cntkResults, ellPredictorResults, decimal=5, err_msg='results for CNTK and ELL predictor (' + modelName + ') do not match!')            

            print('Comparing map output (reference)')
            sys.stdout.flush()
            ellMapResults = ellMap.ComputeFloat(ellOrderedInput)
            # Verify CNTK results and ELL map results match
            np.testing.assert_array_almost_equal(
            cntkResults, ellMapResults, decimal=5, err_msg='results for CNTK and ELL map reference (' + modelName + ') do not match!')

            print('Comparing unarchived map output (reference)')
            sys.stdout.flush()
            ellMapFromArchiveResults = ellMapFromArchive.ComputeFloat(ellOrderedInput)

            # Verify CNTK results and unarchived ELL model results match
            np.testing.assert_array_almost_equal(
            cntkResults, ellMapFromArchiveResults, decimal=5, err_msg='results for CNTK and ELL unarchived map reference (' + modelName + ') do not match!')

            print('Comparing map output (compiled)')
            sys.stdout.flush()
            ellCompiledMapResults = ellCompiledMap.ComputeFloat(ellOrderedInput)

            # Verify CNTK results and unarchived ELL model results match
            np.testing.assert_array_almost_equal(
            cntkResults, ellCompiledMapResults, decimal=5, err_msg='results for CNTK and ELL map compiled (' + modelName + ') do not match!')

            print('Comparing unarchived map output (compiled)')
            sys.stdout.flush()
            ellCompiledMapFromArchiveResults = ellCompiledMapFromArchive.ComputeFloat(ellOrderedInput)

            # Verify CNTK results and unarchived ELL model results match
            np.testing.assert_array_almost_equal(
            cntkResults, ellCompiledMapFromArchiveResults, decimal=5, err_msg='results for CNTK and ELL unarchived map compiled (' + modelName + ') do not match!')

            print('Testing output of ' + modelName + '.cntk vs ELL for model ' + modelName + ' passed!' )
            print()
        return


if __name__ == '__main__':
    if not SkipTests:
        unittest.main()
