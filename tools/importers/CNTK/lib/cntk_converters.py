####################################################################################################
#
# Project:  Embedded Learning Library (ELL)
# File:     cntk_converters.py (importers)
# Authors:  Byron Changuion
#
# Requires: Python 3.x, cntk-2.4
#
####################################################################################################

"""Converts CNTK data structures to ELL equivalents"""
import logging

import numpy as np
from cntk.initializer import glorot_uniform, he_normal
from cntk.layers import Convolution, MaxPooling, AveragePooling, Dropout, BatchNormalization, Dense
import cntk.layers.blocks
from cntk.layers.typing import *
from cntk.ops import *
from cntk.logging.graph import *

import ell
from custom_functions import CustomSign, BinaryConvolution
import lib.shared_importer as shared_importer
from lib.cntk_utilities import *

_logger = logging.getLogger(__name__)

def get_float_vector_from_constant(constant, size):
    # Workaround: For some reason, np.full is not returning a type that SWIG can parse. So just manually walk the array setting the scalar
    array = np.zeros(size, dtype=np.float)
    for i in range(array.size):
        array[i] = constant
    return ell.math.FloatVector(array)


def get_float_vector_from_cntk_trainable_parameter(tensorParameter):
    """Returns an ell.math.FloatVector from a trainable parameter
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

    return ell.math.FloatVector(orderedWeights)


def get_float_vector_from_cntk_array(inputArray):
    """Returns an ell.math.FloatTensor from a 4, 3, 2, or 1 dimensional numpy array.
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
        orderedWeights = orderedWeights.reshape(1, 1, inputArray.size)
    else:
        _logger.error("Error: Input array has incorrect dimensions")
        return None

    return np.ravel(orderedWeights)


def get_float_tensor_from_cntk_dense_weight_parameter(tensorParameter):
    """Returns an ell.math.FloatTensor from a trainable parameter
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

    return ell.math.FloatTensor(orderedWeights)


def get_float_tensor_from_cntk_convolutional_weight_parameter(tensorParameter):
    """Returns an ell.math.FloatTensor from a trainable parameter
       Note that ELL's ordering is row, column, channel.
       4D parameters (e.g. those that represent convolutional weights) are stacked vertically in the row dimension.
       CNTK has them in filter, channel, row, column order.
    """
    tensorShape = tensorParameter.shape
    tensorValue = tensorParameter.value
    return get_float_tensor_from_cntk_convolutional_weight_value_shape(tensorParameter.value, tensorParameter.shape)


def get_float_tensor_from_cntk_convolutional_weight_value_shape(tensorValue, tensorShape):
    """Returns an ell.math.FloatTensor from a trainable parameter
       Note that ELL's ordering is row, column, channel.
       4D parameters (e.g. those that represent convolutional weights) are stacked vertically in the row dimension.
       CNTK has them in filter, channel, row, column order.
    """
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
    return ell.math.FloatTensor(orderedWeights)


###############################################################################
# Code above this belongs to the legacy importer and will be removed
# once importing Nodes comes online.
###############################################################################

class CntkStandardConverter():
    """
    Base class converter for a CNTK node. It's primary purpose is to create an
    an ImporterNode, which is the intermediate object used by the 
    ImporterEngine to describe model nodes in a common way.

    It can be initialized with:
    cntk_node - the CNTK node
    weight_index_mapping - a mapping of the weights that will be imported using this node.
        See CntkStandardConverter.get_weights for more info.
    importer_op_name - The ELL ImporterNode operation for this CNTK node. If not set,
        then the CNTK node's 'op_name' will be used, otherwise override it here e.g.
        "Dense" and "linear" nodes in CNTK are equivalent to "FullyConnected" in ELL.
        A list of supported operations can be retrieved with:
            ImporterEngine().get_supported_operation_types()
        All nodes to be imported must map to one of these operations.
    """
    def __init__(self, cntk_node, weight_index_mapping = {}, importer_op_name = None):
        self.cntk_node = cntk_node
        if importer_op_name:
            self.importer_op_name = importer_op_name
        else:
            self.importer_op_name = cntk_node.op_name
        self.uid = cntk_node.uid

        self.activation = None
        # Get activation function for block nodes
        if importer_op_name == "Convolution":
            if hasattr(self.cntk_node, "block_root") and self.cntk_node.is_block:
                internal_nodes = Utilities.get_model_nodes(self.cntk_node.block_root)
                self.activation = Utilities.get_ell_activation_type(internal_nodes)

        # Fill in members based on methods that base classes may override
        self.inputs = self.get_input_ids()
        self.input_shapes = self.get_input_shapes()
        self.weights = self.get_weights(weight_index_mapping)
        self.outputs = self.get_output_ids()
        self.output_shapes = self.get_output_shapes()
        self.attributes = self.get_attributes()
        self.padding = self.get_padding()
        
    def log(self):
        """
        Used for diagnostics
        """
        _logger.info("{}.{}, inputs {} -> outputs {}".format(self.importer_op_name, self.uid, self.inputs, self.outputs))
        _logger.info("    weights: {}".format("".join(["({}: {}{},order='{}')".format(w, self.weights[w][0], self.weights[w][1].shape, self.weights[w][2]) for w in self.weights.keys()])))
        _logger.info("    attributes: {}".format(self.attributes))
        _logger.info("    padding: {}".format(self.padding))

    def get_weights(self, weight_index_mapping):
        """
        Returns a mapping of the weights, where each entry is:
            name: (uid, numpy array value, order)
        weight_index_mapping - contains a mapping of the name of the weight 
        tensor to the index where it can be found in the inputs and the order 
        of the elements.
        e.g. on a Convolution block, the mapping would be
        {"weights": 0, "bias": 1}, or,
        {"weights": (0, "filter_channel_row_column", "bias": (1, "channel_row_column")}
        """
        weights = {}
        for label in weight_index_mapping.keys():
            tensor = None
            weight_mapping = weight_index_mapping[label]
            if isinstance(weight_mapping, tuple):
                order = weight_mapping[1]
                uid, tensor = self.get_tensor_at_input_index(weight_mapping[0])
            else:
                uid, tensor = self.get_tensor_at_input_index(weight_mapping)
                order = ""
            # Add the tensor if exists. If it doesn't, then it means
            # the mapping contains an optional value that is missing
            # e.g. {"weights"}
            if tensor is not None:
                weights[label] = (uid, tensor, order)
        
        return weights

    def get_shape_at_input(self, uid):
        """
        Returns a tensor with the specified uid of cntk_node.inputs
        """
        shape = None

        for i in self.cntk_node.inputs:
            if uid == i.uid:
                shape = i.shape

        return shape

    def get_shape_at_output(self, uid):
        """
        Returns a tensor with the specified uid of cntk_node.outputs
        """
        shape = None

        for i in self.cntk_node.outputs:
            if uid == i.uid:
                shape = i.shape

        return shape

    def get_tensor_at_input_index(self, index):
        """
        Returns a tensor at the specified index of cntk_node.inputs
        as a tuple:
        (uid, numpy array)
        """
        tensor = None
        uid = None

        if index < len(self.cntk_node.inputs):
            uid = self.cntk_node.inputs[index].uid
            # Only get tensor values for Parameters and Contants. Things like Input
            # should be skipped.
            if self.cntk_node.inputs[index].is_parameter:
                tensor = self.cntk_node.inputs[index].as_parameter().value
            elif self.cntk_node.inputs[index].is_constant:
                tensor = self.cntk_node.inputs[index].as_constant().value

        return (uid, tensor)

    def get_input_ids(self):
        """
        Returns ids for inputs into this node. Only those that are true inputs
        to the model, or outputs from other nodes are considered inputs.
        Other 'inputs' in CNTK are parameters and constants, but those are
        weights to the ELL importer.
        """
        ids = []
        for i in self.cntk_node.inputs:
            if i.is_input or i.is_output:
                ids.append(Utilities.get_uid(i))
        return ids

    def get_output_ids(self):
        """
        Returns list of ids for output of a node.
        """
        ids = []
        for i in self.cntk_node.outputs:
            if i.is_output:
                ids.append(Utilities.get_uid(i))
        return ids

    def get_attributes(self):
        """
        Returns attributes for this node. Derived classes that have custom
        attributes should override.
        """
        attributes = {}
        if self.activation:
            attributes["activation"] = self.activation
            
        return attributes

    def get_padding(self):
        """
        Derived classes can override. Return is a mapping:
        {"size": size_value, "scheme": scheme_value}
        where:
        size - size of padding
        scheme - padding scheme to use, one of:
          ell.neural.PaddingScheme.zeros
          ell.neural.PaddingScheme.minusOnes
          ell.neural.PaddingScheme.alternatingZeroAndOnes
          ell.neural.PaddingScheme.min
          ell.neural.PaddingScheme.max
        """
        return {"size": 0, "scheme": ell.neural.PaddingScheme.zeros}
    
    def get_input_shapes(self):
        """
        Return list of input shapes. 
        Typically, derived classes do not need to override.
        """
        shapes = []
        for uid in self.inputs:
            shape = self.get_shape_at_input(uid)
            if len(shape) > 1:
                shapes.append((shape, "channel_row_column"))
            else:
                shapes.append((shape, "channel"))
                
        return shapes

    def get_output_shapes(self):
        """
        Return list of output shapes.
        Typically, derived classes do not need to override.
        """
        shapes = []
        if hasattr(self.cntk_node, "outputs"):
            for uid in self.outputs:
                shape = self.get_shape_at_output(uid)
                if len(shape) > 1:
                    shapes.append((shape, "channel_row_column"))
                else:
                    shapes.append((shape, "channel"))
                
        return shapes

    def convert(self):
        """
        Return an instance of the ELL ImporterNode for the CNTK node.
        """
        return shared_importer.ImporterNode(self.cntk_node.uid, 
            self.importer_op_name, self.inputs, self.outputs, 
            self.weights, self.attributes, self.padding,
            self.input_shapes, self.output_shapes)

class CntkConvolutionConverter(CntkStandardConverter):
    """
    Custom converter for Convolutions.
    """
    def __init__(self, cntk_node):
        convolutional_nodes = depth_first_search(cntk_node.block_root, 
            lambda x: Utilities.op_name_equals(x, 'Convolution'))
        self.convolutional_node = convolutional_nodes[0]
        super().__init__(cntk_node, {"weights": (0, "filter_channel_row_column"), "bias": (1, "channel")}, "Convolution")

    def get_attributes(self):
        attributes = {}
        attributes["size"] = self.convolutional_node.W.shape[2]
        attributes["stride"] = self.convolutional_node.attributes['strides'][2]
        if self.activation:
            attributes["activation"] = self.activation
            
        return attributes

    def get_padding(self):
        receptive_field = self.attributes["size"]
        return Utilities.get_padding_for_layer_with_sliding_window(
            self.convolutional_node.attributes, receptive_field, 
            ell.neural.PaddingScheme.zeros)

class CntkDenseConverter(CntkStandardConverter):
    """
    Custom converter for Dense, linear, fully connected etc.
    """
    def __init__(self, cntk_node):
        # Check the dimensions of the weights to get ordering
        weights_shape = cntk_node.inputs[0].shape
        order = "channel_row_column"
        if len(weights_shape) == 4:
            order = "channel_row_column_filter"
        elif len(weights_shape) == 2:
            order = "row_column"
        super().__init__(cntk_node, {"weights": (0, order), "bias": (1, "channel")}, "FullyConnected")

    def get_attributes(self):
        attributes = {}
        if self.activation:
            attributes["activation"] = self.activation
            
        return attributes

class CntkElementTimesConverter(CntkStandardConverter):
    """
    Custom converter for ElementTimes.
    """
    def __init__(self, cntk_node):
        # Until we support arbitrary connections between nodes, look for the
        # input that is not an output to denote the scale.
        indexes = [index for index,value in enumerate(cntk_node.inputs) if not value.is_output]
        if len(indexes) < 1:
            raise Exception("ElementTimes node {} cannot be converted, the CNTK importer does not currently support arbitrary inputs".format(cntk_node.uid))
        super().__init__(cntk_node, {"scale": (indexes[0], "channel")}, "ElementTimes")

class CntkPoolingConverter(CntkStandardConverter):
    """
    Custom converter for Pooling.
    """
    def __init__(self, cntk_node):
        super().__init__(cntk_node, {}, cntk_node.op_name)

    def get_attributes(self):
        if self.cntk_node.is_block:
            node_attributes = self.cntk_node.block_root.attributes
        else:
            node_attributes = self.cntk_node.attributes

        size = node_attributes['poolingWindowShape'][0]
        stride = node_attributes['strides'][0]
        return {"size": size, "stride": stride}

    def get_padding(self):
        if self.cntk_node.is_block:
            node_attributes = self.cntk_node.block_root.attributes
        else:
            node_attributes = self.cntk_node.attributes
        scheme = ell.neural.PaddingScheme.min
        if self.importer_op_name == "AveragePooling":
            scheme = ell.neural.PaddingScheme.zeros
        pooling_size = self.attributes['size']

        return Utilities.get_padding_for_layer_with_sliding_window(
            node_attributes, pooling_size, scheme)

class CntkSpliceConverter(CntkStandardConverter):
    """
    Custom converter for Splice. Until ELL importer moves to ELL nodes,
    just do the standard conversion.
    """
    def __init__(self, cntk_node):
        super().__init__(cntk_node, {}, cntk_node.op_name)

    def get_attributes(self):
        return {}

class CntkReshapeConverter(CntkStandardConverter):
    """
    Custom converter for Reshape. Until ELL importer moves to ELL nodes,
    just do the standard conversion.
    """
    def __init__(self, cntk_node):
        super().__init__(cntk_node, {}, cntk_node.op_name)

    def get_attributes(self):
        return {}

class CntkInputConverter(CntkStandardConverter):
    """
    Custom converter for Input. Inputs are special nodes who have no
    input of their own, and whose output are themselves.
    """
    def __init__(self, cntk_node):
        self.input_shape = cntk_node.shape
        super().__init__(cntk_node, {}, "Input")

    def get_input_ids(self):
        return []

    def get_output_ids(self):
        return [self.uid]

    def get_output_shapes(self):
        shapes = []
        shape = self.cntk_node.shape
        if len(shape) > 1:
            shapes.append((shape, "channel_row_column"))
        else:
            shapes.append((shape, "channel"))
                
        return shapes

class CntkCrossEntropyWithSoftmaxConverter(CntkStandardConverter):
    """
    Custom converter for CntkCrossEntropyWithSoftmaxConverter. 
    This just equates to a Softmax node in ELL, but in CNTK contains more inputs
    and outputs since it was inserted by the trainer. For conversion to ELL,
    only one input and output is expected.
    """
    def __init__(self, cntk_node):
        super().__init__(cntk_node, {}, "Softmax")

    def get_input_ids(self):
        return [self.cntk_node.inputs[0].uid]

    def get_output_ids(self):
        return [self.cntk_node.outputs[0].uid]

    def get_output_shapes(self):
        # Output shape should be the same as the input shape
        shapes = []
        shape = self.get_shape_at_input(self.cntk_node.inputs[0].uid)
        if len(shape) > 1:
            shapes.append((shape, "channel_row_column"))
        else:
            shapes.append((shape, "channel"))
                
        return shapes        

class CntkConverterFactory():
    @staticmethod
    def get_converter(cntk_node, cntk_converter_map):
        """
        Gets the appropriate converter for the 
        """
        try:
            if cntk_node.op_name in cntk_converter_map:
                if isinstance(cntk_converter_map[cntk_node.op_name],tuple):
                    converter = cntk_converter_map[cntk_node.op_name][0]
                    weights_mapping = cntk_converter_map[cntk_node.op_name][1]
                    if len(cntk_converter_map[cntk_node.op_name]) > 2:
                        operation = cntk_converter_map[cntk_node.op_name][2]
                    else:
                        operation = None
                    return converter(cntk_node, weights_mapping, operation)
                else:
                    converter = cntk_converter_map[cntk_node.op_name]
                    return converter(cntk_node)
            else:
                raise Exception("Cannot not process " + cntk_node.op_name +
                      "- no converter found")
        except (ValueError, AttributeError) as e:
            # raised if a layer contains invalid characteristics
            _logger.info("\nCould not process", cntk_node.op_name, "-", str(e), ", model cannot be imported")

        return None

