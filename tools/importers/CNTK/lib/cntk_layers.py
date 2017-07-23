####################################################################################################
#
# Project:  Embedded Learning Library (ELL)
# File:     layers.py (importers)
# Authors:  Byron Changuion, Lisa Ong
#
# Requires: Python 3.x, cntk-2.0-cp35
#
####################################################################################################

"""Imports CNTK layers to ELL equivalents"""

import ELL
from cntk.initializer import glorot_uniform, he_normal
from cntk.layers import Convolution, MaxPooling, AveragePooling, Dropout, BatchNormalization, Dense
import cntk.layers.blocks
from cntk.layers.typing import *
from cntk.ops import *
from cntk import load_model
from cntk.logging.graph import *

import lib.cntk_converters as converters

def op_name_equals(node, name):
    result = False
    if hasattr(node, 'op_name'):
        result = (node.op_name == name)

    return result

def find_parameter_by_name(parameters, name, index=0):
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

def find_node_by_op_name(parameters, name):
    for p in parameters:
        if (p.op_name == name):
            return p
    return None

def get_activation_type(nodes):
    """Returns an ELL.ActivationType from the list of nodes"""
    if (find_node_by_op_name(nodes, 'ReLU') != None):
        return ELL.ActivationType.relu
    elif (find_node_by_op_name(nodes, 'Sigmoid') != None):
        return ELL.ActivationType.sigmoid
    elif (find_node_by_op_name(nodes, 'LeakyReLU') != None):
        return ELL.ActivationType.leaky

    return None

def ell_activation_type_to_string(type):
    """Returns the string representation of an ELL.ActivationType"""
    if (type == ELL.ActivationType.relu):
        return 'ReLU'
    elif (type == ELL.ActivationType.sigmoid):
        return 'Sigmoid'
    elif (type == ELL.ActivationType.leaky):
        return 'LeakyReLU'

    return ""

def ell_shape_to_string(shape):
    """Returns the string representation of an ELL.LayerShape"""
    return (str(shape.rows) + "x" + str(shape.columns) + "x" + str(shape.channels))

def get_model_layers(root):
    """Returns a list of the high-level layers (i.e. function blocks) that make up the CNTK model """
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
        if (not isinstance(node, Variable)) and (not node.uid in visited):
            layers.append(node)
            visited.add(node.uid)

    # CNTK layers are in opposite order to what ELL wants, so reverse the list
    layers.reverse()

    return layers


class BaseLayer:
    """Base class with common layer processing functionality"""

    def __init__(self, layer):
        self.layer = layer
        self.layer.ell_inputPaddingParameters = self.get_input_padding_parameters()
        self.ell_binarized = False

        if not hasattr(self, 'input_shape'):
            if (len(self.layer.arguments) > 0 and len(self.layer.arguments[0].shape) > 0):
                self.input_shape = self.layer.arguments[0].shape
        # else, assume derived classes have already initialized the input shape

        if hasattr(self, 'input_shape'):
            self.layer.ell_inputShape = BaseLayer.get_adjusted_shape(
                self.input_shape, self.layer.ell_inputPaddingParameters)
        else:
            raise RuntimeError("Could not initialize input_shape") # coding error

    @staticmethod
    def get_shape(inputShape):
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

    @staticmethod
    def get_adjusted_shape(inputShape, paddingParameters):
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
            raise NotImplementedError(
                "Unsupported input shape length: " + str(len(inputShape)))

        return ELL.LayerShape(rows, columns, channels)

    def get_input_padding_parameters(self):
        """Returns the default ELL.PaddingParameters for a layer's input.
           Derived classes may override this.
        """

        return ELL.PaddingParameters(ELL.PaddingScheme.zeros, 0)

    def set_output_characteristics(self, nextLayer):
        """Sets the output characteristics based on the next layer"""

        if (nextLayer is not None):            
            self.layer.ell_outputPaddingParameters = nextLayer.layer.ell_inputPaddingParameters
            self.layer.ell_outputShape = BaseLayer.get_adjusted_shape(
                self.layer.output.shape, self.layer.ell_outputPaddingParameters)
            self.layer.ell_outputShapeMinusPadding = BaseLayer.get_shape(
                self.layer.output.shape)
        else:
            # last layer
            self.layer.ell_outputPaddingParameters = ELL.NoPadding()
            self.layer.ell_outputShape = BaseLayer.get_adjusted_shape(
                self.layer.output.shape, ELL.NoPadding())
            self.layer.ell_outputShapeMinusPadding = self.layer.ell_outputShape

    def process(self, ellLayers):
        """Appends the ELL equivalent of the current layer to ellLayers.
           Derived classes must override this.
        """

        raise NotImplementedError(
            "Error: subclasses must override this method")

    def print_summary(self):
        """Prints summary info about this layer.
           Derived classes may override this.
        """
        label = self.op_name
        if self.ell_binarized:
            label += "(binarized)"

        print(label, ": ", ell_shape_to_string(self.layer.ell_inputShape), " -> ",
            ell_shape_to_string(self.layer.ell_outputShape),
            "| input padding", self.layer.ell_inputPaddingParameters.paddingSize,
            " output padding", self.layer.ell_outputPaddingParameters.paddingSize)


class DenseLayer(BaseLayer):
    """Logic for converting a CNTK Dense layer to ELL"""

    def __init__(self, layer):
        self.op_name = 'Dense'
        super().__init__(layer)

    def process(self, ellLayers):
        """Appends the ELL equivalent of the current layer to ellLayers."""

        if not self.layer.is_block:
            print("Error: Dense node is not a block node")
            return

        # Note that a single CNTK Dense function block is equivalent to the following 3 ELL layers:
        # - FullyConnectedLayer
        # - BiasLayer
        # - ActivationLayer. This layer is sometimes missing, depending on activation type.
        #
        # Therefore, make sure the output padding characteristics of the last layer reflect the next layer's
        # padding requirements.

        weightsParameter = find_parameter_by_name(self.layer.parameters, 'W', 0)
        biasParameter = find_parameter_by_name(self.layer.parameters, 'b', 1)
        weightsTensor = converters.get_float_tensor_from_cntk_dense_weight_parameter(
            weightsParameter)
        biasVector = converters.get_float_vector_from_cntk_trainable_parameter(
            biasParameter)

        # Create the ELL.LayerParameters for the various ELL layers
        firstLayerParameters = ELL.LayerParameters(
            self.layer.ell_inputShape, self.layer.ell_inputPaddingParameters, self.layer.ell_outputShapeMinusPadding, ELL.NoPadding())
        middleLayerParameters = ELL.LayerParameters(self.layer.ell_outputShapeMinusPadding, ELL.NoPadding(
        ), self.layer.ell_outputShapeMinusPadding, ELL.NoPadding())
        lastLayerParameters = ELL.LayerParameters(self.layer.ell_outputShapeMinusPadding, ELL.NoPadding(
        ), self.layer.ell_outputShape, self.layer.ell_outputPaddingParameters)

        layerParameters = firstLayerParameters

        internalNodes = get_model_layers(self.layer.block_root)
        activationType = get_activation_type(internalNodes)

        # Create the ELL fully connected layer
        ellLayers.append(ELL.FloatFullyConnectedLayer(
            layerParameters, weightsTensor))

        # Create the ELL bias layer
        if (SoftmaxLayer.is_softmax_activation(internalNodes) or activationType != None):
            layerParameters = middleLayerParameters
        else:
            layerParameters = lastLayerParameters
        ellLayers.append(ELL.FloatBiasLayer(layerParameters, biasVector))

        # Create the ELL activation layer
        if (SoftmaxLayer.is_softmax_activation(internalNodes) or activationType != None):
            layerParameters = lastLayerParameters

            # Special case: if this is softmax activation, create an ELL Softmax layer.
            # Else, insert an ELL ActivationLayer
            if(SoftmaxLayer.is_softmax_activation(internalNodes)):
                ellLayers.append(ELL.FloatSoftmaxLayer(layerParameters))
            else:
                if (activationType != None):
                    ellLayers.append(ELL.FloatActivationLayer(
                        layerParameters, activationType))


class ConvolutionLayer(BaseLayer):
    """Logic for converting a CNTK Convolution layer to ELL"""

    def __init__(self, layer):
        self.op_name = 'Convolution'

        # initialize weights and input characteristics
        if layer.is_block:
            self.input_parameter = layer.arguments[0]
            self.weights_parameter = find_parameter_by_name(layer.parameters, 'W', 0)
            self.bias_parameter = find_parameter_by_name(layer.parameters, 'b', 1)

            # Get the hyper-parameters for the convolution.
            # They are on the convolution node inside this block.
            convolution_nodes = depth_first_search(
                layer.block_root, lambda x: op_name_equals(x, 'Convolution'))

            self.attributes = convolution_nodes[0].attributes
            self.convolution_method = 0
        else:
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

            self.weights_parameter = find_parameter_by_name(weights_input.owner.parameters, 'filter')
            self.attributes = layer.attributes

            # Determine the binarization method used for weights based on the
            # name attributes of the UserFunctions defined in the custom_functions.py
            # used during training.
            # Until we can find a better heuristic, assume that the custom function names
            # don't change across models.
            function_name = weights_input.owner.name
            if function_name == 'Sign':
                # TODO: bitwise not yet functional for padded inputs
                self.convolution_method = ELL.BinaryConvolutionMethod.gemm
                self.weights_scale = ELL.BinaryWeightsScale.none
            else:
                raise ValueError("Error: unrecognized binarization function: " + function_name)

        self.input_shape = self.input_parameter.shape

        super().__init__(layer)

        self.ell_binarized = not layer.is_block

    def get_input_padding_parameters(self):
        """Returns the ELL.PaddingParameters for a layer's input."""

        paddingScheme = ELL.PaddingScheme.zeros
        padding = 0
        receptiveField = self.weights_parameter.shape[2]

        if ('autoPadding' in self.attributes):
            if (self.attributes['autoPadding'][1] == True):
                padding = int((receptiveField - 1) / 2)
            else:
                padding = self.attributes['upperPad'][0]
        else:
            padding = self.attributes['upperPad'][0]

        return ELL.PaddingParameters(paddingScheme, padding)

    def convert_real_valued_to_ell(self, ellLayers):
        """Helper to convert a convolutional layer to the ELL equivalent."""
        if not self.layer.is_block:
            print("Error: Convolution node is not a block node")
            return

        # Note that a single CNTK Convolutional function block is equivalent to the following 3 ELL layers:
        # - ConvolutionalLayer
        # - BiasLayer
        # - ActivationLayer. This layer is sometimes missing, depending on activation type.
        #
        # Therefore, make sure the output padding characteristics of the last layer reflect the next layer's
        # padding requirements.

        weightsTensor = converters.get_float_tensor_from_cntk_convolutional_weight_parameter(
            self.weights_parameter)
        biasVector = converters.get_float_vector_from_cntk_trainable_parameter(
            self.bias_parameter)

        # Create the ELL.LayerParameters for the various ELL layers
        firstLayerParameters = ELL.LayerParameters(
            self.layer.ell_inputShape, self.layer.ell_inputPaddingParameters, self.layer.ell_outputShapeMinusPadding, ELL.NoPadding())
        middleLayerParameters = ELL.LayerParameters(self.layer.ell_outputShapeMinusPadding, ELL.NoPadding(
        ), self.layer.ell_outputShapeMinusPadding, ELL.NoPadding())
        lastLayerParameters = ELL.LayerParameters(self.layer.ell_outputShapeMinusPadding, ELL.NoPadding(
        ), self.layer.ell_outputShape, self.layer.ell_outputPaddingParameters)

        layerParameters = firstLayerParameters

        # Fill in the convolutional parameters
        weightsShape = self.weights_parameter.shape
        receptiveField = weightsShape[2]
        stride = self.attributes['strides'][2]

        filterBatchSize = layerParameters.outputShape.channels

        internalNodes = get_model_layers(self.layer.block_root)
        activationType = get_activation_type(internalNodes)

        convolutionalParameters = ELL.ConvolutionalParameters(
            receptiveField, stride, self.convolution_method, filterBatchSize)

        # Create the ELL convolutional layer
        ellLayers.append(ELL.FloatConvolutionalLayer(
            layerParameters, convolutionalParameters, weightsTensor))

        # Create the ELL bias layer
        if (SoftmaxLayer.is_softmax_activation(internalNodes) or activationType != None):
            layerParameters = middleLayerParameters
        else:
            layerParameters = lastLayerParameters
        ellLayers.append(ELL.FloatBiasLayer(layerParameters, biasVector))

        # Create the ELL activation layer
        if (SoftmaxLayer.is_softmax_activation(internalNodes) or activationType != None):
            layerParameters = lastLayerParameters

            # Special case: if this is softmax activation, create an ELL Softmax layer.
            # Else, insert an ELL ActivationLayer
            if(SoftmaxLayer.is_softmax_activation(internalNodes)):
                ellLayers.append(ELL.FloatSoftmaxLayer(layerParameters))
            else:
                if (activationType != None):
                    ellLayers.append(ELL.FloatActivationLayer(
                        layerParameters, activationType))

    def convert_binarized_to_ell(self, ellLayers):
        """Helper to convert a binary convolutional layer to the ELL equivalent."""
        if self.layer.is_block:
            print("Error: Layer node is in block node")
            return

        # A CNTK Binary Convolutional layer is a single function.
        # Bias and Activation are separate layers (processed outside of this class).
        weightsTensor = converters.get_float_tensor_from_cntk_convolutional_weight_parameter(
            self.weights_parameter)

        layerParameters = ELL.LayerParameters(
            self.layer.ell_inputShape, self.layer.ell_inputPaddingParameters, self.layer.ell_outputShape,
            self.layer.ell_outputPaddingParameters)

        # Fill in the convolutional parameters
        weightsShape = self.weights_parameter.shape
        receptiveField = weightsShape[2]
        stride = self.attributes['strides'][2]

        convolutionalParameters = ELL.BinaryConvolutionalParameters(
            receptiveField, stride, self.convolution_method, self.weights_scale)

        ellLayers.append(ELL.FloatBinaryConvolutionalLayer(
            layerParameters, convolutionalParameters, weightsTensor))

    def process(self, ellLayers):
        """Appends the ELL equivalent of the current layer to ellLayers."""

        if self.ell_binarized:
            self.convert_binarized_to_ell(ellLayers)
        else:
            self.convert_real_valued_to_ell(ellLayers)

    def print_summary(self):
        """Prints summary info about this layer.
        """
        label = self.op_name
        if self.ell_binarized:
            label += "(binarized)"

        if (self.layer.is_block):
            nodes = get_model_layers(self.layer.block_root)
            if SoftmaxLayer.is_softmax_activation(nodes):
                label += "(softmax)"
            else:
                activation_type = get_activation_type(nodes)
                if activation_type is not None:
                    label += "(" + ell_activation_type_to_string(activation_type) + ")"

        print(label, ": ", ell_shape_to_string(self.layer.ell_inputShape), " -> ",
            ell_shape_to_string(self.layer.ell_outputShape),
            "| input padding", self.layer.ell_inputPaddingParameters.paddingSize,
            " output padding", self.layer.ell_outputPaddingParameters.paddingSize)


class LinearLayer(BaseLayer):
    """Logic for converting a CNTK Linear layer to ELL"""

    def __init__(self, layer):
        self.op_name = 'linear'
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

        weightsParameter = find_parameter_by_name(self.layer.parameters, 'W', 0)
        biasParameter = find_parameter_by_name(self.layer.parameters, 'b', 1)
        weightsTensor = converters.get_float_tensor_from_cntk_dense_weight_parameter(
            weightsParameter)
        biasVector = converters.get_float_vector_from_cntk_trainable_parameter(
            biasParameter)

        # Create the ELL.LayerParameters for the various ELL layers
        firstLayerParameters = ELL.LayerParameters(
            self.layer.ell_inputShape, self.layer.ell_inputPaddingParameters, self.layer.ell_outputShapeMinusPadding, ELL.NoPadding())
        middleLayerParameters = ELL.LayerParameters(self.layer.ell_outputShapeMinusPadding, ELL.NoPadding(
        ), self.layer.ell_outputShapeMinusPadding, ELL.NoPadding())
        lastLayerParameters = ELL.LayerParameters(self.layer.ell_outputShapeMinusPadding, ELL.NoPadding(
        ), self.layer.ell_outputShape, self.layer.ell_outputPaddingParameters)

        layerParameters = firstLayerParameters

        internalNodes = get_model_layers(self.layer.block_root)
        activationType = get_activation_type(internalNodes)

        # Create the ELL fully connected layer
        ellLayers.append(ELL.FloatFullyConnectedLayer(
            layerParameters, weightsTensor))

        # Create the ELL bias layer
        if (SoftmaxLayer.is_softmax_activation(internalNodes) or activationType != None):
            layerParameters = middleLayerParameters
        else:
            layerParameters = lastLayerParameters
        ellLayers.append(ELL.FloatBiasLayer(layerParameters, biasVector))

        # Create the ELL activation layer
        if (SoftmaxLayer.is_softmax_activation(internalNodes) or activationType != None):
            layerParameters = lastLayerParameters

            # Special case: if this is softmax activation, create an ELL Softmax layer.
            # Else, insert an ELL ActivationLayer
            if(SoftmaxLayer.is_softmax_activation(internalNodes)):
                ellLayers.append(ELL.FloatSoftmaxLayer(layerParameters))
            else:
                if (activationType != None):
                    ellLayers.append(ELL.FloatActivationLayer(
                        layerParameters, activationType))


class ElementTimesLayer(BaseLayer):
    """Logic for converting a CNTK ElementTimes layer to ELL"""

    def __init__(self, layer):
        self.op_name = 'ElementTimes'
        super().__init__(layer)

    def process(self, ellLayers):
        """Appends the ELL representation of the current layer to ellLayers."""

        if (len(self.layer.constants) == 1):

            # Create the ELL.LayerParameters for the ELL layer
            layerParameters = ELL.LayerParameters(
                self.layer.ell_inputShape, self.layer.ell_inputPaddingParameters, self.layer.ell_outputShape, self.layer.ell_outputPaddingParameters)

            # Create ELL scaling layer
            scaleValue = self.layer.constants[0].value
            # Workaround: For some reason, np.full is not returning a type that SWIG can parse. So just manually walk the array setting the scalar
            scaleArray = np.arange(
                layerParameters.outputShape.channels, dtype=np.float)
            for i in range(scaleArray.size):
                scaleArray[i] = scaleValue
            scalesVector = ELL.FloatVector(scaleArray)
            ellLayers.append(ELL.FloatScalingLayer(
                layerParameters, scalesVector))

        else:
            print("Error: Skipping ElementTimes layer due to dimensions of Constants")


class BasePoolingLayer(BaseLayer):
    """Common logic for converting a Pooling layer to ELL"""

    def __init__(self, layer):
        if layer.is_block:
            self.attributes = layer.block_root.attributes
        else:
            self.attributes = layer.attributes
        super().__init__(layer)

    def get_input_padding_parameters(self):
        """Returns the ELL.PaddingParameters for a layer's input."""

        if ('autoPadding' in self.attributes):
            if (self.attributes['autoPadding'][0] == True):
                padding = int((self.attributes['poolingWindowShape'][0] - 1) / 2)
            else:
                padding = self.attributes['upperPad'][0]
        else:
            padding = self.attributes['upperPad'][0]

        return ELL.PaddingParameters(self.padding_scheme, padding)

    def process(self, ellLayers):
        """Appends the ELL representation of the current layer to ellLayers."""

        # Create the ELL.LayerParameters for the ELL layer
        layerParameters = ELL.LayerParameters(
            self.layer.ell_inputShape, self.layer.ell_inputPaddingParameters, self.layer.ell_outputShape, self.layer.ell_outputPaddingParameters)

        # Fill in the pooling parameters
        poolingSize = self.attributes['poolingWindowShape'][0]
        stride = self.attributes['strides'][0]

        poolingParameters = ELL.PoolingParameters(poolingSize, stride)

        # Create the ELL pooling layer
        ellLayers.append(ELL.FloatPoolingLayer(
            layerParameters, poolingParameters, self.pooling_type))


class MaxPoolingLayer(BasePoolingLayer):
    """Logic for converting a CNTK MaxPooling layer to ELL"""

    def __init__(self, layer):
        self.op_name = 'MaxPooling'
        self.padding_scheme = ELL.PaddingScheme.min
        self.pooling_type = ELL.PoolingType.max

        super().__init__(layer)


class AveragePoolingLayer(BasePoolingLayer):
    """Logic for converting a CNTK AveragePooling layer to ELL"""

    def __init__(self, layer):
        self.op_name = 'AveragePooling'
        self.padding_scheme = ELL.PaddingScheme.zeros
        self.pooling_type = ELL.PoolingType.mean

        super().__init__(layer)


class PoolingLayer(BasePoolingLayer):
    """Logic for converting a CNTK Pooling layer to ELL"""

    def __init__(self, layer):
        self.op_name = 'Pooling'

        if layer.is_block:
            self.attributes = layer.block_root.attributes
        else:
            self.attributes = layer.attributes

        if (self.attributes['poolingType'] == PoolingType_Max):
            self.padding_scheme = ELL.PaddingScheme.min
            self.pooling_type = ELL.PoolingType.max
        else:
            self.padding_scheme = ELL.PaddingScheme.zeros
            self.pooling_type = ELL.PoolingType.mean
            
        super().__init__(layer)


class ReLULayer(BaseLayer):
    """Logic for converting a CNTK ReLU layer to ELL"""

    def __init__(self, layer):
        self.op_name = 'ReLU'
        super().__init__(layer)

    def process(self, ellLayers):
        """Appends the ELL representation of the current layer to ellLayers."""

        # Create the ELL.LayerParameters for the ELL layer
        layerParameters = ELL.LayerParameters(
            self.layer.ell_inputShape, self.layer.ell_inputPaddingParameters, self.layer.ell_outputShape, self.layer.ell_outputPaddingParameters)

        # Create the ELL activation layer
        ellLayers.append(ELL.FloatActivationLayer(
            layerParameters, ELL.ActivationType.relu))


class LeakyReLULayer(BaseLayer):
    """Logic for converting a CNTK LeakyReLU layer to ELL"""

    def __init__(self, layer):
        self.op_name = 'LeakyReLU'
        super().__init__(layer)

    def process(self, ellLayers):
        """Appends the ELL representation of the current layer to ellLayers."""

        # Create the ELL.LayerParameters for the ELL layer
        layerParameters = ELL.LayerParameters(
            self.layer.ell_inputShape, self.layer.ell_inputPaddingParameters, self.layer.ell_outputShape, self.layer.ell_outputPaddingParameters)

        # Create the ELL activation layer
        ellLayers.append(ELL.FloatActivationLayer(
            layerParameters, ELL.ActivationType.leaky))


class PReLULayer(BaseLayer):
    """Logic for converting a CNTK PReLU layer to ELL"""

    def __init__(self, layer):
        self.op_name = 'PReLU'
        super().__init__(layer)

    def process(self, ellLayers):
        """Appends the ELL representation of the current layer to ellLayers."""

        preluParameter = find_parameter_by_name(self.layer.parameters, 'prelu', 0)

        # TODO
        # preluVector = converters.get_float_vector_from_cntk_trainable_parameter(
        #    preluParameter)

        # Create the ELL.LayerParameters for the ELL layer
        #layerParameters = ELL.LayerParameters(
        #    self.layer.ell_inputShape, self.layer.ell_inputPaddingParameters, self.layer.ell_outputShape, self.layer.ell_outputPaddingParameters)

        # Create the ELL activation layer
        #ellLayers.append(ELL.FloatActivationLayer(
        #    layerParameters, ELL.ActivationType.leaky))


class SoftmaxLayer(BaseLayer):
    """Logic for converting a CNTK Softmax layer to ELL"""

    def __init__(self, layer):
        self.op_name = 'Softmax'
        super().__init__(layer)

    def process(self, ellLayers):
        """Appends the ELL representation of the current layer to ellLayers."""

        # Create the ELL.LayerParameters for the ELL layer
        layerParameters = ELL.LayerParameters(
            self.layer.ell_inputShape, self.layer.ell_inputPaddingParameters, self.layer.ell_outputShape, self.layer.ell_outputPaddingParameters)

        # Create the ELL max pooling layer
        ellLayers.append(ELL.FloatSoftmaxLayer(layerParameters))

    @staticmethod
    def is_softmax_activation(nodes):
        """Returns True is the nodes contain a softmax activation"""
        if (find_node_by_op_name(nodes, 'SoftMax') != None):
            return True
        return False

class BatchNormalizationLayer(BaseLayer):
    """Logic for converting a CNTK BatchNormalization layer to ELL"""

    def __init__(self, layer):
        self.op_name = 'BatchNormalization'
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

        scaleParameter = find_parameter_by_name(self.layer.parameters, 'scale', 0)
        biasParameter = find_parameter_by_name(self.layer.parameters, 'bias', 1)
        meanParameter = find_parameter_by_name(
            self.layer.constants, 'aggregate_mean', 0)
        varianceParameter = find_parameter_by_name(
            self.layer.constants, 'aggregate_variance', 1)

        scaleVector = converters.get_float_vector_from_cntk_trainable_parameter(
            scaleParameter)
        biasVector = converters.get_float_vector_from_cntk_trainable_parameter(
            biasParameter)
        meanVector = converters.get_float_vector_from_cntk_trainable_parameter(
            meanParameter)
        varianceVector = converters.get_float_vector_from_cntk_trainable_parameter(
            varianceParameter)

        # Create the ELL.LayerParameters for the various ELL layers
        firstLayerParameters = ELL.LayerParameters(
            self.layer.ell_inputShape, self.layer.ell_inputPaddingParameters, self.layer.ell_outputShapeMinusPadding, ELL.NoPadding())
        middleLayerParameters = ELL.LayerParameters(self.layer.ell_outputShapeMinusPadding, ELL.NoPadding(
        ), self.layer.ell_outputShapeMinusPadding, ELL.NoPadding())
        lastLayerParameters = ELL.LayerParameters(self.layer.ell_outputShapeMinusPadding, ELL.NoPadding(
        ), self.layer.ell_outputShape, self.layer.ell_outputPaddingParameters)

        # The default CNTK epsilon
        epsilon = 1e-5

        # Create the layers
        ellLayers.append(ELL.FloatBatchNormalizationLayer(
            firstLayerParameters, meanVector, varianceVector, epsilon, ELL.EpsilonSummand_variance))
        ellLayers.append(ELL.FloatScalingLayer(
            middleLayerParameters, scaleVector))
        ellLayers.append(ELL.FloatBiasLayer(lastLayerParameters, biasVector))


class BiasLayer(BaseLayer):
    """Logic for converting a CNTK Plus layer to ELL"""

    def __init__(self, layer):
        # Only accept Plus functions that act as bias layers (one and only one parameter)
        if len(layer.parameters) != 1:
            raise ValueError        

        self.op_name = 'Plus'
        super().__init__(layer)

    def process(self, ellLayers):
        """Appends the ELL representation of the current layer to ellLayers."""

        biasVector = converters.get_float_vector_from_cntk_trainable_parameter(
            self.layer.parameters[0])

        # Create the ELL.LayerParameters for the ELL layer
        layerParameters = ELL.LayerParameters(
            self.layer.ell_inputShape, self.layer.ell_inputPaddingParameters, self.layer.ell_outputShape, self.layer.ell_outputPaddingParameters)

        # Create the ELL bias layer
        ellLayers.append(ELL.FloatBiasLayer(layerParameters, biasVector))


class LayerFactory():
    @staticmethod
    def get_layer_object(cntkLayer):
        try:
            if (cntkLayer.op_name == 'AveragePooling'):
                return AveragePoolingLayer(cntkLayer)
            elif (cntkLayer.op_name == 'BatchNormalization'):
                return BatchNormalizationLayer(cntkLayer)
            elif (cntkLayer.op_name == 'Convolution'):
                return ConvolutionLayer(cntkLayer)
            elif (cntkLayer.op_name == 'Dense'):
                return DenseLayer(cntkLayer)
            elif (cntkLayer.op_name == 'ElementTimes'):
                return ElementTimesLayer(cntkLayer)
            elif (cntkLayer.op_name == 'LeakyReLU'):
                return LeakyReLULayer(cntkLayer)
            elif (cntkLayer.op_name == 'linear'):
                return LinearLayer(cntkLayer)
            elif (cntkLayer.op_name == 'MaxPooling'):
                return MaxPoolingLayer(cntkLayer)
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
                print("\nWill not process", cntkLayer.op_name,
                    "- skipping this layer as irrelevant.")
        except (ValueError, AttributeError):
            # raised if a layer contains invalid characteristics
            print("\nCould not process", cntkLayer.op_name,
                "- skipping this layer as irrelevant.")
            
        return None

    @staticmethod
    def has_inputs(cntkLayer):
        return ((len(cntkLayer.arguments) > 0 and len(cntkLayer.arguments[0].shape) > 0) or
                # special case for Binary Convolution
                (cntkLayer.op_name == "Convolution" and len(cntkLayer.inputs) > 0 and len(cntkLayer.inputs[0].shape) > 0))

def get_filtered_layers_list(modelLayers):
    """Returns a relevant list of CNTK layers and layer objects
    """

    # Go through the layers and append layer objects to the relevantLayers list
    relevantLayers = []
    for currentLayer in modelLayers:
        if (isinstance(currentLayer, cntk_py.Function)):
            if (LayerFactory.has_inputs(currentLayer)):
                layerObject = LayerFactory.get_layer_object(currentLayer)
                if (layerObject is not None):
                    relevantLayers.append(layerObject)
            else:
                print("\nWill not process", currentLayer.op_name,
                    "- empty input shape.")

    # Go through the layers and set the output characteristics:
    # - padding parameters for output, based on the next layer's input
    # - output shape, which is adjusted to include the padding
    for i in range(len(relevantLayers)):
        currentLayer = relevantLayers[i]

        if (i < (len(relevantLayers) - 1)):
            # Use the next layer's input characteristics to set the output for this layer
            nextLayer = relevantLayers[i + 1]
            currentLayer.set_output_characteristics(nextLayer)
        else:
            # This is the last layer, so the output characteristics are known
            currentLayer.set_output_characteristics(None)

        currentLayer.print_summary()
    return relevantLayers

def convert_cntk_layers_to_ell_layers(layersToConvert):
    """Walks a list of CNTK layers and returns a list of ELL Layer objects that is used to construct a Neural Network Predictor"""

    print("\nConstructing equivalent ELL layers from CNTK...")
    ellLayers = []
    for layerObject in layersToConvert:
        print("Converting layer ", layerObject.layer)
        layerObject.process(ellLayers)

    print("\n...Finished constructing ELL layers.")

    return ellLayers
