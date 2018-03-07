####################################################################################################
#
# Project:  Embedded Learning Library (ELL)
# File:     cntk_layers.py (importers)
# Authors:  Byron Changuion, Lisa Ong
#
# Requires: Python 3.x, cntk-2.4
#
####################################################################################################

"""Imports CNTK layers to ELL equivalents"""
import logging

from cntk.initializer import glorot_uniform, he_normal
from cntk.layers import Convolution, MaxPooling, AveragePooling, Dropout, BatchNormalization, Dense
import cntk.layers.blocks
from cntk.layers.typing import *
from cntk.ops import *
from cntk import load_model
from cntk.logging.graph import *
from custom_functions import CustomSign, BinaryConvolution

import ell
import lib.cntk_converters as converters
import lib.cntk_utilities as utilities
from custom_functions import BinaryConvolution, CustomSign

_logger = logging.getLogger(__name__)

class BaseLayer:
    """Base class with common layer processing functionality"""

    def __init__(self, layer):
        self.layer = layer
        self.layer.ell_inputPaddingParameters = self.get_input_padding_parameters()
        self.additional_layer_text = None

        if not hasattr(self, 'input_shape'):
            if (len(self.layer.arguments) > 0 and len(self.layer.arguments[0].shape) > 0):
                self.input_shape = self.layer.arguments[0].shape
        # else, assume derived classes have already initialized the input shape

        if hasattr(self, 'input_shape'):
            self.layer.ell_inputShape = utilities.get_adjusted_shape(
                self.input_shape, self.layer.ell_inputPaddingParameters)
        else:
            raise RuntimeError(
                "Could not initialize input_shape")  # coding error

    def __repr__(self):
        """Prints summary info about this layer.
           Derived classes may override this.
        """
        layer_prefix = self.op_name
        if self.additional_layer_text:
            layer_prefix = '{} ({})'.format(layer_prefix, self.additional_layer_text)
        return '{} : {} -> {} | input padding {} output padding {}'.format(layer_prefix, utilities.ell_shape_to_string(self.layer.ell_inputShape),
                         utilities.ell_shape_to_string(self.layer.ell_outputShape),
                         str(self.layer.ell_inputPaddingParameters.paddingSize),
                         str(self.layer.ell_outputPaddingParameters.paddingSize))

    def get_input_padding_parameters(self):
        """Returns the default ell.neural.PaddingParameters for a layer's input.
           Derived classes may override this.
        """

        return ell.neural.PaddingParameters(ell.neural.PaddingScheme.zeros, 0)

    def set_output_characteristics(self, nextLayer):
        """Sets the output characteristics based on the next layer"""

        if nextLayer:
            self.layer.ell_outputPaddingParameters = nextLayer.layer.ell_inputPaddingParameters
            self.layer.ell_outputShape = utilities.get_adjusted_shape(
                self.layer.output.shape, self.layer.ell_outputPaddingParameters)
            self.layer.ell_outputShapeMinusPadding = utilities.get_shape(
                self.layer.output.shape)
        else:
            # last layer
            self.layer.ell_outputPaddingParameters = ell.neural.NoPadding()
            self.layer.ell_outputShape = utilities.get_adjusted_shape(
                self.layer.output.shape, ell.neural.NoPadding())
            self.layer.ell_outputShapeMinusPadding = self.layer.ell_outputShape

    def process(self, ellLayers):
        """Appends the ELL equivalent of the current layer to ellLayers.
           Derived classes must override this.
        """

        raise NotImplementedError(
            "Error: subclasses must override this method")

    def clone_cntk_layer(self, feature):
        """Returns a shallow clone of the CNTK layer for operating on the given feature (input-variable) """
        raise NotImplementedError(
            "Error: subclasses must override this method")

class DenseLayer(BaseLayer):
    """Logic for converting a CNTK Dense layer to ELL"""

    def __init__(self, layer):
        if not layer.is_block:
            raise ValueError("Dense node is not a block node")

        self.op_name = 'Dense'
        super().__init__(layer)
        internalNodes = utilities.get_model_layers(self.layer.block_root)
        self.additional_layer_text = utilities.get_cntk_activation_name(internalNodes)

    def process(self, ellLayers):
        """Appends the ELL equivalent of the current layer to ellLayers."""

        # Note that a single CNTK Dense function block is equivalent to the following 3 ELL layers:
        # - FullyConnectedLayer
        # - BiasLayer
        # - ActivationLayer. This layer is sometimes missing, depending on activation type.
        #
        # Therefore, make sure the output padding characteristics of the last layer reflect the next layer's
        # padding requirements.

        weightsParameter = utilities.find_parameter_by_name(
            self.layer.parameters, 'W', 0)
        biasParameter = utilities.find_parameter_by_name(
            self.layer.parameters, 'b', 1)
        weightsTensor = converters.get_float_tensor_from_cntk_dense_weight_parameter(
            weightsParameter)
        biasVector = converters.get_float_vector_from_cntk_trainable_parameter(
            biasParameter)

        # Create the ell.neural.LayerParameters for the various ELL layers
        firstLayerParameters = ell.neural.LayerParameters(
            self.layer.ell_inputShape, self.layer.ell_inputPaddingParameters, self.layer.ell_outputShapeMinusPadding, ell.neural.NoPadding())
        middleLayerParameters = ell.neural.LayerParameters(self.layer.ell_outputShapeMinusPadding, ell.neural.NoPadding(
        ), self.layer.ell_outputShapeMinusPadding, ell.neural.NoPadding())
        lastLayerParameters = ell.neural.LayerParameters(self.layer.ell_outputShapeMinusPadding, ell.neural.NoPadding(
        ), self.layer.ell_outputShape, self.layer.ell_outputPaddingParameters)

        layerParameters = firstLayerParameters

        internalNodes = utilities.get_model_layers(self.layer.block_root)
        activationType = utilities.get_ell_activation_type(internalNodes)

        # Create the ELL fully connected layer
        ellLayers.append(ell.neural.FloatFullyConnectedLayer(
            layerParameters, weightsTensor))

        # Create the ELL bias layer
        if (utilities.is_softmax_activation(internalNodes) or activationType != None):
            layerParameters = middleLayerParameters
        else:
            layerParameters = lastLayerParameters
        ellLayers.append(ell.neural.FloatBiasLayer(layerParameters, biasVector))

        # Create the ELL activation layer
        if (utilities.is_softmax_activation(internalNodes) or activationType != None):
            layerParameters = lastLayerParameters

            # Special case: if this is softmax activation, create an ELL Softmax layer.
            # Else, insert an ELL ActivationLayer
            if (utilities.is_softmax_activation(internalNodes)):
                ellLayers.append(ell.neural.FloatSoftmaxLayer(layerParameters))
            else:
                if (activationType != None):
                    ellLayers.append(ell.neural.FloatActivationLayer(
                        layerParameters, activationType))

    def clone_cntk_layer(self, feature):
        """Returns a clone of the CNTK layer for per-layer forward prop validation"""
        weightsParameter = utilities.find_parameter_by_name(
            self.layer.parameters, 'W', 0)
        biasParameter = utilities.find_parameter_by_name(
            self.layer.parameters, 'b', 1)

        internalNodes = utilities.get_model_layers(self.layer.block_root)
        activationType = utilities.get_cntk_activation_op(internalNodes)

        includeBias = biasParameter is not None
        layer = Dense(self.layer.shape, activation=activationType, bias=includeBias)(feature)

        layer.parameters[0].value = weightsParameter.value
        if includeBias:
            layer.parameters[1].value = biasParameter.value
        return layer

class BinaryConvolutionLayer(BaseLayer):
    """Logic for converting a CNTK Binary Convolution layer to ELL"""

    def __init__(self, layer):
        if layer.is_block:
            raise ValueError(
                "Error: Binary Convolution layer node is in block node")

        self.op_name = 'BinaryConvolution'

        # Convolution function (ASSUME part of a Binary Convolution layer)
        # - Weights is 4-dimensional (filters, channels, rows, columns)
        # - Input is 3-dimensional (channels, rows, columns)
        # - Bias is a separate layer and not processed by this class
        # - Activation is a separate layer and not processed by this class
        if len(layer.inputs[0].shape) == 3:
            self.input_parameter = layer.inputs[0]
            weights_input = layer.inputs[1]
        else:
            self.input_parameter = layer.inputs[1]
            weights_input = layer.inputs[0]

        self.weights_parameter = utilities.find_parameter_by_name(
            weights_input.owner.parameters, 'filter')
        self.attributes = layer.attributes

        # Determine the binarization method used for weights based on the
        # name attributes of the UserFunctions defined in the custom_functions.py
        # used during training.
        # Until we can find a better heuristic, assume that the custom function names
        # don't change across models.
        function_name = weights_input.owner.name
        if function_name == 'Sign':
            self.convolution_method = ell.neural.BinaryConvolutionMethod.bitwise
            self.weights_scale = ell.neural.BinaryWeightsScale.none
        else:
            raise ValueError(
                "Error: unrecognized binarization function: " + function_name)

        self.input_shape = self.input_parameter.shape

        super().__init__(layer)

    def get_input_padding_parameters(self):
        """Returns the ell.neural.PaddingParameters for a layer's input."""

        paddingScheme = ell.neural.PaddingScheme.zeros
        padding = 0
        receptiveField = self.weights_parameter.shape[2]

        if ('autoPadding' in self.attributes):
            if (self.attributes['autoPadding'][1] == True):
                padding = int((receptiveField - 1) / 2)
            else:
                padding = self.attributes['upperPad'][0]
        else:
            padding = self.attributes['upperPad'][0]

        return ell.neural.PaddingParameters(paddingScheme, padding)

    def process(self, ellLayers):
        """Helper to convert a binary convolutional layer to the ELL equivalent."""

        # A CNTK Binary Convolutional layer is a single function.
        # Bias and Activation are separate layers (processed outside of this class).
        weightsTensor = converters.get_float_tensor_from_cntk_convolutional_weight_parameter(
            self.weights_parameter)

        layerParameters = ell.neural.LayerParameters(
            self.layer.ell_inputShape, self.layer.ell_inputPaddingParameters, self.layer.ell_outputShape,
            self.layer.ell_outputPaddingParameters)

        # Fill in the convolutional parameters
        weightsShape = self.weights_parameter.shape
        receptiveField = weightsShape[2]
        stride = self.attributes['strides'][2]

        convolutionalParameters = ell.neural.BinaryConvolutionalParameters(
            receptiveField, stride, self.convolution_method, self.weights_scale)

        ellLayers.append(ell.neural.FloatBinaryConvolutionalLayer(
            layerParameters, convolutionalParameters, weightsTensor))

    def clone_cntk_layer(self, feature):
        """Returns a clone of the CNTK layer for per-layer forward prop validation"""

        weightsShape = self.weights_parameter.shape  # filters, channels, rows, columns
        pad = self.attributes['autoPadding'][0] or (
            self.attributes['autoPadding'][1] and self.attributes['autoPadding'][2])

        # Bias is a separate layer and not processed by this class
        # Activation is a separate layer and not processed by this class

        x = CustomSign(feature)
        return BinaryConvolution((weightsShape[2], weightsShape[3]), num_filters=weightsShape[0],
                                 channels=weightsShape[1], init=self.weights_parameter.value,
                                 pad=pad, activation=False, bias=False, init_bias=0)(x)

class ConvolutionLayer(BaseLayer):
    """Logic for converting a CNTK Convolution layer to ELL"""

    def __init__(self, layer):
        if not layer.is_block:
            raise ValueError(
                "Error: Convolution layer node is not in block node")

        self.op_name = 'Convolution'
        # initialize weights and input characteristics
        self.input_parameter = layer.arguments[0]
        self.weights_parameter = utilities.find_parameter_by_name(
            layer.parameters, 'W', 0)
        self.bias_parameter = utilities.find_parameter_by_name(
            layer.parameters, 'b', 1)

        # Get the hyper-parameters for the convolution.
        # They are on the convolution node inside this block.
        convolution_nodes = depth_first_search(
            layer.block_root, lambda x: utilities.op_name_equals(x, 'Convolution'))

        self.attributes = convolution_nodes[0].attributes
        self.convolution_method = 0
        self.input_shape = self.input_parameter.shape

        super().__init__(layer)
        nodes = utilities.get_model_layers(layer.block_root)
        if utilities.is_softmax_activation(nodes):
            self.additional_layer_text = 'softmax'
        else:
            activation_type = utilities.get_cntk_activation_name(nodes)
            if activation_type:
                self.additional_layer_text = activation_type

    def get_input_padding_parameters(self):
        """Returns the ell.neural.PaddingParameters for a layer's input."""

        paddingScheme = ell.neural.PaddingScheme.zeros
        padding = 0
        receptiveField = self.weights_parameter.shape[2]

        if ('autoPadding' in self.attributes):
            if (self.attributes['autoPadding'][1] == True):
                padding = int((receptiveField - 1) / 2)
            else:
                padding = self.attributes['upperPad'][0]
        else:
            padding = self.attributes['upperPad'][0]

        return ell.neural.PaddingParameters(paddingScheme, padding)

    def process(self, ellLayers):
        """Helper to convert a convolutional layer to the ELL equivalent."""

        # Note that a single CNTK Convolutional function block is equivalent to the following 3 ELL layers:
        # - ConvolutionalLayer
        # - BiasLayer. This layer is sometimes missing, depending on whether bias is included.
        # - ActivationLayer. This layer is sometimes missing, depending on activation type.
        #
        # Therefore, make sure the output padding characteristics of the last layer reflect the next layer's
        # padding requirements.

        weightsTensor = converters.get_float_tensor_from_cntk_convolutional_weight_parameter(
            self.weights_parameter)

        internalNodes = utilities.get_model_layers(self.layer.block_root)
        activationType = utilities.get_ell_activation_type(internalNodes)
        isSoftmaxActivation = utilities.is_softmax_activation(internalNodes)
        hasActivation = isSoftmaxActivation or activationType != None
        hasBias = self.bias_parameter != None

        # Create the ell.neural.LayerParameters for the various ELL layers
        onlyLayerParameters = ell.neural.LayerParameters(
            self.layer.ell_inputShape, self.layer.ell_inputPaddingParameters, self.layer.ell_outputShape, self.layer.ell_outputPaddingParameters)
        firstLayerParameters = ell.neural.LayerParameters(
            self.layer.ell_inputShape, self.layer.ell_inputPaddingParameters, self.layer.ell_outputShapeMinusPadding, ell.neural.NoPadding())
        middleLayerParameters = ell.neural.LayerParameters(self.layer.ell_outputShapeMinusPadding, ell.neural.NoPadding(
        ), self.layer.ell_outputShapeMinusPadding, ell.neural.NoPadding())
        lastLayerParameters = ell.neural.LayerParameters(self.layer.ell_outputShapeMinusPadding, ell.neural.NoPadding(
        ), self.layer.ell_outputShape, self.layer.ell_outputPaddingParameters)

        # Choose the layer parameters for the convolutional layer. If there is 
        # bias or activation, then the convolution is the first of two or more,
        # otherwise it is the only layer
        if hasActivation or hasBias:
            layerParameters = firstLayerParameters
        else:
            layerParameters = onlyLayerParameters

        # Fill in the convolutional parameters
        weightsShape = self.weights_parameter.shape
        receptiveField = weightsShape[2]
        stride = self.attributes['strides'][2]

        filterBatchSize = layerParameters.outputShape.channels

        convolutionalParameters = ell.neural.ConvolutionalParameters(
            receptiveField, stride, self.convolution_method, filterBatchSize)

        # Create the ELL convolutional layer
        ellLayers.append(ell.neural.FloatConvolutionalLayer(
            layerParameters, convolutionalParameters, weightsTensor))

        # Create the ELL bias layer
        if hasBias:
            if hasActivation:
                layerParameters = middleLayerParameters
            else:
                layerParameters = lastLayerParameters
            biasVector = converters.get_float_vector_from_cntk_trainable_parameter(
                self.bias_parameter)
            ellLayers.append(ell.neural.FloatBiasLayer(layerParameters, biasVector))

        # Create the ELL activation layer
        if hasActivation:
            layerParameters = lastLayerParameters

            # Special case: if this is softmax activation, create an ELL Softmax layer.
            # Else, insert an ELL ActivationLayer
            if (isSoftmaxActivation):
                ellLayers.append(ell.neural.FloatSoftmaxLayer(layerParameters))
            else:
                ellLayers.append(ell.neural.FloatActivationLayer(
                    layerParameters, activationType))

    def clone_cntk_layer(self, feature):
        """Returns a clone of the CNTK layer for per-layer forward prop validation"""

        nodes = utilities.get_model_layers(self.layer.block_root)
        activation = utilities.get_cntk_activation_op(nodes)

        weightsShape = self.weights_parameter.shape
        pad = self.attributes['autoPadding'][0] or (
            self.attributes['autoPadding'][1] and self.attributes['autoPadding'][2])
        bias = (self.bias_parameter is not None)

        layer = Convolution((weightsShape[2], weightsShape[3]), weightsShape[0],
                            pad=pad, activation=activation, bias=bias)(feature)

        layer.parameters[0].value = self.weights_parameter.value
        if bias:
            layer.parameters[1].value = self.bias_parameter.value
        return layer


class LinearLayer(BaseLayer):
    """Logic for converting a CNTK Linear layer to ELL"""

    def __init__(self, layer):
        self.op_name = 'Linear'
        super().__init__(layer)

    def process(self, ellLayers):
        """Appends the ELL representation of the current layer to ellLayers."""

        # Note that a single CNTK Linear function block is equivalent to the following 3 ELL layers:
        # - FullyConnectedLayer
        # - BiasLayer
        # - ActivationLayer. This layer is sometimes missing, depending on activation type.
        #
        # Therefore, make sure the output padding characteristics of the last layer reflect the next layer's
        # padding requirements.

        weightsParameter = utilities.find_parameter_by_name(
            self.layer.parameters, 'W', 0)
        biasParameter = utilities.find_parameter_by_name(
            self.layer.parameters, 'b', 1)
        weightsTensor = converters.get_float_tensor_from_cntk_dense_weight_parameter(
            weightsParameter)
        biasVector = converters.get_float_vector_from_cntk_trainable_parameter(
            biasParameter)

        # Create the ell.neural.LayerParameters for the various ELL layers
        firstLayerParameters = ell.neural.LayerParameters(
            self.layer.ell_inputShape, self.layer.ell_inputPaddingParameters, self.layer.ell_outputShapeMinusPadding, ell.neural.NoPadding())
        middleLayerParameters = ell.neural.LayerParameters(self.layer.ell_outputShapeMinusPadding, ell.neural.NoPadding(
        ), self.layer.ell_outputShapeMinusPadding, ell.neural.NoPadding())
        lastLayerParameters = ell.neural.LayerParameters(self.layer.ell_outputShapeMinusPadding, ell.neural.NoPadding(
        ), self.layer.ell_outputShape, self.layer.ell_outputPaddingParameters)

        layerParameters = firstLayerParameters

        internalNodes = utilities.get_model_layers(self.layer.block_root)
        activationType = utilities.get_ell_activation_type(internalNodes)

        # Create the ELL fully connected layer
        ellLayers.append(ell.neural.FloatFullyConnectedLayer(
            layerParameters, weightsTensor))

        # Create the ELL bias layer
        isSoftmaxActivation = utilities.is_softmax_activation(internalNodes)
        hasActivation = isSoftmaxActivation or activationType != None
        if (hasActivation):
            layerParameters = middleLayerParameters
        else:
            layerParameters = lastLayerParameters
        ellLayers.append(ell.neural.FloatBiasLayer(layerParameters, biasVector))

        # Create the ELL activation layer
        if (hasActivation):
            layerParameters = lastLayerParameters

            # Special case: if this is softmax activation, create an ELL Softmax layer.
            # Else, insert an ELL ActivationLayer
            if (isSoftmaxActivation):
                ellLayers.append(ell.neural.FloatSoftmaxLayer(layerParameters))
            else:
                ellLayers.append(ell.neural.FloatActivationLayer(
                    layerParameters, activationType))

    def clone_cntk_layer(self, feature):
        """Returns a clone of the CNTK layer for per-layer forward prop validation"""
        raise NotImplementedError("Error: not yet implemented")

class ElementTimesLayer(BaseLayer):
    """Logic for converting a CNTK ElementTimes layer to ELL"""

    def __init__(self, layer):
        if (len(layer.parameters) != 1 and len(layer.constants) != 1):
            raise ValueError(
                "Skipping ElementTimes layer due to dimensions of Constants and Parameters")

        self.op_name = 'ElementTimes'
        if (len(layer.constants) > 0):
            self.scale = layer.constants[0]
        elif (len(layer.parameters) > 0):
            self.scale = layer.parameters[0]

        super().__init__(layer)

    def process(self, ellLayers):
        """Appends the ELL representation of the current layer to ellLayers."""

        # Create the ell.neural.LayerParameters for the ELL layer
        layerParameters = ell.neural.LayerParameters(
            self.layer.ell_inputShape, self.layer.ell_inputPaddingParameters, self.layer.ell_outputShape, self.layer.ell_outputPaddingParameters)

        # Create ELL scaling layer
        if (self.scale.value.size == 1):
            scalesVector = converters.get_float_vector_from_constant(
                self.scale.value, layerParameters.outputShape.channels)
        else:
            scalesVector = converters.get_float_vector_from_cntk_array(
                self.scale.value)

        ellLayers.append(ell.neural.FloatScalingLayer(
            layerParameters, scalesVector))

    def clone_cntk_layer(self, feature):
        """Returns a clone of the CNTK layer for per-layer forward prop validation"""

        x = reshape(feature, (self.layer.ell_outputShape.channels,))
        return element_times(x, self.scale)

class BasePoolingLayer(BaseLayer):
    """Common logic for converting a Pooling layer to ELL"""

    def __init__(self, layer):
        if layer.is_block:
            self.attributes = layer.block_root.attributes
        else:
            self.attributes = layer.attributes
        super().__init__(layer)

    def get_input_padding_parameters(self):
        """Returns the ell.neural.PaddingParameters for a layer's input."""

        padding = 0
        if ('autoPadding' in self.attributes):
            if (self.attributes['autoPadding'][0] == True):
                padding = int((self.attributes['poolingWindowShape'][0] - 1) / 2)
            else:
                padding = self.attributes['upperPad'][0]
        else:
            padding = self.attributes['upperPad'][0]

        return ell.neural.PaddingParameters(self.padding_scheme, padding)

    def get_cntk_parameters(self):
        pad = False
        if ('autoPadding' in self.attributes and True in self.attributes['autoPadding']):
            pad = True
        poolingSize = self.attributes['poolingWindowShape']
        filterShape = (poolingSize[0], poolingSize[1])
        stride = self.attributes['strides'][0]
        return pad, filterShape, stride

    def process(self, ellLayers):
        """Appends the ELL representation of the current layer to ellLayers."""

        # Create the ell.neural.LayerParameters for the ELL layer
        layerParameters = ell.neural.LayerParameters(
            self.layer.ell_inputShape, self.layer.ell_inputPaddingParameters, self.layer.ell_outputShape, self.layer.ell_outputPaddingParameters)

        # Fill in the pooling parameters
        poolingSize = self.attributes['poolingWindowShape'][0]
        stride = self.attributes['strides'][0]

        poolingParameters = ell.neural.PoolingParameters(poolingSize, stride)

        # Create the ELL pooling layer
        ellLayers.append(ell.neural.FloatPoolingLayer(
            layerParameters, poolingParameters, self.pooling_type))

    def clone_cntk_layer(self, feature):
        """Returns a clone of the CNTK layer for per-layer forward prop validation"""

        raise NotImplementedError(
            "Error: subclasses must override this method")

class MaxPoolingLayer(BasePoolingLayer):
    """Logic for converting a CNTK MaxPooling layer to ELL"""

    def __init__(self, layer):
        self.op_name = 'MaxPooling'
        self.padding_scheme = ell.neural.PaddingScheme.min
        self.pooling_type = ell.neural.PoolingType.max

        super().__init__(layer)

    def clone_cntk_layer(self, feature):
        """Returns a clone of the CNTK layer for per-layer forward prop validation"""

        pad, filterShape, stride = self.get_cntk_parameters()
        return MaxPooling(filterShape, strides=(stride, stride), pad=pad)(feature)

class AveragePoolingLayer(BasePoolingLayer):
    """Logic for converting a CNTK AveragePooling layer to ELL"""

    def __init__(self, layer):
        self.op_name = 'AveragePooling'
        self.padding_scheme = ell.neural.PaddingScheme.zeros
        self.pooling_type = ell.neural.PoolingType.mean

        super().__init__(layer)

    def clone_cntk_layer(self, feature):
        """Returns a clone of the CNTK layer for per-layer forward prop"""

        pad, filterShape, stride = self.get_cntk_parameters()
        return AveragePooling(filterShape, strides=(stride, stride), pad=pad)(feature)

class PoolingLayer(BaseLayer):
    """Logic for converting a CNTK Pooling layer to ELL"""

    def __init__(self, layer):
        self.op_name = 'Pooling'
        super().__init__(layer)

        if (layer.attributes['poolingType'] == PoolingType_Max):
            self.actual_layer = MaxPoolingLayer(layer)
        else:
            self.actual_layer = AveragePoolingLayer(layer)

    def __repr__(self):
        return self.actual_layer.__repr__()

    def process(self, ellLayers):
        """Appends the ELL representation of the current layer to ellLayers."""
        self.actual_layer.process(ellLayers)

    def clone_cntk_layer(self, feature):
        """Returns a clone of the CNTK layer for per-layer forward prop"""

        return self.actual_layer.clone_cntk_layer(feature)

class ActivationLayer(BaseLayer):
    """Logic for converting a CNTK Activation layer to ELL"""

    def __init__(self, layer):
        if not layer.is_block:
            raise ValueError("Activation node is not a block node")
        self.op_name = 'Activation'
        super().__init__(layer)

        internal_nodes = utilities.get_model_layers(self.layer.block_root)
        self.activation_type = utilities.get_ell_activation_type(internal_nodes)
        self.additional_layer_text = utilities.get_cntk_activation_name(internal_nodes)

    def process(self, ellLayers):
        """Appends the ELL representation of the current layer to ellLayers."""

        # Create the ell.neural.LayerParameters for the ELL layer
        layerParameters = ell.neural.LayerParameters(
            self.layer.ell_inputShape, self.layer.ell_inputPaddingParameters, self.layer.ell_outputShape, self.layer.ell_outputPaddingParameters)

        # Create the ELL activation layer
        ellLayers.append(ell.neural.FloatActivationLayer(
            layerParameters, self.activation_type))

    def clone_cntk_layer(self, feature):
        """Returns a clone of the CNTK layer for per-layer forward prop validation"""
        if self.activation_type == ell.neural.ActivationType.sigmoid:
            return sigmoid(feature)
        elif self.activation_type == ell.neural.ActivationType.leaky:
            return leaky_relu(feature)
        else:
            return relu(feature)

class ReLULayer(BaseLayer):
    """Logic for converting a CNTK ReLU layer to ELL"""

    def __init__(self, layer):
        self.op_name = 'ReLU'
        super().__init__(layer)

    def process(self, ellLayers):
        """Appends the ELL representation of the current layer to ellLayers."""

        # Create the ell.neural.LayerParameters for the ELL layer
        layerParameters = ell.neural.LayerParameters(
            self.layer.ell_inputShape, self.layer.ell_inputPaddingParameters, self.layer.ell_outputShape, self.layer.ell_outputPaddingParameters)

        # Create the ELL activation layer
        ellLayers.append(ell.neural.FloatActivationLayer(
            layerParameters, ell.neural.ActivationType.relu))

    def clone_cntk_layer(self, feature):
        """Returns a clone of the CNTK layer for per-layer forward prop validation"""

        return relu(feature)

class LeakyReLULayer(BaseLayer):
    """Logic for converting a CNTK LeakyReLU layer to ELL"""

    def __init__(self, layer):
        self.op_name = 'LeakyReLU'
        super().__init__(layer)

    def process(self, ellLayers):
        """Appends the ELL representation of the current layer to ellLayers."""

        # Create the ell.neural.LayerParameters for the ELL layer
        layerParameters = ell.neural.LayerParameters(
            self.layer.ell_inputShape, self.layer.ell_inputPaddingParameters, self.layer.ell_outputShape, self.layer.ell_outputPaddingParameters)

        # Create the ELL activation layer
        ellLayers.append(ell.neural.FloatActivationLayer(
            layerParameters, ell.neural.ActivationType.leaky))

    def clone_cntk_layer(self, feature):
        """Returns a clone of the CNTK layer for per-layer forward prop validation"""

        return leaky_relu(feature)

class PReLULayer(BaseLayer):
    """Logic for converting a CNTK PReLU layer to ELL"""

    def __init__(self, layer):
        self.op_name = 'PReLU'
        super().__init__(layer)
        self.prelu_parameter = utilities.find_parameter_by_name(
            self.layer.parameters, 'prelu', 0)

    def process(self, ellLayers):
        """Appends the ELL representation of the current layer to ellLayers."""

        preluTensor = converters.get_float_tensor_from_cntk_convolutional_weight_parameter(
            self.prelu_parameter)

        # Create the ell.neural.LayerParameters for the ELL layer
        layerParameters = ell.neural.LayerParameters(
            self.layer.ell_inputShape, self.layer.ell_inputPaddingParameters, self.layer.ell_outputShape, self.layer.ell_outputPaddingParameters)

        # Create the ELL PReLU activation layer
        ellLayers.append(ell.neural.FloatPReLUActivationLayer(
            layerParameters, preluTensor))

    def clone_cntk_layer(self, feature):
        """Returns a clone of the CNTK layer for per-layer forward prop validation"""

        return param_relu(self.prelu_parameter, feature)


class SoftmaxLayer(BaseLayer):
    """Logic for converting a CNTK Softmax layer to ELL"""

    def __init__(self, layer):
        self.op_name = 'Softmax'
        super().__init__(layer)

    def process(self, ellLayers):
        """Appends the ELL representation of the current layer to ellLayers."""

        if (self.layer.op_name == 'CrossEntropyWithSoftmax'):
            # ugly hack for CrossEntropyWithSoftmax
            # CrossEntropyWithSoftmax outputs to a Tensor[1], but we just need Softmax
            layerParameters = ell.neural.LayerParameters(
                self.layer.ell_inputShape, self.layer.ell_inputPaddingParameters, self.layer.ell_inputShape, self.layer.ell_inputPaddingParameters)
        else:
            layerParameters = ell.neural.LayerParameters(
                self.layer.ell_inputShape, self.layer.ell_inputPaddingParameters, self.layer.ell_outputShape, self.layer.ell_outputPaddingParameters)

        # Create the ELL softmax layer
        ellLayers.append(ell.neural.FloatSoftmaxLayer(layerParameters))

    def clone_cntk_layer(self, feature):
        """Returns a clone of the CNTK layer for per-layer forward prop validation"""

        return softmax(feature)

class BatchNormalizationLayer(BaseLayer):
    """Logic for converting a CNTK BatchNormalization layer to ELL"""

    def __init__(self, layer):
        self.op_name = 'BatchNormalization'

        self.scale = utilities.find_parameter_by_name(
            layer.parameters, 'scale', 0)
        self.bias = utilities.find_parameter_by_name(
            layer.parameters, 'bias', 1)
        self.mean = utilities.find_parameter_by_name(
            layer.constants, 'aggregate_mean', 0)
        self.variance = utilities.find_parameter_by_name(
            layer.constants, 'aggregate_variance', 1)

        # The default CNTK epsilon
        self.epsilon = 1e-5

        super().__init__(layer)

    def process(self, ellLayers):
        """Appends the ELL representation of the current layer to ellLayers."""

        # Note that a single CNTK Batch Normalization layer is equivalent to the following 3 ELL layers:
        # - BatchNormalizationLayer
        # - ScalingLayer
        # - BiasLayer
        #
        # Therefore, make sure the output padding characteristics of the last layer reflect the next layer's
        # padding requirements.

        scaleVector = converters.get_float_vector_from_cntk_trainable_parameter(
            self.scale)
        biasVector = converters.get_float_vector_from_cntk_trainable_parameter(
            self.bias)
        meanVector = converters.get_float_vector_from_cntk_trainable_parameter(
            self.mean)
        varianceVector = converters.get_float_vector_from_cntk_trainable_parameter(
            self.variance)

        # Create the ell.neural.LayerParameters for the various ELL layers
        firstLayerParameters = ell.neural.LayerParameters(
            self.layer.ell_inputShape, self.layer.ell_inputPaddingParameters, self.layer.ell_outputShapeMinusPadding, ell.neural.NoPadding())
        middleLayerParameters = ell.neural.LayerParameters(self.layer.ell_outputShapeMinusPadding, ell.neural.NoPadding(
        ), self.layer.ell_outputShapeMinusPadding, ell.neural.NoPadding())
        lastLayerParameters = ell.neural.LayerParameters(self.layer.ell_outputShapeMinusPadding, ell.neural.NoPadding(
        ), self.layer.ell_outputShape, self.layer.ell_outputPaddingParameters)

        # Create the layers
        ellLayers.append(ell.neural.FloatBatchNormalizationLayer(
            firstLayerParameters, meanVector, varianceVector, self.epsilon, ell.neural.EpsilonSummand.variance))
        ellLayers.append(ell.neural.FloatScalingLayer(
            middleLayerParameters, scaleVector))
        ellLayers.append(ell.neural.FloatBiasLayer(lastLayerParameters, biasVector))

    def clone_cntk_layer(self, feature):
        """Returns a clone of the CNTK layer for per-layer forward prop validation"""
        scale = parameter(shape=self.scale.shape, init=self.scale.value, name='scale')
        bias = parameter(shape=self.scale.shape, init=self.bias.value, name='bias')
        run_mean = constant(shape=self.scale.shape, value=self.mean.value, name='aggregate_mean')
        run_variance = constant(shape=self.scale.shape, value=self.variance.value, name='aggregate_variance')
        run_count = constant(0, shape=(), name='aggregate_count')
        return batch_normalization(feature, scale, bias, run_mean, run_variance, running_count=run_count, spatial=True)

class BiasLayer(BaseLayer):
    """Logic for converting a CNTK Plus layer to ELL"""

    def __init__(self, layer):
        if (len(layer.parameters) != 1):
            raise ValueError(
                "Only processing Plus functions that act as bias layers")

        self.op_name = 'Plus'
        super().__init__(layer)

    def process(self, ellLayers):
        """Appends the ELL representation of the current layer to ellLayers."""

        biasVector = converters.get_float_vector_from_cntk_trainable_parameter(
            self.layer.parameters[0])

        # Create the ell.neural.LayerParameters for the ELL layer
        layerParameters = ell.neural.LayerParameters(
            self.layer.ell_inputShape, self.layer.ell_inputPaddingParameters, self.layer.ell_outputShape, self.layer.ell_outputPaddingParameters)

        # Create the ELL bias layer
        ellLayers.append(ell.neural.FloatBiasLayer(layerParameters, biasVector))

    def clone_cntk_layer(self, feature):
        """Returns a clone of the CNTK layer for per-layer forward prop validation"""
        return plus(self.layer.parameters[0], feature)


class NegativeBiasLayer(BaseLayer):
    """Logic for converting a CNTK Minus layer to ELL"""

    def __init__(self, layer):
        if (len(layer.constants) != 1 and layer.constants[0].value.size != 1):
            raise ValueError(
                "Skipping Minus function due to dimensions of Constant")

        # TODO: This logic is very fragile, we may want to have a model
        # schema for labeling inputs, nodes, and outputs
        if (layer.output.name != 'mean_removed_input'):
            raise ValueError(
                "Only processing Minus functions that remove input mean")

        self.op_name = 'Minus'
        super().__init__(layer)

    def process(self, ellLayers):
        """Appends the ELL representation of the current layer to ellLayers."""

        # Create the ell.neural.LayerParameters for the ELL layer
        layerParameters = ell.neural.LayerParameters(
            self.layer.ell_inputShape, self.layer.ell_inputPaddingParameters, self.layer.ell_outputShape, self.layer.ell_outputPaddingParameters)

        bias = -1.0 * self.layer.constants[0].value
        if len(bias.shape) == 0:
            biasVector = converters.get_float_vector_from_constant(bias, layerParameters.outputShape.channels)
        else:
            biasVector = converters.get_float_vector_from_cntk_array(bias)

        # Create the ELL bias layer
        ellLayers.append(ell.neural.FloatBiasLayer(layerParameters, biasVector))

    def clone_cntk_layer(self, feature):
        """Returns a clone of the CNTK layer for per-layer forward prop validation"""
        return minus(feature, constant(self.layer.constants[0].value), name=self.layer.output.name)

class LayerFactory():
    @staticmethod
    def get_layer_object(cntkLayer):
        try:
            if (cntkLayer.op_name == 'Activation'):
                return ActivationLayer(cntkLayer)
            elif (cntkLayer.op_name == 'AveragePooling'):
                return AveragePoolingLayer(cntkLayer)
            elif (cntkLayer.op_name == 'BatchNormalization'):
                return BatchNormalizationLayer(cntkLayer)
            elif (cntkLayer.op_name == 'Convolution'):
                if (cntkLayer.is_block):
                    return ConvolutionLayer(cntkLayer)
                else:
                    return BinaryConvolutionLayer(cntkLayer)
            elif (cntkLayer.op_name == 'Dense'):
                return DenseLayer(cntkLayer)
            elif (cntkLayer.op_name == 'ElementTimes'):
                return ElementTimesLayer(cntkLayer)
            elif (cntkLayer.op_name == 'LeakyReLU'):
                return LeakyReLULayer(cntkLayer)
            elif (cntkLayer.op_name == 'linear'): # Note: this op_name is lowercase
                return LinearLayer(cntkLayer)
            elif (cntkLayer.op_name == 'MaxPooling'):
                return MaxPoolingLayer(cntkLayer)
            elif (cntkLayer.op_name == 'Minus'):
                return NegativeBiasLayer(cntkLayer)
            elif (cntkLayer.op_name == 'Plus'):
                return BiasLayer(cntkLayer)
            elif (cntkLayer.op_name == 'Pooling'):
                return PoolingLayer(cntkLayer)
            elif (cntkLayer.op_name == 'PReLU'):
                return PReLULayer(cntkLayer)
            elif (cntkLayer.op_name == 'ReLU'):
                return ReLULayer(cntkLayer)
            elif (cntkLayer.op_name == 'Softmax'):
                return SoftmaxLayer(cntkLayer)
            else:
                _logger.warning("Will not process " + cntkLayer.op_name +
                      "- skipping this layer as irrelevant.")
        except (ValueError, AttributeError) as e:
            # raised if a layer contains invalid characteristics
            _logger.info("\nWill not process", cntkLayer.op_name, "-", str(e))

        return None

    @staticmethod
    def has_inputs(cntkLayer):
        return ((len(cntkLayer.arguments) > 0 and len(cntkLayer.arguments[0].shape) > 0) or
                # special case for Binary Convolution
                (cntkLayer.op_name == 'Convolution' and len(cntkLayer.inputs) > 0 and 
                len(cntkLayer.inputs[0].shape) > 0))


def get_filtered_layers_list(modelLayers, maxLayerCount=None):
    """Returns a relevant list of CNTK layers and layer objects
    """

    # Go through the layers and append layer objects to the relevantLayers list
    relevantLayers = []
    lastSoftmaxLayer = None
    for currentLayer in modelLayers:
        if (isinstance(currentLayer, cntk_py.Function)):
            if (LayerFactory.has_inputs(currentLayer)):
                layerObject = LayerFactory.get_layer_object(currentLayer)
                if (layerObject is not None):
                    relevantLayers.append(layerObject)
                elif currentLayer.op_name == 'CrossEntropyWithSoftmax':
                    # ugly hack for CrossEntropyWithSoftmax
                    # CrossEntropyWithSoftmax pops up in the beginning of the layers list
                    # because the input is connected to it (it's used for evaluating training)
                    lastSoftmaxLayer = SoftmaxLayer(currentLayer)
            else:
                _logger.warning("Will not process " + currentLayer.op_name + 
                                " - empty input shape.")

    if (lastSoftmaxLayer is not None):
        # Retroactively insert a softmax layer
        relevantLayers.append(lastSoftmaxLayer)

    if (maxLayerCount is not None):
        maxLayerCount = min(maxLayerCount, len(relevantLayers))
        relevantLayers = relevantLayers[0:maxLayerCount]

    # Go through the layers and set the output characteristics:
    # - padding parameters for output, based on the next layer's input
    # - output shape, which is adjusted to include the padding
    currentLayer = None
    for i in range(len(relevantLayers)):
        currentLayer = relevantLayers[i]
        if (i < (len(relevantLayers) - 1)):
            # Use the next layer's input characteristics to set the output for this layer
            nextLayer = relevantLayers[i + 1]
            currentLayer.set_output_characteristics(nextLayer)
        else:
            # This is the last layer, so the output characteristics are known
            currentLayer.set_output_characteristics(None)
        _logger.info(currentLayer)

    return relevantLayers


def convert_cntk_layers_to_ell_layers(layersToConvert):
    """Walks a list of CNTK layers and returns a list of ELL Layer objects that is used to construct a Neural Network Predictor"""

    ellLayers = []
    for layerObject in layersToConvert:
        layerObject.process(ellLayers)

    return ellLayers
