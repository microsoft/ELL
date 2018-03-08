####################################################################################################
#
# Project:  Embedded Learning Library (ELL)
# File:     shared_importer.py (importers)
# Authors:  Byron Changuion
#
# Requires: Python 3.5+
#
####################################################################################################

import logging
from enum import Enum, auto
import typing

import ell
from lib.shared_converters import *

_logger = logging.getLogger(__name__)

# The standard Importer operation label to converter map.
# Importer nodes with matching operation string are converted
# using the listed converter class, or list of classes.
#
# A list of classes denotes a block, where a single importer node
# becomes a sequence of ELL nodes e.g.
#   "Convolution": [ConvertConvolution, ConvertBias, ConvertActivation]
# shows how an ImporterNode wih operation_type "Convolution", is actually
# a sequence of 3 ELL operations, namely Convolution, Bias, Activation.
#
# Note that elements in a block can be optional. The importer engine checks 
# whether importer node has all required elements to be converted. If it 
# does not, then that converter is removed from the block. This allows
# for optional conversion elements e.g. 
#   "Convolution": [ConvertConvolution, ConvertBias, ConvertActivation]
#   supports the following variants:
#       - a Convolution followed by bias and activation
#       - a Convolution followed by activation
#       - a Convolution followed by bias
#       - a Convolution
operation_map = {
    "Activation": ConvertActivation,
    "AveragePooling": ConvertAveragePooling,
    "BatchNormalization": [ConvertBatchNormalization, ConvertScaling, ConvertBias],
    "BinaryConvolution": [ConvertBinaryConvolution, ConvertBias, ConvertActivation],
    "Convolution": [ConvertConvolution, ConvertBias, ConvertActivation],
    "FullyConnected": [ConvertFullyConnected, ConvertBias, ConvertActivation],
    "ElementTimes": ConvertScaling,
    "LeakyReLU": ConvertLeakyReLU,
    "Input": ConvertInput,
    "MaxPooling": ConvertMaxPooling,
    "Minus": ConvertMinus,
    "Passthrough": ConvertPassthrough,
    "Plus": ConvertPlus,
    "Pooling": ConvertPooling,
    "PReLU": ConvertPReLU,
    "ReLU": ConvertReLU,
    "Softmax": ConvertSoftmax,
    "Splice": ConvertSplice,
    "Reshape": ConvertReshape,
    }

class ImporterModel:
    """
    Defines a class that holds the nodes and tensors to be imported.
    Nodes describe the operations, and tensors describe the weights 
    (and their ordering).
    """
    def __init__(self):
        self.nodes = {}
        self.tensors = {}

    def add_tensor(self, name: str, value: typing.Any, order: str) -> None:
        self.tensors[name] = (value, order)

    def add_node(self, name: str, node: ImporterNode) -> None:
        self.nodes[name] = node

class ImporterEngine:
    """
    The common class for doing an import to ELL. The ImporterEngine converts
    an ImporterModel to ELL nodes or (currently, layers).
    """
    def __init__(self, operation_map: typing.Mapping[str, typing.Sequence[typing.Any]] = operation_map):
        """
        Initializes the engine with an appropriate operation to converter mapping.
        See `operation_map` comments for more information.
        """
        self.ell_nodes = {}
        self.ell_model = None
        self.operation_map = operation_map
        self.lookup_table = None

    def get_supported_operation_types(self):
        """
        Returns the list of supported operation_type values for this
        importer engine.
        Font-end converters should ensure the ImporterModel they create
        contain ImportNodes one of these operation_types.
        """
        return self.operation_map.keys()

    def convert(self, model: ImporterModel):
        """
        Converts the model using the operation conversion map that was 
        specified when this class was initialized.
        """
        self.lookup_table = LookupTable(model.tensors)
        builder = ell.model.ModelBuilder()

        # Make a first pass through the model to set output padding. Since ELL
        # does pre-padding, a particular node must know what the input of the
        # next node wants in terms of padding.
        self.set_output_padding_for_nodes(model.nodes)

        ordered_nodes = self.get_nodes_in_import_order(model.nodes)
        _logger.info("Processing the following importer nodes in order:")
        for ordered_node in ordered_nodes:
            _logger.info(ordered_node)
        
        _logger.info("Converting intermediate importer nodes to ELL....")
        # For now, convert to ELL layers. Later, we will convert to ELL nodes.
        layers = []
        for node_to_import in ordered_nodes:
            converted = self.convert_importer_node_to_ell_layers(node_to_import)
            layers += converted
        _logger.info("Done.")
            
        return layers
    
    def convert_importer_node_to_ell_layers(self, node_to_import: ImporterNode):
        """
        Convert this importer node to an ELL neural network layer. This will
        soon be replaced by construction of one or more ELL layers.
        """
        converter_types = self.operation_map[node_to_import.operation_type]
        if not isinstance(converter_types, list):
            converter_types = [converter_types]

        # Remove converters that don't apply. This can happen when blocks
        # contain optional nodes
        converters = []
        for converter_type in converter_types:
            converter = converter_type(node_to_import)
            if converter.can_convert():
                converters.append(converter)    
                
        # Convert a block.
        layers = []
        for i in range(len(converters)):
            conversion_parameters = {"lookup_table": self.lookup_table,
                                     "first_in_block": i == 0,
                                     "last_in_block": i == (len(converters) - 1)}
            layer = converters[i].convert(conversion_parameters)
            if layer:
                layers.append(layer)
        
        return layers

    def get_nodes_in_import_order(self, nodes: typing.Mapping[str, typing.Any]):
        """
        Returns the nodes in an order that is suitable to import. That means
        each node is guaranteed to appear after the nodes it relies on.
        """
        pending_nodes = list(nodes.values())
        ordered_nodes = []

        node_processed = len(pending_nodes) > 0
        while node_processed:
            node_processed = False
            for current_node in pending_nodes:
                # Find a node which already has all of its input nodes in the
                # ordered list.
                if current_node.operation_type != "Skip":
                    if all(any(node_id in processed_node.outputs for processed_node in ordered_nodes) for node_id in current_node.inputs):
                        pending_nodes.remove(current_node)
                        ordered_nodes.append(current_node)
                        node_processed = True
        
        # Remove unreferenced inputs.
        result = []
        for current_node in ordered_nodes:
            if current_node.operation_type != "Input" or any(current_node.outputs[0] in node.inputs for node in ordered_nodes):
                result.append(current_node)

        return result

    def get_nodes_of_type(self, operation_type: str, nodes: typing.Mapping[str, ImporterNode]):
        """
        Returns nodes of matching importer operation_type.
        """
        matching_nodes = []
        for key in nodes.keys():
            if operation_type == nodes[key].operation_type:
                matching_nodes.append(nodes[key])
        return matching_nodes
        
    def find_nodes_with_input(self, node_id: str, nodes: typing.Mapping[str, ImporterNode]):
        """
        Returns first node containing node_id as an input.
        """
        nodes_with_input = []
        for key in nodes.keys():
            if node_id in nodes[key].inputs:
                nodes_with_input.append(nodes[key])
        return nodes_with_input

    def get_padding_for_node(self, node: ImporterNode, nodes: typing.Mapping[str, typing.Any]):
        """
        Returns padding for a node.
        """
        padding = None
        if node.outputs:
            # Find the node whose input is this node's output. A special case
            # exists if node is used to splice or concatenate output, since
            # the padding info needs to come from the next downstream node.
            next_nodes = self.find_nodes_with_input(node.outputs[0], nodes)
            if next_nodes:
                if next_nodes[0]:
                    if (next_nodes[0].operation_type == "Splice") or (next_nodes[0].operation_type == "Reorder"):
                        padding = self.get_padding_for_node(next_nodes[0], nodes)
                    else:
                        padding = next_nodes[0].padding
        return padding

    def set_output_padding_for_nodes(self, nodes: typing.Mapping[str, ImporterNode]):
        """
        Sets the output padding for all nodes. Output padding is the required
        input padding of the subsequent node(s).
        """
        for key in nodes.keys():
            # For now, just pick the first output node and use that as the 
            # padding.
            padding = self.get_padding_for_node(nodes[key], nodes)
            if padding:
                nodes[key].output_padding = padding




