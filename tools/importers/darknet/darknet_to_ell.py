####################################################################################################
##
##  Project:  Embedded Learning Library (ELL)
##  File:     darknet_to_ell.py (importers)
##  Authors:  Byron Changuion
##
##  Requires: Python 3.x
##
####################################################################################################

import os
import configparser
import re
import struct
import sys, getopt
import numpy as np
import ELL

def convolutional_out_height(layer):
    return (int(layer['h']) + 2*int(layer['padding']) - int(layer['size'])) / int(layer['stride']) + 1

def convolutional_out_width(layer):
    return (int(layer['w']) + 2*int(layer['padding']) - int(layer['size'])) / int(layer['stride']) + 1

def parse_cfg(filename):
    """Parses a Darknet .cfg file and returns a list of layers. Each layer has
       properties denoting type, shape of input and outputs, padding requirements
       and anything else needed to construct up the relevant ELL layers"""
    f = open(filename)
    content = f.read()
    f.close()
    matches = re.findall('(\[.*?\])((.*?)(?=\[))', content, re.DOTALL)
    network = []
    for layer in matches:
        layer_desc = {}
        layer_desc['type'] = layer[0].replace('[', '').replace(']', '')
        param_list = list(filter(None, layer[1].split('\n')))
        for param in param_list:
            if ("=" in param):
                arg, val = param.split('=')
                layer_desc[arg] = val
        network.append(layer_desc)

    # add extra information needed, size calculations and properties like padding
    for i in range(len(network)):
        layer = network[i]
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
            print("convolutional: ", layer['h'], 'x', layer['w'], 'x', layer['c'], '-> ', layer['out_h'], 'x', layer['out_w'], 'x', layer['out_c'], ', pad ', layer['padding'])
        elif layer['type'] == 'connected':
            layer['h'] = network[i-1]['out_h']
            layer['w'] = network[i-1]['out_w']
            layer['c'] = network[i-1]['out_c']
            layer['inputs'] = int(layer['h']) * int(layer['w']) * int(layer['c'])
            layer['out_w'] = 1
            layer['out_h'] = 1
            layer['out_c'] = int(layer['output'])
            print("connected: ", layer['h'], 'x', layer['w'], 'x', layer['c'], '-> ', layer['out_h'], 'x', layer['out_w'], 'x', layer['out_c'])
        elif layer['type'] == 'maxpool':
            if 'padding' not in layer:
                layer['padding'] = int((int(layer['size']) - 1) / 2)
            layer['h'] = int(network[i-1]['out_h'])
            layer['w'] = int(network[i-1]['out_w'])
            layer['c'] = int(network[i-1]['out_c'])
            layer['out_h'] = int(((int(layer['h'])) + 2 * int(layer['padding'])) / int(layer['stride']))
            layer['out_w'] = int(((int(layer['w'])) + 2 * int(layer['padding'])) / int(layer['stride']))
            layer['out_c'] = layer['c']
            print("max_pool: ", layer['h'], 'x', layer['w'], 'x', layer['c'], '-> ', layer['out_h'], 'x', layer['out_w'], 'x', layer['out_c'], ', pad ', layer['padding'])
        elif layer['type'] == 'avgpool':
            layer['c'] = network[i-1]['out_c']
            layer['out_c'] = layer['c']
            layer['out_h'] = 1
            layer['out_w'] = 1
            layer['h'] = network[i-1]['out_h']
            layer['w'] = network[i-1]['out_w']
            layer['padding'] = 0
            # Darknet's mean pooling is accross an entire layer. Fix up stride and size so ELL can behave that way.
            layer['size'] = layer["w"]
            layer['stride'] = layer["w"]
            print("avg_pool: ", layer['h'], 'x', layer['w'], 'x', layer['c'], '-> ', layer['out_h'], 'x', layer['out_w'], 'x', layer['out_c'], ', pad ', layer['padding'])
        elif layer['type'] == 'softmax':
            layer['c'] = network[i-1]['out_c']
            layer['h'] = network[i-1]['out_h']
            layer['w'] = network[i-1]['out_w']
            layer['out_c'] = layer['c']
            layer['out_h'] = layer['h']
            layer['out_w'] = layer['w']
            print("softmax: ", layer['h'], 'x', layer['w'], 'x', layer['c'], '-> ', layer['out_h'], 'x', layer['out_w'], 'x', layer['out_c'])
        elif layer['type'] == 'region':
            layer['c'] = network[i-1]['out_c']
            layer['h'] = network[i-1]['out_h']
            layer['w'] = network[i-1]['out_w']
        else:
            layer['c'] = network[i-1]['out_c']
            layer['h'] = network[i-1]['out_h']
            layer['w'] = network[i-1]['out_w']
            layer['out_c'] = network[i-1]['out_c']
            layer['out_h'] = network[i-1]['out_h']
            layer['out_w'] = network[i-1]['out_w']


    # Do another pass, setting input/output shape and outpadding to next layer's padding
    # Set the ELL padding scheme and shape parameters
    for i in range(len(network)):
        layer = network[i]

        if 'padding' not in layer:
            layer['inputPadding'] = 0
            if layer['type'] == 'maxpool':
                layer['inputPaddingScheme'] = ELL.PaddingScheme.min
            else:
                layer['inputPaddingScheme'] = ELL.PaddingScheme.zeros
        else:
            layer['inputPadding'] = int(layer['padding'])
            if layer['type'] == 'maxpool':
                layer['inputPaddingScheme'] = ELL.PaddingScheme.min
            else:
                layer['inputPaddingScheme'] = ELL.PaddingScheme.zeros
        layer['inputShape'] = ELL.LayerShape(int(layer['h']) + 2 * int(layer['inputPadding']), int(layer['w']) + 2 * int(layer['inputPadding']), int(layer['c']))

        if (i < (len(network) - 1)):
            nextLayer = network[i + 1]
            if 'padding' not in nextLayer:
                layer['outputPadding'] = 0
            else:
                layer['outputPadding'] = int(nextLayer['padding'])

            if nextLayer['type'] == 'maxpool':
                layer['outputPaddingScheme'] = ELL.PaddingScheme.min
            else:
                layer['outputPaddingScheme'] = ELL.PaddingScheme.zeros

            layer['outputShape'] = ELL.LayerShape(int(layer['out_h']) + 2 * int(layer['outputPadding']), int(layer['out_w']) + 2 * int(layer['outputPadding']), int(layer['out_c']))
            layer['outputShapeMinusPadding'] = ELL.LayerShape(int(layer['out_h']), int(layer['out_w']), int(layer['out_c']))
        else:
            layer['outputPadding'] = 0
            layer['outputPaddingScheme'] = ELL.PaddingScheme.zeros
            layer['outputShape'] = ELL.LayerShape(int(layer['out_h']), int(layer['out_w']), int(layer['out_c']))
            layer['outputShapeMinusPadding'] = ELL.LayerShape(int(layer['out_h']), int(layer['out_w']), int(layer['out_c']))

    return network

def create_layer_parameters(inputShape, inputPadding, inputPaddingScheme, outputShape, outputPadding, outputPaddingScheme):
    """Helper function to return ELL.LayerParameters given input and output shapes/padding/paddingScheme"""
    inputPaddingParameters = ELL.PaddingParameters(inputPaddingScheme, inputPadding)
    outputPaddingParameters = ELL.PaddingParameters(outputPaddingScheme, outputPadding)

    return ELL.LayerParameters(inputShape, inputPaddingParameters, outputShape, outputPaddingParameters)

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

    return ELL.FloatTensor(orderedWeights)

def process_batch_normalization_layer(layer, apply_padding, mean_vals, variance_vals, scale_vals):
    """Returns ELL layers corresponding to a Darknet batch normalization layer"""

    # Batch normalization in Darknet corresponds to BatchNormalizationLayer, ScalingLayer in ELL
    layers = []

    # Create BatchNormalizationLayer
    layerParameters = create_layer_parameters(layer['outputShapeMinusPadding'], 0, ELL.PaddingScheme.zeros, layer['outputShapeMinusPadding'], 0, ELL.PaddingScheme.zeros)
    meanVector = ELL.FloatVector(mean_vals.ravel())
    varianceVector = ELL.FloatVector(variance_vals.ravel())

    layers.append(ELL.FloatBatchNormalizationLayer(layerParameters, meanVector, varianceVector, 1e-6, ELL.EpsilonSummand_sqrtVariance))

    # Create Scaling Layer
    if (apply_padding):
        layerParameters = create_layer_parameters(layer['outputShapeMinusPadding'], 0, ELL.PaddingScheme.zeros, layer['outputShape'], layer['outputPadding'], layer['outputPaddingScheme'])
    else:
        layerParameters = create_layer_parameters(layer['outputShapeMinusPadding'], 0, ELL.PaddingScheme.zeros, layer['outputShapeMinusPadding'], 0, ELL.PaddingScheme.zeros)

    layers.append(ELL.FloatScalingLayer(layerParameters, scale_vals.ravel()))

    return layers

def get_activation_type(layer):
    """Returns an ELL.ActivationType from the layer"""
    if (layer["activation"] == 'relu'):
        return ELL.ActivationType.relu
    elif (layer["activation"] == 'sigmoid'):
        return ELL.ActivationType.sigmoid
    elif (layer["activation"] == 'leaky'):
        return ELL.ActivationType.leaky

    return None

def get_activation_layer(layer, apply_padding):
    """Return an ELL activation layer from a darknet activation"""
    if (apply_padding):
        layerParameters = create_layer_parameters(layer['outputShapeMinusPadding'], 0, ELL.PaddingScheme.zeros, layer['outputShape'], layer['outputPadding'], layer['outputPaddingScheme'])
    else:
        layerParameters = create_layer_parameters(layer['outputShapeMinusPadding'], 0, ELL.PaddingScheme.zeros, layer['outputShapeMinusPadding'], 0, ELL.PaddingScheme.zeros)

    activationType = get_activation_type(layer)

    return ELL.FloatActivationLayer(layerParameters, activationType)

def get_bias_layer(layer, apply_padding, bias_vals):
    """Return an ELL bias layer from a darknet layer"""

    if (apply_padding):
        layerParameters = create_layer_parameters(layer['outputShapeMinusPadding'], 0, ELL.PaddingScheme.zeros, layer['outputShape'], layer['outputPadding'], layer['outputPaddingScheme'])
    else:
        layerParameters = create_layer_parameters(layer['outputShapeMinusPadding'], 0, ELL.PaddingScheme.zeros, layer['outputShapeMinusPadding'], 0, ELL.PaddingScheme.zeros)

    biasVector = ELL.FloatVector(bias_vals.ravel())

    return ELL.FloatBiasLayer(layerParameters, biasVector)

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
            mean_vals.append(struct.unpack('f'   , bin_data.read(4)))
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


    layerParameters = create_layer_parameters(layer['inputShape'], layer['inputPadding'], layer['inputPaddingScheme'], layer['outputShapeMinusPadding'], 0, ELL.PaddingScheme.zeros)
    convolutionWeightsTensor = get_weights_tensor((int(layer['filters']), layer['c'], int(layer["size"]), int(layer["size"])), weight_vals)

    # Create the appropriate convolutional layer
    if 'xnor' not in layer:
        # Create the ELL convolutional layer
        convolutionalParameters = ELL.ConvolutionalParameters(int(layer["size"]), int(layer["stride"]), ELL.ConvolutionMethod.columnwise, int(layer['filters']))
        layers.append(ELL.FloatConvolutionalLayer(layerParameters, convolutionalParameters, convolutionWeightsTensor))
    else:
        # Create the ELL binary convolutional layer
        convolutionalParameters = ELL.BinaryConvolutionalParameters(int(layer["size"]), int(layer["stride"]), ELL.BinaryConvolutionMethod.bitwise)
        layers.append(ELL.FloatBinaryConvolutionalLayer(layerParameters, convolutionalParameters, convolutionWeightsTensor))

    # Override global ordering with layer-specific ordering
    if ('order' in layer):
        convolution_order = 	layer['order']

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
    poolingParameters = ELL.PoolingParameters(int(layer["size"]), int(layer["stride"]))
    
    return ELL.FloatPoolingLayer(layerParameters, poolingParameters, poolingType)

def get_softmax_layer(layer):
    """Returns ELL softmax layer from Darknet softmax layer"""

    # Create the ELL pooling layer
    layerParameters = create_layer_parameters(layer['inputShape'], layer['inputPadding'], layer['inputPaddingScheme'], layer['outputShape'], layer['outputPadding'], layer['outputPaddingScheme'])
    
    return ELL.FloatSoftmaxLayer(layerParameters)

def process_fully_connected_layer(layer, weightsData):
    """Returns ELL layers corresponding to a Darknet connected layer"""

    # Connected layer in Darknet corresponds to FullyConnectedLayer, ActivationLayer in ELL
    layers = []

    # Create Fully Connected
    activationType = get_activation_type(layer)
    if (activationType is None):
        layerParameters = create_layer_parameters(layer['outputShapeMinusPadding'], 0, ELL.PaddingScheme.zeros, layer['outputShape'], layer['outputPadding'], layer['outputPaddingScheme'])
    else:
        layerParameters = create_layer_parameters(layer['outputShapeMinusPadding'], 0, ELL.PaddingScheme.zeros, layer['outputShapeMinusPadding'], 0, ELL.PaddingScheme.zeros)

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

    weightsTensor = ELL.FloatTensor(orderedWeights)

    layers.append(ELL.FloatFullyConnectedLayer(layerParameters, weightsTensor))

    if (activationType is not None):
        # Create BiasLayer
        layers.append(get_bias_layer(layer, False, bias_vals))
        # Create ActivationLayer
        layers.append(get_activation_layer(layer, True))
    else:
        # Create BiasLayer
        layers.append(get_bias_layer(layer, True, bias_vals))

    return layers

def process_network(network, weightsData, convolutionOrder):
    """Returns an ELL.FloatNeuralNetworkPredictor as a result of parsing the network layers"""
    ellLayers = []

    for layer in network:
        if layer['type'] == 'net':
            pass
        elif layer['type'] == 'convolutional':
            ellLayers += process_convolutional_layer(layer, weightsData, convolutionOrder)
        elif layer['type'] == 'connected':
            ellLayers += process_fully_connected_layer(layer, weightsData)
        elif layer['type'] == 'maxpool':
            ellLayers.append(get_pooling_layer(layer, ELL.PoolingType.max))
        elif layer['type'] == 'avgpool':
            ellLayers.append(get_pooling_layer(layer, ELL.PoolingType.mean))
        elif layer['type'] == 'softmax':
            ellLayers.append(get_softmax_layer(layer))
        else:
            print("Skipping, ", layer['type'], "layer")
            print()

    predictor = ELL.FloatNeuralNetworkPredictor(ellLayers)
    return predictor

# Function to import a Darknet model and output the corresponding ELL neural network predictor
def predictor_from_darknet_model(modelConfigFile, modelWeightsFile, convolutionOrder = 'cnba'):
    """Loads a Darknet model and returns an ELL.NeuralNetworkPredictor
       modelConfigFile - Name of the .cfg file for the Darknet model
       modelWightsFile - Name of the .weights file for the Darknet model
       convolutionOrder - Optional parameter specifying order of operations in a 
                          Darknet convolution layer. Typically, this is: 
                          Convolution, BatchNormalization, Bias, Activation ('cnba').
                          It can be overridden to Convolution, Bias, Activation, BatchNormalization ('cban').
    """
    predictor = None

    # Process the network config file. This gives us the layer structure of the neural network
    network = parse_cfg(modelConfigFile)
    weightsData = open(modelWeightsFile, "rb")
    try:
        # discard the first 4 ints (4 bytes each)
        weightsData.read(4 * 4)

        # Create the predictor given the structure of the network and the given weights
        predictor = process_network(network, weightsData, convolutionOrder)
    finally:
        weightsData.close()

    return predictor
