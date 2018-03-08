####################################################################################################
##
##  Project:  Embedded Learning Library (ELL)
##  File:     darknet_to_ell.py (importers)
##  Authors:  Byron Changuion, Kern Handa
##
##  Requires: Python 3.x
##
####################################################################################################

import sys
import os
sys.path.append(os.path.join(os.path.dirname(os.path.abspath(__file__)), '../../utilities/pythonlibs'))
from configparser import ConfigParser
from collections import OrderedDict
import logging
import re
import struct
import getopt
import numpy as np
import find_ell
import ell

_logger = logging.getLogger(__name__)

def convolutional_out_height(layer):
    return (int(layer['h']) + 2*int(layer['padding']) - int(layer['size'])) / int(layer['stride']) + 1


def convolutional_out_width(layer):
    return (int(layer['w']) + 2*int(layer['padding']) - int(layer['size'])) / int(layer['stride']) + 1


def parse_cfg(filename):
    """Parses a Darknet .cfg file and returns a list of layers. Each layer has
       properties denoting type, shape of input and outputs, padding requirements
       and anything else needed to construct up the relevant ELL layers"""
    with open(filename) as f:
        content = f.read()

    class DarknetConfigDict(OrderedDict):
        def __init__(self, **kwargs):
            OrderedDict.__init__(self, **kwargs)
            self.__keysuffixes = {}

        def __setitem__(self, key, value):
            if isinstance(value, dict):
                suffix = self.__keysuffixes.setdefault(key, 0)
                self.__keysuffixes[key] += 1
                key += "_" + str(suffix)
            OrderedDict.__setitem__(self, key, value)

    config = ConfigParser(dict_type=DarknetConfigDict, strict=False)
    config.read_string(content)
    network = []
    for section in config._sections:
        layer_desc = {'type' : section[:section.rfind('_')] }
        layer_desc.update(config._sections[section])
        network.append(layer_desc)

    def print_layer(layer):
        PRETTY_TYPE_MAP = {
            "convolutional": "BinaryConvolution" if layer.get("xnor") == 1 else
                             "Convolution",
            "connected": "FullyConnected",
            "maxpool": "MaxPooling",
            "avgpool": "AveragePooling",
            "softmax": "Softmax",
            "region": "RegionDetection"
        }
        pretty_type = PRETTY_TYPE_MAP.get(layer.get("type"))
        if not pretty_type:
            return
        _logger.info(("{} :  {h}x{w}x{c}  ->  {out_h}x{out_w}x{out_c}"
               " | input padding {inputPadding}  output padding {outputPadding}"
               ).format(pretty_type, **layer))

    # add extra information needed, size calculations and properties like padding
    for i, layer in enumerate(network):
        if layer['type'] == 'net':
            layer['h'] = int(layer['height'])
            layer['w'] = int(layer['width'])
            layer['c'] = int(layer['channels'])
            layer['out_h'] = int(layer['height'])
            layer['out_w'] = int(layer['width'])
            layer['out_c'] = int(layer['channels'])
        elif layer['type'] == 'crop':
            layer['c'] = network[i-1]['out_c']
            layer['h'] = network[i-1]['out_h']
            layer['w'] = network[i-1]['out_w']
            layer['out_h'] = layer['crop_height']
            layer['out_w'] = layer['crop_width']
            layer['out_c'] = network[i-1]['out_c']
        elif layer['type'] == 'convolutional':
            if 'pad' not in layer:
                layer['pad'] = 0
                layer['padding'] = 0
            elif 'padding' not in layer:
                if ('pad' in layer):
                    if (int(layer['pad']) == 0):
                        layer['padding'] = 0
                    else:
                        layer['padding'] = int((int(layer['size']) - 1) / 2)
                else:
                    layer['padding'] = int((int(layer['size']) - 1) / 2)
            layer['h'] = int(network[i-1]['out_h'])
            layer['w'] = int(network[i-1]['out_w'])
            layer['c'] = int(network[i-1]['out_c'])
            layer['out_h'] = int(convolutional_out_height(layer))
            layer['out_w'] = int(convolutional_out_width(layer))
            layer['out_c'] = int(layer['filters'])
        elif layer['type'] == 'connected':
            layer['h'] = network[i-1]['out_h']
            layer['w'] = network[i-1]['out_w']
            layer['c'] = network[i-1]['out_c']
            layer['inputs'] = int(layer['h']) * int(layer['w']) * int(layer['c'])
            layer['out_w'] = 1
            layer['out_h'] = 1
            layer['out_c'] = int(layer['output'])
        elif layer['type'] == 'maxpool':
            if 'padding' not in layer:
                layer['padding'] = int((int(layer['size']) - 1) / 2)
            layer['h'] = int(network[i-1]['out_h'])
            layer['w'] = int(network[i-1]['out_w'])
            layer['c'] = int(network[i-1]['out_c'])
            layer['out_h'] = int(((int(layer['h'])) + 2 * int(layer['padding'])) / int(layer['stride']))
            layer['out_w'] = int(((int(layer['w'])) + 2 * int(layer['padding'])) / int(layer['stride']))
            layer['out_c'] = layer['c']
        elif layer['type'] == 'avgpool':
            layer['c'] = network[i-1]['out_c']
            layer['out_c'] = layer['c']
            layer['out_h'] = 1
            layer['out_w'] = 1
            layer['h'] = network[i-1]['out_h']
            layer['w'] = network[i-1]['out_w']
            layer['padding'] = 0
            # Darknet's average pooling is across an entire layer. Fix up stride and size so ELL can behave that way.
            layer['size'] = layer["w"]
            layer['stride'] = layer["w"]
        elif layer['type'] == 'softmax':
            layer['c'] = network[i-1]['out_c']
            layer['h'] = network[i-1]['out_h']
            layer['w'] = network[i-1]['out_w']
            layer['out_c'] = layer['c']
            layer['out_h'] = layer['h']
            layer['out_w'] = layer['w']
        elif layer['type'] == 'region':
            layer['c'] = network[i-1]['out_c']
            layer['h'] = network[i-1]['out_h']
            layer['w'] = network[i-1]['out_w']
            layer['out_c'] = network[i-1]['out_c']
            layer['out_h'] = network[i-1]['out_h']
            layer['out_w'] = network[i-1]['out_w']
        else:
            layer['c'] = network[i-1]['out_c']
            layer['h'] = network[i-1]['out_h']
            layer['w'] = network[i-1]['out_w']
            layer['out_c'] = network[i-1]['out_c']
            layer['out_h'] = network[i-1]['out_h']
            layer['out_w'] = network[i-1]['out_w']

    # Do another pass, setting input/output shape and outpadding to next layer's padding
    # Set the ELL padding scheme and shape parameters
    for i, layer in enumerate(network):
        if 'padding' not in layer:
            layer['inputPadding'] = 0
            if layer['type'] == 'maxpool':
                layer['inputPaddingScheme'] = ell.neural.PaddingScheme.min
            else:
                layer['inputPaddingScheme'] = ell.neural.PaddingScheme.zeros
        else:
            layer['inputPadding'] = int(layer['padding'])
            if layer['type'] == 'maxpool':
                layer['inputPaddingScheme'] = ell.neural.PaddingScheme.min
            else:
                layer['inputPaddingScheme'] = ell.neural.PaddingScheme.zeros
        layer['inputShape'] = ell.math.TensorShape(int(layer['h']) + 2 * int(layer['inputPadding']), int(layer['w']) + 2 * int(layer['inputPadding']), int(layer['c']))

        if i < (len(network) - 1):
            nextLayer = network[i + 1]
            if 'padding' not in nextLayer:
                layer['outputPadding'] = 0
            else:
                layer['outputPadding'] = int(nextLayer['padding'])

            if nextLayer['type'] == 'maxpool':
                layer['outputPaddingScheme'] = ell.neural.PaddingScheme.min
            else:
                layer['outputPaddingScheme'] = ell.neural.PaddingScheme.zeros

            layer['outputShape'] = ell.math.TensorShape(int(layer['out_h']) + 2 * int(layer['outputPadding']), int(layer['out_w']) + 2 * int(layer['outputPadding']), int(layer['out_c']))
            layer['outputShapeMinusPadding'] = ell.math.TensorShape(int(layer['out_h']), int(layer['out_w']), int(layer['out_c']))
        else:
            layer['outputPadding'] = 0
            layer['outputPaddingScheme'] = ell.neural.PaddingScheme.zeros
            layer['outputShape'] = ell.math.TensorShape(int(layer['out_h']), int(layer['out_w']), int(layer['out_c']))
            layer['outputShapeMinusPadding'] = ell.math.TensorShape(int(layer['out_h']), int(layer['out_w']), int(layer['out_c']))

        print_layer(layer)

    return network


def create_layer_parameters(inputShape, inputPadding, inputPaddingScheme, outputShape, outputPadding, outputPaddingScheme):
    """Helper function to return ell.neural.LayerParameters given input and output shapes/padding/paddingScheme"""
    inputPaddingParameters = ell.neural.PaddingParameters(inputPaddingScheme, inputPadding)
    outputPaddingParameters = ell.neural.PaddingParameters(outputPaddingScheme, outputPadding)

    return ell.neural.LayerParameters(inputShape, inputPaddingParameters, outputShape, outputPaddingParameters)


def get_weights_tensor(weightsShape, values):
    """Returns an ELL tensor from Darknet weights. The weights are re-ordered
       to rows, columns, channels"""
    weights = np.array(values, dtype=np.float).reshape(weightsShape)
    if (len(weights.shape) == 3):
        orderedWeights = np.rollaxis(weights, 0, 3)
    elif (len(weights.shape) == 4):
        orderedWeights = np.rollaxis(weights, 1, 4)
        orderedWeights = orderedWeights.reshape((orderedWeights.shape[0] * orderedWeights.shape[1], orderedWeights.shape[2], orderedWeights.shape[3]))
    elif (len(weights.shape) == 2):
        orderedWeights = values
        orderedWeights = orderedWeights.reshape((weightsShape.shape[0], 1, weightsShape.shape[1]))
    else:
        orderedWeights = weights
        orderedWeights = orderedWeights.reshape((1, 1, weightsShape[0]))

    return ell.math.FloatTensor(orderedWeights)


def process_batch_normalization_layer(layer, apply_padding, mean_vals, variance_vals, scale_vals):
    """Returns ELL layers corresponding to a Darknet batch normalization layer"""

    # Batch normalization in Darknet corresponds to BatchNormalizationLayer, ScalingLayer in ELL
    layers = []

    # Create BatchNormalizationLayer
    layerParameters = create_layer_parameters(layer['outputShapeMinusPadding'], 0, ell.neural.PaddingScheme.zeros, layer['outputShapeMinusPadding'], 0, ell.neural.PaddingScheme.zeros)
    meanVector = ell.math.FloatVector(mean_vals.ravel())
    varianceVector = ell.math.FloatVector(variance_vals.ravel())

    layers.append(ell.neural.FloatBatchNormalizationLayer(layerParameters, meanVector, varianceVector, 1e-6, ell.neural.EpsilonSummand.sqrtVariance))

    # Create Scaling Layer
    if (apply_padding):
        layerParameters = create_layer_parameters(layer['outputShapeMinusPadding'], 0, ell.neural.PaddingScheme.zeros, layer['outputShape'], layer['outputPadding'], layer['outputPaddingScheme'])
    else:
        layerParameters = create_layer_parameters(layer['outputShapeMinusPadding'], 0, ell.neural.PaddingScheme.zeros, layer['outputShapeMinusPadding'], 0, ell.neural.PaddingScheme.zeros)

    layers.append(ell.neural.FloatScalingLayer(layerParameters, scale_vals.ravel()))

    return layers


def get_activation_type(layer):
    """Returns an ell.neural.ActivationType from the layer"""
    if (layer["activation"] == 'relu'):
        return ell.neural.ActivationType.relu
    elif (layer["activation"] == 'sigmoid'):
        return ell.neural.ActivationType.sigmoid
    elif (layer["activation"] == 'leaky'):
        return ell.neural.ActivationType.leaky

    return None


def get_activation_layer(layer, apply_padding):
    """Return an ELL activation layer from a darknet activation"""
    if (apply_padding):
        layerParameters = create_layer_parameters(layer['outputShapeMinusPadding'], 0, ell.neural.PaddingScheme.zeros, layer['outputShape'], layer['outputPadding'], layer['outputPaddingScheme'])
    else:
        layerParameters = create_layer_parameters(layer['outputShapeMinusPadding'], 0, ell.neural.PaddingScheme.zeros, layer['outputShapeMinusPadding'], 0, ell.neural.PaddingScheme.zeros)

    activationType = get_activation_type(layer)

    return ell.neural.FloatActivationLayer(layerParameters, activationType)


def get_bias_layer(layer, apply_padding, bias_vals):
    """Return an ELL bias layer from a darknet layer"""

    if (apply_padding):
        layerParameters = create_layer_parameters(layer['outputShapeMinusPadding'], 0, ell.neural.PaddingScheme.zeros, layer['outputShape'], layer['outputPadding'], layer['outputPaddingScheme'])
    else:
        layerParameters = create_layer_parameters(layer['outputShapeMinusPadding'], 0, ell.neural.PaddingScheme.zeros, layer['outputShapeMinusPadding'], 0, ell.neural.PaddingScheme.zeros)

    biasVector = ell.math.FloatVector(bias_vals.ravel())

    return ell.neural.FloatBiasLayer(layerParameters, biasVector)


def process_convolutional_layer(layer, bin_data, convolution_order):
    """Returns ELL layers corresponding to a Darknet convolutional layer"""

    # Convolution layer in Darknet corresponds to ConvolutionalLayer, BatchNormalizationLayer, BiasLayer and ActivationLayer in ELL
    layers = []

    # Read in binary values
    bias_vals = []
    for i in range(int(layer['filters'])):
        bias_vals.append(struct.unpack('f', bin_data.read(4)))
    bias_vals = np.array(bias_vals, dtype=np.float)
    # now we need to check if these weights have batch normalization data
    scale_vals = []
    mean_vals = []
    variance_vals = []
    if ('batch_normalize' in layer) and ('dontloadscales' not in layer):
        for i in range(int(layer['filters'])):
            scale_vals.append(struct.unpack('f', bin_data.read(4)))
        for i in range(int(layer['filters'])):
            mean_vals.append(struct.unpack('f', bin_data.read(4)))
        for i in range(int(layer['filters'])):
            variance_vals.append(struct.unpack('f', bin_data.read(4)))
    scale_vals = np.array(scale_vals, dtype=np.float)
    mean_vals = np.array(mean_vals, dtype=np.float)
    variance_vals = np.array(variance_vals, dtype=np.float)
    # now we can load the convolutional weights
    weight_vals = []
    num_weights = int(layer['size'])*int(layer['size'])*int(layer['c'])*int(layer['filters'])
    for i in range(num_weights):
        weight_vals.append(struct.unpack('f', bin_data.read(4)))
    weight_vals = np.array(weight_vals, dtype=np.float)

    layerParameters = create_layer_parameters(layer['inputShape'], layer['inputPadding'], layer['inputPaddingScheme'], layer['outputShapeMinusPadding'], 0, ell.neural.PaddingScheme.zeros)
    convolutionWeightsTensor = get_weights_tensor((int(layer['filters']), layer['c'], int(layer["size"]), int(layer["size"])), weight_vals)

    # Create the appropriate convolutional layer
    if 'xnor' not in layer:
        # Create the ELL convolutional layer
        convolutionalParameters = ell.neural.ConvolutionalParameters(int(layer["size"]), int(layer["stride"]), ell.neural.ConvolutionMethod.unrolled, int(layer['filters']))
        layers.append(ell.neural.FloatConvolutionalLayer(layerParameters, convolutionalParameters, convolutionWeightsTensor))
    else:
        # Create the ELL binary convolutional layer
        convolutionalParameters = ell.neural.BinaryConvolutionalParameters(int(layer["size"]), int(layer["stride"]), ell.neural.BinaryConvolutionMethod.bitwise, ell.neural.BinaryWeightsScale.mean)
        layers.append(ell.neural.FloatBinaryConvolutionalLayer(layerParameters, convolutionalParameters, convolutionWeightsTensor))

    # Override global ordering with layer-specific ordering
    if 'order' in layers:
        convolution_order = layer['order']

    applyBatchNormalization = False
    if ('batch_normalize' in layer) and ('dontloadscales' not in layer):
        applyBatchNormalization = True
    activationType = get_activation_type(layer)
    biasIsLast = (activationType is None) and applyBatchNormalization

    if (convolution_order == 'cnba'):
        # This ordering is convolution followed by batch norm, bias then activation
        if (applyBatchNormalization):
            layers += process_batch_normalization_layer(layer, False, mean_vals, variance_vals, scale_vals)
        layers.append(get_bias_layer(layer, biasIsLast, bias_vals))
        if (activationType is not None):
            layers.append(get_activation_layer(layer, True))
    else:
        # This ordering is convolution followed by bias, activation then batch norm
        layers.append(get_bias_layer(layer, biasIsLast, bias_vals))
        if (activationType is not None):
            layers.append(get_activation_layer(layer, not applyBatchNormalization))
        if (applyBatchNormalization):
            layers += process_batch_normalization_layer(layer, True, mean_vals, variance_vals, scale_vals)

    return layers


def get_pooling_layer(layer, poolingType):
    """Returns ELL pooling layer from Darknet pooling layer"""

    # Create the ELL pooling layer
    layerParameters = create_layer_parameters(layer['inputShape'], layer['inputPadding'], layer['inputPaddingScheme'], layer['outputShape'], layer['outputPadding'], layer['outputPaddingScheme'])
    poolingParameters = ell.neural.PoolingParameters(int(layer["size"]), int(layer["stride"]))

    return ell.neural.FloatPoolingLayer(layerParameters, poolingParameters, poolingType)


def get_softmax_layer(layer):
    """Returns ELL softmax layer from Darknet softmax layer"""

    # Create the ELL pooling layer
    layerParameters = create_layer_parameters(layer['inputShape'], layer['inputPadding'], layer['inputPaddingScheme'], layer['outputShape'], layer['outputPadding'], layer['outputPaddingScheme'])

    return ell.neural.FloatSoftmaxLayer(layerParameters)


def get_region_detection_layer(layer):
    """Returns ELL region detection layer from Darknet region detection layer"""

    layerParameters = create_layer_parameters(layer['outputShapeMinusPadding'], 0, ell.neural.PaddingScheme.zeros,
            layer['outputShapeMinusPadding'], 0, ell.neural.PaddingScheme.zeros)

    detectionParams = ell.neural.RegionDetectionParameters(int(layer['w']), int(layer['h']), int(layer['num']), int(layer['classes']),
        int(layer['coords']))

    return ell.neural.FloatRegionDetectionLayer(layerParameters, detectionParams)

def process_fully_connected_layer(layer, weightsData):
    """Returns ELL layers corresponding to a Darknet connected layer"""

    # Connected layer in Darknet corresponds to FullyConnectedLayer, ActivationLayer in ELL
    layers = []

    # Create Fully Connected
    activationType = get_activation_type(layer)
    if activationType:
        layerParameters = create_layer_parameters(layer['outputShapeMinusPadding'], 0, ell.neural.PaddingScheme.zeros, layer['outputShapeMinusPadding'], 0, ell.neural.PaddingScheme.zeros)
    else:
        layerParameters = create_layer_parameters(layer['outputShapeMinusPadding'], 0, ell.neural.PaddingScheme.zeros, layer['outputShape'], layer['outputPadding'], layer['outputPaddingScheme'])

    bias_vals = []
    for i in range(int(layer['output'])):
        bias_vals.append(struct.unpack('f', weightsData.read(4)))
    bias_vals = np.array(bias_vals, dtype=np.float)

    weight_vals = []
    num_weights = int(layer['output'])*int(layer['inputs'])
    for i in range(num_weights):
        weight_vals.append(struct.unpack('f', weightsData.read(4)))
    weight_vals = np.array(weight_vals, dtype=np.float)

    orderedWeights = weight_vals.reshape(layer['c'], layer['h'], layer['w'], (layer['out_h'] * layer['out_w'] * layer['out_c']))
    orderedWeights = np.moveaxis(orderedWeights, 0, 2)
    orderedWeights = np.moveaxis(orderedWeights,-1, 0)
    orderedWeights = orderedWeights.reshape((layer['out_h'] * layer['out_w'] * layer['out_c'] * layer['h'], layer['w'], layer['c']))

    weightsTensor = ell.math.FloatTensor(orderedWeights)

    layers.append(ell.neural.FloatFullyConnectedLayer(layerParameters, weightsTensor))

    if activationType:
        # Create BiasLayer
        layers.append(get_bias_layer(layer, False, bias_vals))
        # Create ActivationLayer
        layers.append(get_activation_layer(layer, True))
    else:
        # Create BiasLayer
        layers.append(get_bias_layer(layer, True, bias_vals))

    return layers


def get_first_scaling_layer(nextLayerParameters):
    scaleValues = np.ones((nextLayerParameters.inputShape.channels), dtype=np.float) * [1/255]

    inputShape = ell.math.TensorShape(nextLayerParameters.inputShape.rows - (2 * nextLayerParameters.inputPaddingParameters.paddingSize),
                                    nextLayerParameters.inputShape.columns - (2 * nextLayerParameters.inputPaddingParameters.paddingSize),
                                    nextLayerParameters.inputShape.channels)

    layerParameters = create_layer_parameters(inputShape, 0, ell.neural.PaddingScheme.zeros, nextLayerParameters.inputShape, nextLayerParameters.inputPaddingParameters.paddingSize, nextLayerParameters.inputPaddingParameters.paddingScheme)
    return ell.neural.FloatScalingLayer(layerParameters, scaleValues.ravel())


def process_network(network, weightsData, convolutionOrder):
    """Returns an ell.neural.FloatNeuralNetworkPredictor as a result of parsing the network layers"""
    ellLayers = []

    for layer in network:
        if layer['type'] in ['net', 'cost', 'dropout']:
            # Known layers that can be safely ignored or don't require processing
            pass
        elif layer['type'] == 'convolutional':
            ellLayers += process_convolutional_layer(layer, weightsData, convolutionOrder)
        elif layer['type'] == 'connected':
            ellLayers += process_fully_connected_layer(layer, weightsData)
        elif layer['type'] == 'maxpool':
            ellLayers.append(get_pooling_layer(layer, ell.neural.PoolingType.max))
        elif layer['type'] == 'avgpool':
            ellLayers.append(get_pooling_layer(layer, ell.neural.PoolingType.mean))
        elif layer['type'] == 'softmax':
            ellLayers.append(get_softmax_layer(layer))
        elif layer['type'] == 'region':
            ellLayers.append(get_region_detection_layer(layer))
        else:
            _logger.warning("Skipping unrecognized layer %s. The model may still work without it", layer['type'])

    if ellLayers:
        # Darknet expects the input to be between 0 and 1, so prepend
        # a scaling layer with a scale factor of 1/255
        parameters = ellLayers[0].parameters
        ellLayers = [get_first_scaling_layer(parameters)] + ellLayers

    predictor = ell.neural.FloatNeuralNetworkPredictor(ellLayers)
    return predictor


# Function to import a Darknet model and output the corresponding ELL neural network predictor
def predictor_from_darknet_model(modelConfigFile, modelWeightsFile, convolutionOrder = 'cnba'):
    """Loads a Darknet model and returns an ell.neural.NeuralNetworkPredictor
       modelConfigFile - Name of the .cfg file for the Darknet model
       modelWightsFile - Name of the .weights file for the Darknet model
       convolutionOrder - Optional parameter specifying order of operations in a
                          Darknet convolution layer. Typically, this is:
                          Convolution, BatchNormalization, Bias, Activation ('cnba').
                          It can be overridden to Convolution, Bias, Activation, BatchNormalization ('cban').
    """
    # Process the network config file. This gives us the layer structure of the neural network
    network = parse_cfg(modelConfigFile)

    with open(modelWeightsFile, "rb") as weights_file:
        # discard the first 4 ints (4 bytes each)
        weights_file.seek(4 * 4)

        # Create the predictor given the structure of the network and the given weights
        predictor = process_network(network, weights_file, convolutionOrder)

    return predictor
