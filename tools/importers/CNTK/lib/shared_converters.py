####################################################################################################
#
# Project:  Embedded Learning Library (ELL)
# File:     shared_converters.py (importers)
# Authors:  Byron Changuion
#
# Requires: Python 3.x
#
####################################################################################################

import logging
from enum import Enum, auto
import typing

from cntk import parameter, constant, load_model
from cntk.layers.typing import *
from cntk.ops import *
import cntk.logging.graph as graph

import ell

_logger = logging.getLogger(__name__)

class ImporterNode:
    """
    Common class for intermediate representation of nodes in the importer.
    The core importer engine can convert ImporterNodes into ELL Nodes
    and insert them into an ELL Model.
    """

    def __init__(self, id: str,
        operation_type: str, 
        inputs: typing.Sequence[str] = [],
        outputs: typing.Sequence[str] = [],
        weights: typing.Mapping[str, typing.Any] = {},
        attributes: typing.Mapping[str, typing.Any] = {},
        padding: typing.Mapping[str, typing.Any] = {},
        input_shapes: typing.Sequence[typing.Any] = [],
        output_shapes: typing.Sequence[typing.Any] = []):
        """
        id: unique identifier for this node
        operation_type: string name of the operation type to be imported. 
            This will get mapped to an ELL operation via the operation_map.
        inputs: array of strings representing where the input comes from.
        outputs: array of strings representing the output tensors.
        weights: dictionary of weight parameter labels to weight names e.g. a 
            convolutional node may have {'weights': 'w123', 'bias': 'b832'}.
            Dictionary keys are specific to the ELL operation.
        attributes: dictionary of attribute names and values e.g. a
            convolutional node may have {'size': 3, 'step': 1, 'pad': 0 }.
            Dictionary keys are specific to the ELL operation.
        padding: dictionary of padding size and padding scheme e.g.
                {"size": 0, "scheme": ell.neural.PaddingScheme.zeros}
        input_shapes: array of tuples representing input shapes and ordering
                e.g. ((3,64,64), "channel_row_column").
        output_shapes: array of tuples representing output shapes and ordering
                e.g. ((32,8,8), "channel_row_column").
        """
        self.id = id
        self.operation_type = operation_type
        self.weights = weights
        self.inputs = inputs
        self.outputs = outputs
        self.attributes = attributes
        self.padding = padding
        self.output_padding = {"size": 0, "scheme": ell.neural.PaddingScheme.zeros}
        self.input_shapes = input_shapes
        self.output_shapes = output_shapes

    def __repr__(self):
        line1 = "{}: {} -> {}, {}\n".format(self.operation_type, self.inputs, self.outputs, self.attributes)
        line2 = "    output_shape {}\n".format(self.output_shapes[0])
        line3 = "    padding {}\n".format(self.padding)
        line4 = "    output_padding {}\n".format(self.output_padding)
        return line1 + line2 + line3 + line4

class LookupTable:
    """
    A helper class that stores the typing.Mappings between:
    - tensor id to a tuple containing (tensor value, tensor order)
    - ELL id to ELL node. These get created during the conversion process.
    - importer node id to ELL ids. These get created during the conversion 
        process. Note that one ImporterNode could get converted to multiple
        ELL nodes.
    In addition, there are convenience methods for accessing the tenspors
    in appropriate ELL order.
    """
    def __init__(self, tensors: typing.Mapping[str, typing.Any]):
        self.ell_id_to_ell_nodes = {}
        self.importer_id_to_ell_ids = {}
        self.tensors = tensors

    def add_imported_ell_node(self, importer_node_id: str, ell_node_id: str, ell_node: typing.Any):
        """
        Adds an ImporterNode and associated ELL node to the lookup.
        """
        self.ell_id_to_ell_nodes[ell_node_id] = ell_node
        if importer_node_id in self.importer_id_to_ell_ids:
            self.importer_id_to_ell_ids[importer_node_id] = self.importer_id_to_ell_ids[importer_node_id].append(ell_node_id)
        else:
            self.importer_id_to_ell_ids[importer_node_id] = [ell_node_id]

    def add_ell_node(self, ell_node_id: str, ell_node: typing.Any):
        """
        Adds an ELL node to the lookup.
        """
        self.ell_id_to_ell_nodes[ell_node_id] = ell_node

    def get_ell_id(self, importer_node_id: str):
        """
        Return the id of the last ELL node associated with this importer node.
        """
        id = None
        if importer_node_id in self.importer_id_to_ell_ids:
            id = self.importer_id_to_ell_ids[importer_node_id][-1]
        return id 

    def get_ell_node(self, importer_node_id: str):       
        """
        Return the last ELL node associated with this importer node.
        """
        node = None
        if importer_node_id in self.importer_id_to_ell_ids:
            id = self.importer_id_to_ell_ids[importer_node_id][-1]
            if id in self.ell_id_to_ell_nodes:
                node = self.ell_id_to_ell_nodes[id]
        return node

    def get_tensor_in_ell_order(self, uid: str):
        """
        Returns a numpy array in ELL order
        """
        original_tensor, order = self.tensors[uid]
        original_shape = original_tensor.shape
        if order == "filter_channel_row_column":
            ordered_weights = np.moveaxis(original_tensor, 1, -1)
            ordered_weights = ordered_weights.ravel().astype(np.float).reshape(
                original_shape[0] * original_shape[2], original_shape[3], original_shape[1])
        elif order == "row_column":
            ordered_weights = np.moveaxis(original_tensor, 0, -1)
            ordered_weights = ordered_weights.ravel().astype(
                np.float).reshape(original_shape[1], original_shape[0], 1)
        elif order == "channel":
            ordered_weights = original_tensor.ravel().astype(
                np.float).reshape(1, 1, original_shape.size)
        elif order == "channel_row_column_filter":
            ordered_weights = original_tensor
            ordered_weights = np.moveaxis(ordered_weights, 0, -1)
            ordered_weights = np.moveaxis(ordered_weights, 2, 0)
            ordered_weights = ordered_weights.ravel().astype(np.float).reshape(
                original_shape[3] * original_shape[1], original_shape[2], original_shape[0])
        else:
            raise NotImplementedError(
                "Unsupported tensor order {}, for {}".format(order, uid))

        return ordered_weights

    def get_vector_from_constant(self, uid: str, size: int):
        """
        Returns a single dimensional numpy array containing the tensor weights.
        If the tensor is actually a scalar, expand it to be a vector of length
        'size'.
        """
        original_vector, order = self.tensors[uid]
        # Workaround: For some reason, np.full is not returning a type that SWIG can parse.
        # So just manually walk the array setting the scalar
        array = np.zeros(size, dtype=np.float)
        for i in range(array.size):
            array[i] = original_vector
        return array

    def get_vector_in_ell_order(self, uid: str):
        """
        Returns a single dimensional numpy array containing the tensor weights.
        """
        original_vector, order = self.tensors[uid]

        ordered_weights = np.zeros(original_vector.size, dtype=np.float)
        i = 0
        for value in original_vector:
            ordered_weights[i] = value
            i += 1

        return ordered_weights
    
    def get_tensor_info(self, uid: str):
        """
        Returns a tuple containing (shape, order) for the tensor.
        """
        value, order = self.tensors[uid]
        return (value.shape, order)
        

class ConvertBase:
    """
    Base class for converting an ImporterNode into an ELL Node
    """
    def __init__(self, node: ImporterNode):
        """
        Derived classes should initialize their required_inputs,
        required_weights and required_attributes
        """
        self.required_weights = []
        self.required_attributes = []
        self.importer_node = node

    def can_convert(self) -> bool:
        """
        Verify that the node contains the necessary inputs, weights and 
        attributes to convert. Nodes that cannot be converted due to 
        missing weights or attributes are deemed optional and are skipped.
        See comments in operation_map for examples.
        """
        for w in self.required_weights:
            if w not in self.importer_node.weights:
                return False
        for attr in self.required_attributes:
            if attr not in self.importer_node.attributes:
                return False
        return True

    def get_input_parameters(self, first_in_block = True, input_index = 0):
        """
        Return the input shape and padding parameters as a tuple.
        first_in_block - indicates whether this will be the first ell 
        node in a block. If it is, it will have its padding requirements set
        differently.
        input_index - indicates the index of the input shape requested.
        """
        shape_entry = self.importer_node.input_shapes[input_index]
        padding = 0
        if first_in_block:
            padding = self.importer_node.padding["size"]
        else:
            shape_entry = self.importer_node.output_shapes[0]
        ell_shape = self.get_ell_shape(shape_entry[0], shape_entry[1], padding)
        ell_padding_parameter = ell.neural.PaddingParameters(self.importer_node.padding["scheme"], padding)
        return (ell_shape, ell_padding_parameter)
        
    def get_output_parameters(self, last_in_block = True, output_index = 0):
        """
        Return the output shape and padding parameters as a tuple.
        last_in_block - indicates whether this will be the last ell 
        node in a block. If it is, it will have its output padding set
        differently.
        """
        shape_entry = self.importer_node.output_shapes[output_index]
        padding = 0
        if last_in_block:
            padding = self.importer_node.output_padding["size"]
        ell_shape = self.get_ell_shape(shape_entry[0], shape_entry[1], padding)
        ell_padding_parameter = ell.neural.PaddingParameters(self.importer_node.output_padding["scheme"], padding)
        return (ell_shape, ell_padding_parameter)

    def get_layer_parameters(self, conversion_parameters: typing.Mapping[str, typing.Any]):
        """
        Return the ELL layer parameters for this node.
        """
        input_shape, input_padding = self.get_input_parameters(conversion_parameters["first_in_block"])
        output_shape, output_padding = self.get_output_parameters(conversion_parameters["last_in_block"])

        return ell.neural.LayerParameters(input_shape, input_padding, output_shape, output_padding)

    def get_ell_shape(self, shape: tuple, order: str, padding: int = 0):
        """
        Return the shape in ELL canonical order
        """
        if order == "channel_row_column":
            channels = shape[0]
            rows = shape[1] + 2*padding
            columns = shape[2] + 2*padding
        elif order == "channel":
            channels = shape[0]
            rows = 1 + 2*padding
            columns = 1 + 2*padding
        else:
            raise NotImplementedError(
                "Unsupported input shape order: {}".format(order))
        return ell.math.TensorShape(rows, columns, channels)

    def get_ell_tensor(self, uid: str, conversion_parameters: typing.Mapping[str, typing.Any]):
        """
        Returns a weight tensor as an ELL tensor
        """
        lookup_table = conversion_parameters["lookup_table"]
        return ell.math.FloatTensor(lookup_table.get_tensor_in_ell_order(uid))

    def get_vector(self, uid: str, conversion_parameters: typing.Mapping[str, typing.Any]):
        """
        Returns a weight tensor as a 1 dimensional numpy array. If the 
        original tensor is a scalar, it will be expanded to a vector of size
        equal to the number of output channels.
        """
        lookup_table = conversion_parameters["lookup_table"]
        shape, order = lookup_table.get_tensor_info(uid)

        if len(shape) == 0:
            shape_entry = self.importer_node.output_shapes[0]
            ell_shape = self.get_ell_shape(shape_entry[0], shape_entry[1], 0)
            vector = lookup_table.get_vector_from_constant(uid, ell_shape.channels)
        else:
            vector = lookup_table.get_vector_in_ell_order(uid)
        
        return vector

    def get_ell_vector(self, uid: str, conversion_parameters: typing.Mapping[str, typing.Any]):
        """
        Returns a weight tensor as an ELL vector. If the original tensor is a
        scalar, it will be expanded to a vector of size equal to the number of
        output channels.
        """
        return ell.math.FloatVector(self.get_vector(uid, conversion_parameters))

    def convert(self, conversion_parameters: typing.Mapping[str, typing.Any]):
        """
        Derived classes override to return the appropriate ELL node
        """
        return None

class ConvertActivation(ConvertBase):
    """
    Converter for Activation
    """
    def __init__(self, node: ImporterNode):
        super().__init__(node)
        self.required_weights = []
        self.required_attributes = ["activation"]
        
    def convert(self, conversion_parameters: typing.Mapping[str, typing.Any]):
        """
        Return the appropriate ELL node
        """
        layer_parameters = self.get_layer_parameters(conversion_parameters)
        activation = self.importer_node.attributes["activation"]

        return ell.neural.FloatActivationLayer(layer_parameters, activation)

class ConvertAveragePooling(ConvertBase):
    """
    Converter for Average Pooling
    """
    def __init__(self, node: ImporterNode):
        super().__init__(node)
        self.required_weights = []
        self.required_attributes = ["size", "stride"]
        
    def convert(self, conversion_parameters: typing.Mapping[str, typing.Any]):
        """
        Return the appropriate ELL node
        """
        layer_parameters = self.get_layer_parameters(conversion_parameters)

        attributes = self.importer_node.attributes
        pooling_parameters = ell.neural.PoolingParameters(
            attributes["size"], attributes["stride"])

        # Create the ELL pooling layer
        return ell.neural.FloatPoolingLayer(layer_parameters, 
            pooling_parameters, ell.neural.PoolingType.mean)

class ConvertBatchNormalization(ConvertBase):
    """
    Converter for BatchNormalization
    """
    def __init__(self, node: ImporterNode):
        super().__init__(node)
        self.required_weights = ["mean", "variance"]
        self.required_attributes = []
        self.epsilon = 1e-5
        
    def convert(self, conversion_parameters: typing.Mapping[str, typing.Any]):
        """
        Derived classes override to return the appropriate ELL node
        """
        layer_parameters = self.get_layer_parameters(conversion_parameters)

        mean_vector = self.get_ell_vector(
            self.importer_node.weights["mean"][0], conversion_parameters)
        variance_vector = self.get_ell_vector(
            self.importer_node.weights["variance"][0], conversion_parameters)

        return ell.neural.FloatBatchNormalizationLayer(layer_parameters, 
            mean_vector, variance_vector, self.epsilon, 
            ell.neural.EpsilonSummand.variance)

class ConvertBias(ConvertBase):
    """
    Converter for Bias
    """
    def __init__(self, node: ImporterNode):
        super().__init__(node)
        self.required_weights = ["bias"]
        self.required_attributes = []
        
    def convert(self, conversion_parameters: typing.Mapping[str, typing.Any]):
        """
        Derived classes override to return the appropriate ELL node
        """
        layer_parameters = self.get_layer_parameters(conversion_parameters)
        bias = self.get_ell_vector(
            self.importer_node.weights["bias"][0], conversion_parameters)
        input_shape = layer_parameters.inputShape
        output_shape = layer_parameters.outputShape

        return ell.neural.FloatBiasLayer(layer_parameters, bias)

class ConvertBinaryConvolution(ConvertBase):
    """
    Converter for BinaryConvolution
    """
    def __init__(self, node: ImporterNode):
        super().__init__(node)
        self.required_weights = ["weights"]
        self.required_attributes = ["size", "stride"]
        
    def convert(self, conversion_parameters: typing.Mapping[str, typing.Any]):
        """
        Derived classes override to return the appropriate ELL node
        """
        return None

class ConvertConvolution(ConvertBase):
    """
    Converter for Convolution
    """
    def __init__(self, node: ImporterNode):
        super().__init__(node)
        self.required_weights = ["weights"]
        self.required_attributes = ["size", "stride"]
        
    def convert(self, conversion_parameters: typing.Mapping[str, typing.Any]):
        """
        Derived classes override to return the appropriate ELL node
        """
        layer_parameters = self.get_layer_parameters(conversion_parameters)

        weights = self.get_ell_tensor(
            self.importer_node.weights["weights"][0], conversion_parameters)

        attributes = self.importer_node.attributes
        convolutional_parameters = ell.neural.ConvolutionalParameters(
            attributes["size"], attributes["stride"], 0, 1)

        return ell.neural.FloatConvolutionalLayer(layer_parameters,
            convolutional_parameters, weights)

class ConvertFullyConnected(ConvertBase):
    """
    Converter for FullyConnected
    """
    def __init__(self, node: ImporterNode):
        super().__init__(node)
        self.required_weights = ["weights"]
        self.required_attributes = []
        
    def convert(self, conversion_parameters: typing.Mapping[str, typing.Any]):
        """
        Derived classes override to return the appropriate ELL node
        """
        layer_parameters = self.get_layer_parameters(conversion_parameters)

        weights = self.get_ell_tensor(
            self.importer_node.weights["weights"][0], conversion_parameters)

        return ell.neural.FloatFullyConnectedLayer(
            layer_parameters, weights)

class ConvertElementTimes(ConvertBase):
    """
    Converter for Element Times, which is equivalent to Scaling
    """
    def __init__(self, node: ImporterNode):
        super().__init__(node)
        self.required_weights = ["scale"]
        self.required_attributes = []
        
    def convert(self, conversion_parameters: typing.Mapping[str, typing.Any]):
        """
        Derived classes override to return the appropriate ELL node
        """
        return None

class ConvertInput(ConvertBase):
    """
    Converter for Input
    """
    def __init__(self, node: ImporterNode):
        super().__init__(node)
        self.required_weights = []
        self.required_attributes = []
        
    def convert(self, conversion_parameters: typing.Mapping[str, typing.Any]):
        """
        Derived classes override to return the appropriate ELL node
        """
        # Skip processing the input. It is implicit when using
        # ELL Layers
        return None

class ConvertLeakyReLU(ConvertBase):
    """
    Converter for LeakyReLU, which is equivalent to
    Activation
    """
    def __init__(self, node: ImporterNode):
        super().__init__(node)
        self.required_weights = []
        self.required_attributes = []
        
    def convert(self, conversion_parameters: typing.Mapping[str, typing.Any]):
        """
        Derived classes override to return the appropriate ELL node
        """
        return None

class ConvertMaxPooling(ConvertBase):
    """
    Converter for Max Pooling
    """
    def __init__(self, node: ImporterNode):
        super().__init__(node)
        self.required_weights = []
        self.required_attributes = ["size", "stride"]
        
    def convert(self, conversion_parameters: typing.Mapping[str, typing.Any]):
        """
        Return the appropriate ELL node
        """
        layer_parameters = self.get_layer_parameters(conversion_parameters)

        attributes = self.importer_node.attributes
        pooling_parameters = ell.neural.PoolingParameters(
            attributes["size"], attributes["stride"])

        # Create the ELL pooling layer
        return ell.neural.FloatPoolingLayer(layer_parameters, 
            pooling_parameters, ell.neural.PoolingType.max)

class ConvertMinus(ConvertBase):
    """
    Converter for Minus, which is equivalent to
    a negative Bias
    """
    def __init__(self, node: ImporterNode):
        super().__init__(node)
        self.required_weights = ["bias"]
        self.required_attributes = []
        
    def convert(self, conversion_parameters: typing.Mapping[str, typing.Any]):
        """
        Return the appropriate ELL node
        """
        layer_parameters = self.get_layer_parameters(conversion_parameters)
        bias = self.get_vector(
            self.importer_node.weights["bias"][0], conversion_parameters)
        # Minus is a negative bias in ELL. Negate the bias values so we
        # can use an additive bias layer.
        bias = -1.0 * bias        

        return ell.neural.FloatBiasLayer(layer_parameters, 
            ell.math.FloatVector(bias))

class ConvertPassthrough(ConvertBase):
    """
    Converter for Passthrough, which has information on inputs and outputs but
    doesn't produce typing.Any ELL nodes/layers.
    It's sole purpose is to preserve connections between nodes during the conversion
    process.
    """
    def __init__(self, node: ImporterNode):
        super().__init__(node)
        
    def convert(self, conversion_parameters: typing.Mapping[str, typing.Any]):
        """
        Return nothing
        """
        return None

class ConvertPlus(ConvertBase):
    """
    Converter for Plus, which is equivalent to
    Bias
    """
    def __init__(self, node: ImporterNode):
        super().__init__(node)
        self.required_weights = ["bias"]
        self.required_attributes = []
        
    def convert(self, conversion_parameters: typing.Mapping[str, typing.Any]):
        """
        Return the appropriate ELL node
        """
        return None

class ConvertPooling(ConvertBase):
    """
    Converter for Pooling
    """
    def __init__(self, node: ImporterNode):
        super().__init__(node)
        self.required_weights = []
        self.required_attributes = ["size", "stride", "poolingType"]
        
    def convert(self, conversion_parameters: typing.Mapping[str, typing.Any]):
        """
        Return the appropriate ELL node
        """
        return None

class ConvertPReLU(ConvertBase):
    """
    Converter for PReLU, which is equivalent to
    Activation
    """
    def __init__(self, node: ImporterNode):
        super().__init__(node)
        self.required_weights = []
        self.required_attributes = []
        
    def convert(self, conversion_parameters: typing.Mapping[str, typing.Any]):
        """
        Return the appropriate ELL node
        """
        return None

class ConvertReLU(ConvertBase):
    """
    Converter for ReLU, which is equivalent to
    Activation
    """
    def __init__(self, node: ImporterNode):
        super().__init__(node)
        self.required_weights = []
        self.required_attributes = []
        
    def convert(self, conversion_parameters: typing.Mapping[str, typing.Any]):
        """
        Return the appropriate ELL node
        """
        return None

class ConvertScaling(ConvertBase):
    """
    Converter for Scaling
    """
    def __init__(self, node: ImporterNode):
        super().__init__(node)
        self.required_weights = ["scale"]
        self.required_attributes = []
        
    def convert(self, conversion_parameters: typing.Mapping[str, typing.Any]):
        """
        Return the appropriate ELL node
        """
        layer_parameters = self.get_layer_parameters(conversion_parameters)
        scale = self.get_ell_vector(
            self.importer_node.weights["scale"][0], conversion_parameters)

        return ell.neural.FloatScalingLayer(layer_parameters, scale)
        
class ConvertSoftmax(ConvertBase):
    """
    Converter for Softmax
    """
    def __init__(self, node: ImporterNode):
        super().__init__(node)
        self.required_weights = []
        self.required_atteamstributes = []
        
    def convert(self, conversion_parameters: typing.Mapping[str, typing.Any]):
        """
        Return the appropriate ELL node
        """
        layer_parameters = self.get_layer_parameters(conversion_parameters)
        return ell.neural.FloatSoftmaxLayer(layer_parameters)

class ConvertSplice(ConvertBase):
    """
    Converter for Splice, which for now is Output followed by
    Reshape
    """
    def __init__(self, node: ImporterNode):
        super().__init__(node)
        self.required_weights = []
        self.required_attributes = []
        
    def convert(self, conversion_parameters: typing.Mapping[str, typing.Any]):
        """
        Return the appropriate ELL node
        """
        return None

class ConvertReshape(ConvertBase):
    """
    Converter for Reshape
    """
    def __init__(self, node: ImporterNode):
        super().__init__(node)
        self.required_weights = []
        self.required_attributes = []
        
    def convert(self, conversion_parameters: typing.Mapping[str, typing.Any]):
        """
        Return the appropriate ELL node
        """
        return None
