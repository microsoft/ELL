####################################################################################################
#
# Project:  Embedded Learning Library (ELL)
# File:     cntk_to_ell.py (importers)
# Authors:  Byron Changuion
#
# Requires: Python 3.x, cntk-2.0-cp35
#
####################################################################################################
import ELL
import sys
import getopt
import os
import configparser
import re
import struct
from cntk.layers import Convolution, MaxPooling, AveragePooling, Dropout, BatchNormalization, Dense
import numpy as np
from cntk.ops import *
from cntk.initializer import glorot_uniform, he_normal
from cntk import load_model
import cntk.layers.blocks
from cntk.logging.graph import *
from cntk.layers.typing import *
import traceback


def get_model_layers(root):
    """Returns a list of the high-level layers (.e. function blocks) that make up the CNTK model """
    stack = [root.root_function]  # node
    layers = []         # final result, list of all relevant layers
    visited = set()

    while stack:
        node = stack.pop(0)
        from cntk import cntk_py
        try:
            # Function node
            stack = list(node.root_function.inputs) + stack
        except AttributeError:
            # OutputVariable node. We need process the owner node if this is an output.
            try:
                if node.is_output:
                    stack.insert(0, node.owner)
                    continue
            except AttributeError:
                pass
        # Add function nodes but skip Variable nodes
        if not isinstance(node, Variable):
            layers.append(node)

    # CNTK layers are in opposite order to what ELL wants, so reverse the list
    layers.reverse()

    return layers


def opNameEquals(node, name):
    result = False
    if hasattr(node, 'op_name'):
        result = (node.op_name == name)

    return result


def findParameterByName(parameters, name, index=0):
    for p in parameters:
        if (p.name == name):
            return p
    # Fallback case: Sometimes parameters are renamed.
    # Convention is to end with the original name e.g.
    # if weights are normally "W", a renamed weights parameters
    # is something like "conv2_2.W".
    for p in parameters:
        if (p.name.endswith(name)):
            return p
    # If no named parameter was found, just return the one at the
    # specified index
    return parameters[index]


def findNodeByOp(parameters, name):
    for p in parameters:
        if (p.op_name == name):
            return p
    return None


def is_softmax_activation(nodes):
    """Returns True is the nodes contain a softmax activation"""
    if (findNodeByOp(nodes, 'SoftMax') != None):
        return True
    return False


def get_activation_type(nodes):
    """Returns an ELL.ActivationType from the list of nodes"""
    if (findNodeByOp(nodes, 'ReLU') != None):
        return ELL.ActivationType.relu
    elif (findNodeByOp(nodes, 'Sigmoid') != None):
        return ELL.ActivationType.sigmoid
    elif (findNodeByOp(nodes, 'LeakyReLU') != None):
        return ELL.ActivationType.leaky

    return None


def get_float_vector_from_cntk_trainable_parameter(tensorParameter):
    """Returns an ELL.FloatVector from a trainable parameter
       Note that ELL's ordering is row, column, channel.
       CNTK has them in filter, channel, row, column order.
    """
    tensorShape = tensorParameter.shape
    tensorValue = tensorParameter.value

    orderedWeights = np.zeros(tensorValue.size, dtype=np.float)
    i = 0
    for columnValue in tensorValue:
        orderedWeights[i] = columnValue
        i += 1

    return ELL.FloatVector(orderedWeights)


def get_float_vector_from_cntk_array(inputArray):
    """Returns an ELL.FloatTensor from a 4, 3, 2, or 1 dimensional numpy array.
       CNTK has input in filter/parallel, channel, row, column order while
       ELL's ordering is row, column, channel.
    """
    tensorShape = inputArray.shape
    orderedWeights = np.zeros(inputArray.size, dtype=np.float)
    if (len(tensorShape) == 4):
        i = 0
        for filter in range(tensorShape[0]):
            for row in range(tensorShape[2]):
                for column in range(tensorShape[3]):
                    for channel in range(tensorShape[1]):
                        orderedWeights[i] = inputArray[filter][channel][row][column]
                        i += 1
        # Reshape to (filters * rows, columns, channels)
        orderedWeights = orderedWeights.reshape(
            tensorShape[0] * tensorShape[2], tensorShape[3], tensorShape[1])
    elif (len(tensorShape) == 3):
        i = 0
        for row in range(tensorShape[1]):
            for column in range(tensorShape[2]):
                for channel in range(tensorShape[0]):
                    orderedWeights[i] = inputArray[channel][row][column]
                    i += 1
        # Reshape to (rows, columns, channels)
        orderedWeights = orderedWeights.reshape(
            tensorShape[1], tensorShape[2], tensorShape[0])
    elif (len(tensorShape) == 2):
        i = 0
        for row in range(tensorShape[1]):
            for column in range(tensorShape[0]):
                orderedWeights[i] = inputArray[column][row]
                i += 1
        # Reshape to (rows, 1, channels)
        orderedWeights = orderedWeights.reshape(
            tensorShape[1], 1, tensorShape[0])
    elif (len(tensorShape) == 1):
        i = 0
        for columnValue in inputArray:
            orderedWeights[i] = columnValue
            i += 1
        # Reshape to (1, 1, channels)
        orderedWeights = orderedWeights.reshape(1, 1, tensorValue.size)
    else:
        print("Error: Input array has incorrect dimensions")
        return None

    return np.ravel(orderedWeights)


def get_float_tensor_from_cntk_dense_weight_parameter(tensorParameter):
    """Returns an ELL.FloatTensor from a trainable parameter
       Note that ELL's ordering is row, column, channel.
       CNTK has them in channel, row, column, filter order.
       4D parameters are converted to ELL Tensor by stacking vertically in the row dimension.
    """
    tensorShape = tensorParameter.shape
    tensorValue = tensorParameter.value

    # orderedWeights = tensorValue
    if (len(tensorShape) == 4):
        orderedWeights = tensorValue
        orderedWeights = np.moveaxis(orderedWeights, 0, -1)
        orderedWeights = np.moveaxis(orderedWeights, 2, 0)
        orderedWeights = orderedWeights.ravel().astype(np.float).reshape(
            tensorShape[3] * tensorShape[1], tensorShape[2], tensorShape[0])
    elif (len(tensorShape) == 3):
        orderedWeights = np.moveaxis(tensorValue, 0, -1)
        orderedWeights = orderedWeights.ravel().astype(np.float).reshape(
            tensorShape[1], tensorShape[2], tensorShape[0])
    elif (len(tensorShape) == 2):
        orderedWeights = np.moveaxis(tensorValue, 0, -1)
        orderedWeights = orderedWeights.ravel().astype(
            np.float).reshape(tensorShape[1], 1, tensorShape[0])
    else:
        orderedWeights = tensorValue.ravel().astype(
            np.float).reshape(1, 1, tensorValue.size)

    return ELL.FloatTensor(orderedWeights)


def get_float_tensor_from_cntk_convolutional_weight_parameter(tensorParameter):
    """Returns an ELL.FloatTensor from a trainable parameter
       Note that ELL's ordering is row, column, channel.
       4D parameters (e.g. those that represent convolutional weights) are stacked vertically in the row dimension.
       CNTK has them in filter, channel, row, column order.
    """
    tensorShape = tensorParameter.shape
    tensorValue = tensorParameter.value

    if (len(tensorShape) == 4):
        orderedWeights = np.moveaxis(tensorValue, 1, -1)
        orderedWeights = orderedWeights.ravel().astype(np.float).reshape(
            tensorShape[0] * tensorShape[2], tensorShape[3], tensorShape[1])
    elif (len(tensorShape) == 3):
        orderedWeights = np.moveaxis(tensorValue, 0, -1)
        orderedWeights = orderedWeights.ravel().astype(np.float).reshape(
            tensorShape[1], tensorShape[2], tensorShape[0])
    elif (len(tensorShape) == 2):
        orderedWeights = np.moveaxis(tensorValue, 0, -1)
        orderedWeights = orderedWeights.ravel().astype(
            np.float).reshape(tensorShape[1], tensorShape[0], 1)
    else:
        orderedWeights = tensorValue.ravel().astype(
            np.float).reshape(1, 1, tensorValue.size)
    return ELL.FloatTensor(orderedWeights)

def get_convolutional_layer_info(layer):
    """Returns information about a CNTK Convolutional layer used for converting it to ELL's equivalent."""
    if layer.is_block:
        inputParameter = layer.arguments[0]
        weightsParameter = findParameterByName(layer.parameters, 'W', 0)
        binarized = False
    else:
        # Convolution function (assume part of a Binary Convolution layer)
        # - Weights is 4-dimensional (filters, channels, rows, columns)
        # - Input is 3-dimensional (channels, rows, columns)
        if len(layer.inputs[0].shape) == 3:
            inputParameter = layer.inputs[0]
            weightsParameter = layer.inputs[1]
        else:
            inputParameter = layer.inputs[1]
            weightsParameter = layer.inputs[0]
        binarized = True
    return inputParameter, weightsParameter, binarized


def process_convolutional_layer(layer, ellLayers):
    if not layer.is_block:
        print("Error: Convolution node is not a block node")
        return
    # Note that a single CNTK Convolutional function block is equivalent to the following 3 ELL layers:
    # - ConvolutionalLayer
    # - BiasLayer
    # - ActivationLayer. This layer is sometimes missing, depending on activation type.
    #
    # Therefore, make sure the output padding characteristics of the last layer reflect the next layer's
    # padding requirements.

    # Get the hyper-parameters for the convolution.
    # They are on the convolution node inside this block.
    convolutionNodes = depth_first_search(
        layer.block_root, lambda x: opNameEquals(x, 'Convolution'))
    if len(convolutionNodes) > 0:
        convolutionAttributes = convolutionNodes[0].attributes
        convolutionParameters = layer.parameters

        weightsParameter = findParameterByName(convolutionParameters, 'W', 0)
        weightsShape = weightsParameter.shape
        biasParameter = findParameterByName(convolutionParameters, 'b', 1)

        weightsTensor = get_float_tensor_from_cntk_convolutional_weight_parameter(
            weightsParameter)
        biasVector = get_float_vector_from_cntk_trainable_parameter(
            biasParameter)

        # Create the ELL.LayerParameters for the various ELL layers
        firstLayerParameters = ELL.LayerParameters(
            layer.ell_inputShape, layer.ell_inputPaddingParameters, layer.ell_outputShapeMinusPadding, ELL.NoPadding())
        middleLayerParameters = ELL.LayerParameters(layer.ell_outputShapeMinusPadding, ELL.NoPadding(
        ), layer.ell_outputShapeMinusPadding, ELL.NoPadding())
        lastLayerParameters = ELL.LayerParameters(layer.ell_outputShapeMinusPadding, ELL.NoPadding(
        ), layer.ell_outputShape, layer.ell_outputPaddingParameters)

        layerParameters = firstLayerParameters

        # Fill in the convolutional parameters
        receptiveField = weightsShape[2]
        stride = convolutionAttributes['strides'][2]
        convolutionMethod = 0
        filterBatchSize = layerParameters.outputShape.channels

        internalNodes = get_model_layers(layer.block_root)
        activationType = get_activation_type(internalNodes)

        convolutionalParameters = ELL.ConvolutionalParameters(
            receptiveField, stride, convolutionMethod, filterBatchSize)

        # Create the ELL convolutional layer
        ellLayers.append(ELL.FloatConvolutionalLayer(
            layerParameters, convolutionalParameters, weightsTensor))

        # Create the ELL bias layer
        if (is_softmax_activation(internalNodes) or activationType != None):
            layerParameters = middleLayerParameters
        else:
            layerParameters = lastLayerParameters
        ellLayers.append(ELL.FloatBiasLayer(layerParameters, biasVector))

        # Create the ELL activation layer
        if (is_softmax_activation(internalNodes) or activationType != None):
            layerParameters = lastLayerParameters

            # Special case: if this is softmax activation, create an ELL Softmax layer.
            # Else, insert an ELL ActivationLayer
            if(is_softmax_activation(internalNodes)):
                ellLayers.append(ELL.FloatSoftmaxLayer(layerParameters))
            else:
                if (activationType != None):
                    ellLayers.append(ELL.FloatActivationLayer(
                        layerParameters, activationType))
    else:
        print("Error: Could not find the Convolution function in the Convolution block")

    return

def process_binary_convolutional_layer(layer, ellLayers):
    if layer.is_block:
        print("Error: Convolution node is in block node")
        return

    raise NotImplementedError("Error: Not yet implemented")
    return

def process_dense_layer(layer, ellLayers):
    if not layer.is_block:
        print("Error: Dense node is not a block node")
        return
    # Note that a single CNTK Dense function block is equivalent to the following 3 ELL layers:
    # - FullyConnectedLayer
    # - BiasLayer
    # - ActivationLayer. This layer is sometimes missing, depending on activation type.
    #
    # Therefore, make sure the output padding characteristics of the last layer reflect the next layer's
    # padding requirements.

    weightsParameter = findParameterByName(layer.parameters, 'W', 0)
    biasParameter = findParameterByName(layer.parameters, 'b', 1)
    weightsTensor = get_float_tensor_from_cntk_dense_weight_parameter(
        weightsParameter)
    biasVector = get_float_vector_from_cntk_trainable_parameter(biasParameter)

    # Create the ELL.LayerParameters for the various ELL layers
    firstLayerParameters = ELL.LayerParameters(
        layer.ell_inputShape, layer.ell_inputPaddingParameters, layer.ell_outputShapeMinusPadding, ELL.NoPadding())
    middleLayerParameters = ELL.LayerParameters(layer.ell_outputShapeMinusPadding, ELL.NoPadding(
    ), layer.ell_outputShapeMinusPadding, ELL.NoPadding())
    lastLayerParameters = ELL.LayerParameters(layer.ell_outputShapeMinusPadding, ELL.NoPadding(
    ), layer.ell_outputShape, layer.ell_outputPaddingParameters)

    layerParameters = firstLayerParameters

    internalNodes = get_model_layers(layer.block_root)
    activationType = get_activation_type(internalNodes)

    # Create the ELL fully connected layer
    ellLayers.append(ELL.FloatFullyConnectedLayer(
        layerParameters, weightsTensor))

    # Create the ELL bias layer
    if (is_softmax_activation(internalNodes) or activationType != None):
        layerParameters = middleLayerParameters
    else:
        layerParameters = lastLayerParameters
    ellLayers.append(ELL.FloatBiasLayer(layerParameters, biasVector))

    # Create the ELL activation layer
    if (is_softmax_activation(internalNodes) or activationType != None):
        layerParameters = lastLayerParameters

        # Special case: if this is softmax activation, create an ELL Softmax layer.
        # Else, insert an ELL ActivationLayer
        if(is_softmax_activation(internalNodes)):
            ellLayers.append(ELL.FloatSoftmaxLayer(layerParameters))
        else:
            if (activationType != None):
                ellLayers.append(ELL.FloatActivationLayer(
                    layerParameters, activationType))

    return


def process_linear_layer(layer, ellLayers):

    # Note that a single CNTK Linear function block is equivalent to the following 3 ELL layers:
    # - FullyConnectedLayer
    # - BiasLayer
    # - ActivationLayer. This layer is sometimes missing, depending on activation type.
    #
    # Therefore, make sure the output padding characteristics of the last layer reflect the next layer's
    # padding requirements.

    weightsParameter = findParameterByName(layer.parameters, 'W', 0)
    biasParameter = findParameterByName(layer.parameters, 'b', 1)
    weightsTensor = get_float_tensor_from_cntk_dense_weight_parameter(
        weightsParameter)
    biasVector = get_float_vector_from_cntk_trainable_parameter(biasParameter)

    # Create the ELL.LayerParameters for the various ELL layers
    firstLayerParameters = ELL.LayerParameters(
        layer.ell_inputShape, layer.ell_inputPaddingParameters, layer.ell_outputShapeMinusPadding, ELL.NoPadding())
    middleLayerParameters = ELL.LayerParameters(layer.ell_outputShapeMinusPadding, ELL.NoPadding(
    ), layer.ell_outputShapeMinusPadding, ELL.NoPadding())
    lastLayerParameters = ELL.LayerParameters(layer.ell_outputShapeMinusPadding, ELL.NoPadding(
    ), layer.ell_outputShape, layer.ell_outputPaddingParameters)

    layerParameters = firstLayerParameters

    internalNodes = get_model_layers(layer.block_root)
    activationType = get_activation_type(internalNodes)

    # Create the ELL fully connected layer
    ellLayers.append(ELL.FloatFullyConnectedLayer(
        layerParameters, weightsTensor))

    # Create the ELL bias layer
    if (is_softmax_activation(internalNodes) or activationType != None):
        layerParameters = middleLayerParameters
    else:
        layerParameters = lastLayerParameters
    ellLayers.append(ELL.FloatBiasLayer(layerParameters, biasVector))

    # Create the ELL activation layer
    if (is_softmax_activation(internalNodes) or activationType != None):
        layerParameters = lastLayerParameters

        # Special case: if this is softmax activation, create an ELL Softmax layer.
        # Else, insert an ELL ActivationLayer
        if(is_softmax_activation(internalNodes)):
            ellLayers.append(ELL.FloatSoftmaxLayer(layerParameters))
        else:
            if (activationType != None):
                ellLayers.append(ELL.FloatActivationLayer(
                    layerParameters, activationType))

    return


def process_element_times_layer(layer, ellLayers):
    if (len(layer.constants) == 1):

        # Create the ELL.LayerParameters for the ELL layer
        layerParameters = ELL.LayerParameters(
            layer.ell_inputShape, layer.ell_inputPaddingParameters, layer.ell_outputShape, layer.ell_outputPaddingParameters)

        # Create ELL scaling layer
        scaleValue = layer.constants[0].value
        # Workaround: For some reason, np.full is not returning a type that SWIG can parse. So just manually walk the array setting the scalar
        scaleArray = np.arange(
            layerParameters.outputShape.channels, dtype=np.float)
        for i in range(scaleArray.size):
            scaleArray[i] = scaleValue
        scalesVector = ELL.FloatVector(scaleArray)
        ellLayers.append(ELL.FloatScalingLayer(layerParameters, scalesVector))

    else:
        printf("Error: Skipping ElementTimes layer due to dimenions of Constants")
    return


def process_max_pooling_layer(layer, ellLayers):

    if not layer.is_block:
        print("Error: Pooling node is not a block node")
        return

    # Create the ELL.LayerParameters for the ELL layer
    layerParameters = ELL.LayerParameters(
        layer.ell_inputShape, layer.ell_inputPaddingParameters, layer.ell_outputShape, layer.ell_outputPaddingParameters)

    # Fill in the pooling parameters
    attributes = layer.block_root.attributes

    poolingSize = attributes['poolingWindowShape'][0]
    stride = attributes['strides'][0]

    poolingParameters = ELL.PoolingParameters(poolingSize, stride)

    # Create the ELL max pooling layer
    ellLayers.append(ELL.FloatPoolingLayer(
        layerParameters, poolingParameters, ELL.PoolingType.max))

    return


def process_average_pooling_layer(layer, ellLayers):

    if not layer.is_block:
        print("Error: Pooling node is not a block node")
        return

    # Create the ELL.LayerParameters for the ELL layer
    layerParameters = ELL.LayerParameters(
        layer.ell_inputShape, layer.ell_inputPaddingParameters, layer.ell_outputShape, layer.ell_outputPaddingParameters)

    # Fill in the pooling parameters
    attributes = layer.block_root.attributes

    poolingSize = attributes['poolingWindowShape'][0]
    stride = attributes['strides'][0]

    poolingParameters = ELL.PoolingParameters(poolingSize, stride)

    # Create the ELL mean pooling layer
    ellLayers.append(ELL.FloatPoolingLayer(
        layerParameters, poolingParameters, ELL.PoolingType.mean))

    return


def process_pooling_layer(layer, ellLayers):

    # Create the ELL.LayerParameters for the ELL layer
    layerParameters = ELL.LayerParameters(
        layer.ell_inputShape, layer.ell_inputPaddingParameters, layer.ell_outputShape, layer.ell_outputPaddingParameters)

    # Fill in the pooling parameters
    attributes = layer.attributes

    poolingSize = attributes['poolingWindowShape'][0]
    stride = attributes['strides'][0]

    poolingParameters = ELL.PoolingParameters(poolingSize, stride)

    # Check which pooling layer to create
    if (attributes['poolingType'] == PoolingType_Max):
        # Create the ELL max pooling layer
        ellLayers.append(ELL.FloatPoolingLayer(
            layerParameters, poolingParameters, ELL.PoolingType.max))
    else:
        # Create the ELL mean pooling layer
        ellLayers.append(ELL.FloatPoolingLayer(
            layerParameters, poolingParameters, ELL.PoolingType.mean))

    return


def process_relu_layer(layer, ellLayers):
    # Create the ELL.LayerParameters for the ELL layer
    layerParameters = ELL.LayerParameters(
        layer.ell_inputShape, layer.ell_inputPaddingParameters, layer.ell_outputShape, layer.ell_outputPaddingParameters)

    # Create the ELL activation layer
    ellLayers.append(ELL.FloatActivationLayer(
        layerParameters, ELL.ActivationType.relu))

    return


def process_leakyrelu_layer(layer, ellLayers):
    # Create the ELL.LayerParameters for the ELL layer
    layerParameters = ELL.LayerParameters(
        layer.ell_inputShape, layer.ell_inputPaddingParameters, layer.ell_outputShape, layer.ell_outputPaddingParameters)

    # Create the ELL activation layer
    ellLayers.append(ELL.FloatActivationLayer(
        layerParameters, ELL.ActivationType.leaky))

    return


def process_softmax_layer(layer, ellLayers):
    # Create the ELL.LayerParameters for the ELL layer
    layerParameters = ELL.LayerParameters(
        layer.ell_inputShape, layer.ell_inputPaddingParameters, layer.ell_outputShape, layer.ell_outputPaddingParameters)

    # Create the ELL max pooling layer
    ellLayers.append(ELL.FloatSoftmaxLayer(layerParameters))

    return


def process_batch_normalization_layer(layer, ellLayers):
    # Note that a single CNTK Batch Normalization layer is equivalent to the following 3 ELL layers:
    # - BatchNormalizationLayer
    # - ScalingLayer
    # - BiasLayer
    #
    # Therefore, make sure the output padding characteristics of the last layer reflect the next layer's
    # padding requirements.

    scaleParameter = findParameterByName(layer.parameters, 'scale', 0)
    biasParameter = findParameterByName(layer.parameters, 'bias', 1)
    meanParameter = findParameterByName(layer.constants, 'aggregate_mean', 0)
    varianceParameter = findParameterByName(layer.constants, 'aggregate_variance', 1)

    scaleVector = get_float_vector_from_cntk_trainable_parameter(scaleParameter)
    biasVector = get_float_vector_from_cntk_trainable_parameter(biasParameter)
    meanVector = get_float_vector_from_cntk_trainable_parameter(meanParameter)
    varianceVector = get_float_vector_from_cntk_trainable_parameter(varianceParameter)

    # Create the ELL.LayerParameters for the various ELL layers
    firstLayerParameters = ELL.LayerParameters(
        layer.ell_inputShape, layer.ell_inputPaddingParameters, layer.ell_outputShapeMinusPadding, ELL.NoPadding())
    middleLayerParameters = ELL.LayerParameters(layer.ell_outputShapeMinusPadding, ELL.NoPadding(
    ), layer.ell_outputShapeMinusPadding, ELL.NoPadding())
    lastLayerParameters = ELL.LayerParameters(layer.ell_outputShapeMinusPadding, ELL.NoPadding(
    ), layer.ell_outputShape, layer.ell_outputPaddingParameters)

    # The default CNTK epsilon
    epsilon = 1e-5

    # Create the layers
    ellLayers.append(ELL.FloatBatchNormalizationLayer(firstLayerParameters, meanVector, varianceVector, epsilon, ELL.EpsilonSummand_variance))
    ellLayers.append(ELL.FloatScalingLayer(middleLayerParameters, scaleVector))
    ellLayers.append(ELL.FloatBiasLayer(lastLayerParameters, biasVector))

    return


def convert_cntk_layers_to_ell_layers(layersToConvert):
    """Walks a list of CNTK layers and returns a list of ELL Layer objects that is used to construct a Neural Network Predictor"""

    print("\nConstructing equivalent ELL layers from CNTK...")
    ellLayers = []
    for cntkLayer in layersToConvert:
        print("Converting layer ", cntkLayer)
        if (cntkLayer.op_name == 'Convolution'):
            if (cntkLayer.ell_binarized):
                process_binary_convolutional_layer(cntkLayer, ellLayers)
            else:
                process_convolutional_layer(cntkLayer, ellLayers)
        elif (cntkLayer.op_name == 'Dense'):
            process_dense_layer(cntkLayer, ellLayers)
        elif (cntkLayer.op_name == 'linear'):
            process_linear_layer(cntkLayer, ellLayers)
        elif (cntkLayer.op_name == 'ElementTimes'):
            process_element_times_layer(cntkLayer, ellLayers)
        elif (cntkLayer.op_name == 'MaxPooling'):
            process_max_pooling_layer(cntkLayer, ellLayers)
        elif (cntkLayer.op_name == 'AveragePooling'):
            process_average_pooling_layer(cntkLayer, ellLayers)
        elif (cntkLayer.op_name == 'Pooling'):
            process_pooling_layer(cntkLayer, ellLayers)
        elif (cntkLayer.op_name == 'ReLU'):
            process_relu_layer(cntkLayer, ellLayers)
        elif (cntkLayer.op_name == 'LeakyReLU'):
            process_leakyrelu_layer(cntkLayer, ellLayers)
        elif (cntkLayer.op_name == 'Softmax'):
            process_softmax_layer(cntkLayer, ellLayers)
        elif (cntkLayer.op_name == 'BatchNormalization'):
            process_batch_normalization_layer(cntkLayer, ellLayers)
    print("\n...Finished constructing ELL layers.")

    return ellLayers


def get_input_padding_parameters_for_layer(layer):
    """Returns the ELL.PaddingParameters for a layer's input"""
    paddingScheme = ELL.PaddingScheme.zeros
    padding = 0

    if (layer.op_name == 'Convolution'):
        if (layer.is_block):
            convolutionNodes = depth_first_search(
                layer.block_root, lambda x: opNameEquals(x, 'Convolution'))
            attributes = convolutionNodes[0].attributes
        else:
            attributes = layer.attributes

        inputParameter, weightsParameter, binarized = get_convolutional_layer_info(layer)
        receptiveField = weightsParameter.shape[2]

        if ('autoPadding' in attributes):
            if (attributes['autoPadding'][1] == True):
                padding = int((receptiveField - 1) / 2)
            else:
                padding = attributes['upperPad'][0]
        else:
            padding = attributes['upperPad'][0]
            
    elif (layer.op_name == 'MaxPooling'):
        paddingScheme = ELL.PaddingScheme.min
        attributes = layer.block_root.attributes
        if ('autoPadding' in attributes):
            if (attributes['autoPadding'][0] == True):
                padding = int((attributes['poolingWindowShape'][0] - 1) / 2)
            else:
                padding = attributes['upperPad'][0]
        else:
            padding = attributes['upperPad'][0]
    elif (layer.op_name == 'AveragePooling'):
        paddingScheme = ELL.PaddingScheme.zeros
        attributes = layer.block_root.attributes
        if ('autoPadding' in attributes):
            if (attributes['autoPadding'][0] == True):
                padding = int((attributes['poolingWindowShape'][0] - 1) / 2)
            else:
                padding = attributes['upperPad'][0]
        else:
            padding = attributes['upperPad'][0]
    elif (layer.op_name == 'Pooling'):
        attributes = layer.attributes
        if (attributes['poolingType'] == PoolingType_Max):
            paddingScheme = ELL.PaddingScheme.min
        else:
            paddingScheme = ELL.PaddingScheme.zeros

        if ('autoPadding' in attributes):
            if (attributes['autoPadding'][0] == True):
                padding = int((attributes['poolingWindowShape'][0] - 1) / 2)
            else:
                padding = attributes['upperPad'][0]
        else:
            padding = attributes['upperPad'][0]

    return ELL.PaddingParameters(paddingScheme, padding)


def get_shape_for_layer(inputShape):
    """"Returns the ELL.LayerShape corresponding to the output shape with no adjustment for padding"""

    if (len(inputShape) == 3):
        # CNTK's shape tensor is in channels, rows, columns order
        channels = inputShape[0]
        rows = inputShape[1]
        columns = inputShape[2]
    elif (len(inputShape) == 1):
        # If the input shape is a vector, make it a tensor with 1 row, 1 column and number of channels equal to the length of the vector
        channels = inputShape[0]
        rows = 1
        columns = 1

    return ELL.LayerShape(rows, columns, channels)


def get_adjusted_shape_for_layer(inputShape, paddingParameters):
    """"Returns the ELL.LayerShape corresponding to the input shape adjusted with padding"""

    if (len(inputShape) == 3):
        # Adjust the input shape to account for padding in the row and column dimensions
        # CNTK's shape tensor is in channels, rows, columns order
        channels = inputShape[0]
        rows = inputShape[1]
        columns = inputShape[2]

        rows += 2 * paddingParameters.paddingSize
        columns += 2 * paddingParameters.paddingSize
    elif (len(inputShape) == 1):
        # If the input shape is a vector, make it a tensor with 1 row, 1 column and number of channels equal to the length of the vector
        channels = inputShape[0]
        rows = 1
        columns = 1
    else:
        raise NotImplementedError("Unsupported input shape length: " + str(len(inputShape)))

    return ELL.LayerShape(rows, columns, channels)


def ell_shape_to_string(shape):
    return (str(shape.rows) + "x" + str(shape.columns) + "x" + str(shape.channels))


def get_filtered_layers_list(modelLayers):
    """Returns a relevant list of CNTK layers, which have had the following
       fields added to help with ELL processing:

       ell_inputShape - dimensions of input adjusted for padding
       ell_inputPaddingParameters - padding scheme and size for input
       ell_outputShape - dimensions of output adjusted for padding
       ell_outputPaddingParameters - padding scheme and size for output
       ell_outputShapeMinusPadding - shape of output before padding adjustment
       ell_binarized - True if the layer is binarized

    """

    # Go through the layers and set the:
    # - padding parameters for input
    # - input shape, which is adjusted to include the padding
    # Append layers we will turn in ELL layers to the relevantLayers list
    relevantLayers = []
    for currentLayer in modelLayers:
        if (isinstance(currentLayer, cntk_py.Function)):

            currentLayer.ell_inputPaddingParameters = get_input_padding_parameters_for_layer(currentLayer)
            currentLayer.ell_binarized = False

            if (currentLayer.op_name == 'Convolution'):
                inputParameter, weightsParameter, binarized = get_convolutional_layer_info(currentLayer)
                currentLayer.ell_inputShape = get_adjusted_shape_for_layer(inputParameter.shape, currentLayer.ell_inputPaddingParameters)                    
                relevantLayers.append(currentLayer)
                currentLayer.ell_binarized = binarized

            elif ((currentLayer.op_name == 'Dense') or
                (currentLayer.op_name == 'linear') or
                (currentLayer.op_name == 'ElementTimes') or
                (currentLayer.op_name == 'MaxPooling') or
                (currentLayer.op_name == 'AveragePooling') or
                (currentLayer.op_name == 'Pooling') or
                (currentLayer.op_name == 'ReLU') or
                (currentLayer.op_name == 'LeakyReLU') or
                (currentLayer.op_name == 'Softmax') or
                (currentLayer.op_name == 'BatchNormalization')
            ):
                if (len(currentLayer.arguments) > 0 and len(currentLayer.arguments[0].shape) > 0):
                    currentLayer.ell_inputShape = get_adjusted_shape_for_layer(
                        currentLayer.arguments[0].shape, currentLayer.ell_inputPaddingParameters)                    
                    relevantLayers.append(currentLayer)
                else:
                    print("\nWill not process", currentLayer.op_name, "with no inputs or input shape - skipping this layer as irrelevant.")                    
            else:
                print("\nWill not process", currentLayer.op_name, "- skipping this layer as irrelevant.")

    # Go through the layers and set the:
    # - padding parameters for output, based on the next layer's input
    # - output shape, which is adjusted to include the padding
    for i in range(len(relevantLayers)):
        currentLayer = relevantLayers[i]

        if (i < (len(relevantLayers) - 1)):
            # Use the next layer's input characteristics to set the output for this layer
            nextLayer = relevantLayers[i + 1]

            currentLayer.ell_outputPaddingParameters = nextLayer.ell_inputPaddingParameters
            currentLayer.ell_outputShape = get_adjusted_shape_for_layer(
                currentLayer.output.shape, currentLayer.ell_outputPaddingParameters)
            currentLayer.ell_outputShapeMinusPadding = get_shape_for_layer(
                currentLayer.output.shape)
        else:
            # This is the last layer, so the output characteristics are known
            currentLayer.ell_outputPaddingParameters = ELL.NoPadding()
            currentLayer.ell_outputShape = get_adjusted_shape_for_layer(
                currentLayer.output.shape, ELL.NoPadding())
            currentLayer.ell_outputShapeMinusPadding = currentLayer.ell_outputShape

        op_name = currentLayer.op_name
        if currentLayer.ell_binarized:
            op_name += "(binarized)"
        print(op_name, ": ", ell_shape_to_string(currentLayer.ell_inputShape), " -> ",
              ell_shape_to_string(currentLayer.ell_outputShape), "| padding ", currentLayer.ell_inputPaddingParameters.paddingSize)
    return relevantLayers


def predictor_from_cntk_model(modelFile):
    """Loads a CNTK model and returns an ELL.NeuralNetworkPredictor"""

    print("Loading...")
    z = load_model(modelFile)
    print("\nFinished loading.")

    print("Pre-processing...")
    modelLayers = get_model_layers(z)

    # Get the relevant CNTK layers that we will convert to ELL
    layersToConvert = get_filtered_layers_list(modelLayers)
    print("\nFinished pre-processing.")

    predictor = None

    try:
        # Create a list of ELL layers from the CNTK layers
        ellLayers = convert_cntk_layers_to_ell_layers(layersToConvert)
        # Create an ELL neural network predictor from the layers
        predictor = ELL.FloatNeuralNetworkPredictor(ellLayers)

    except:
        print("Error occurred attempting to convert cntk layers to ELL layers")
        traceback.print_exc()

    return predictor
