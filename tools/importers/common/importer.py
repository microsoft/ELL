####################################################################################################
#
# Project:  Embedded Learning Library (ELL)
# File:     importer.py (importers)
# Authors:  Byron Changuion
#
# Requires: Python 3.5+
#
####################################################################################################

import os
import sys
import typing

sys.path.append(os.path.join(os.path.dirname(os.path.abspath(__file__))))

import ell
import logger
_logger = logger.get()
from converters import *


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
    "Abs": ConvertAbs,
    "Activation": ConvertActivation,
    "AveragePooling": ConvertAveragePooling,
    "BatchNormalization": [ConvertBatchNormalization, OptionalConvertScaling, OptionalConvertBias],
    "Bias": ConvertBias,
    "BinaryConvolution": [ConvertBinaryConvolution, OptionalConvertBias, OptionalConvertActivation],
    "Convolution": [ConvertConvolution, OptionalConvertBias, OptionalConvertActivation],
    "Constant": ConvertConstant,
    "Cos": ConvertCos,
    "ElementTimes": ConvertScaling,
    "ElementwiseMul": ConvertCoordinatewiseMultiply,
    "Exp": ConvertExp,
    "FullyConnected": [ConvertFullyConnected, OptionalConvertBias, OptionalConvertActivation],
    "GRU": ConvertGRU,
    "HardSigmoid": ConvertHardSigmoid,
    "Input": ConvertInput,
    "LeakyReLU": ConvertLeakyReLU,
    "Log": ConvertLog,
    "LSTM": ConvertLSTM,
    "MaxPooling": ConvertMaxPooling,
    "Minus": ConvertMinus,
    "Passthrough": ConvertPassthrough,
    "Plus": ConvertPlus,
    "Pooling": ConvertPooling,
    "PReLU": ConvertPReLU,
    "ReLU": ConvertReLU,
    "Region": ConvertRegion,
    "Reshape": ConvertReshape,
    "Softmax": ConvertSoftmax,
    "Sigmoid": ConvertSigmoid,
    "Sign": ConvertSign,
    "Sin": ConvertSin,
    "Splice": ConvertSplice,
    "Square": ConvertSquare,
    "Sqrt": ConvertSqrt,
    "Subtract": ConvertSubtract,
    "Tanh": ConvertTanh,
    "VAD": ConvertVAD,
    "Cast": ConvertTypeCast,
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
        t = (value, order)
        self.tensors[name] = t
        return t

    def add_node(self, name: str, node: ImporterNode) -> None:
        self.nodes[name] = node


class ImporterEngine:
    """
    The common class for doing an import to ELL. The ImporterEngine converts
    an ImporterModel to ELL nodes or (currently, layers).
    """
    def __init__(self, operation_map: typing.Mapping[str, typing.Sequence[typing.Any]] = operation_map,
                 step_interval_msec=None, lag_threshold_msec=None):
        """
        Initializes the engine with an appropriate operation to converter mapping.
        See `operation_map` comments for more information.
        """
        self.ell_nodes = {}
        self.ell_model = None
        self.ell_model_builder = None
        self.operation_map = operation_map
        self.lookup_table = None
        self.step_interval_msec = step_interval_msec
        self.lag_threshold_msec = lag_threshold_msec
        self.ordered_importer_nodes = []
        self.final_ell_map = None
        self.final_mapping = {}

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

        # Make a first pass through the model to set output padding. Since ELL
        # does pre-padding, a particular node must know what the input of the
        # next node wants in terms of padding.
        self.set_output_padding_for_nodes(model.nodes)

        ordered_nodes = self.get_nodes_in_import_order(model.nodes)

        # ordered_nodes = list(model.nodes.values()) # ADDED CODE | nodes are already topologically sorted
        _logger.info("Processing the following importer nodes in order:")
        for ordered_node in ordered_nodes:
            _logger.info(ordered_node)

        _logger.info("Converting intermediate importer nodes to ELL layers....")
        # For now, convert to ELL layers. Later, we will convert to ELL nodes.
        layers = []
        for node_to_import in ordered_nodes:
            converted = self.convert_importer_node_to_ell_layers(node_to_import)
            layers += converted
        _logger.info("Done.")

        return layers

    def get_node_group_mapping(self, model: ell.model.Model) -> typing.Mapping[str, typing.Sequence[ell.nodes.Node]]:
        """
        Returns the mapping of ImporterNode id to the group of ELL nodes that
        got converted from it.
        """
        group_id_mapping = {}
        nodes = model.GetNodes()

        # Collect the group id to ell node mapping
        while nodes.IsValid():
            node = nodes.Get()
            group_id = node.GetMetadataValue("GroupId")
            # Ensure that GroupId is set to something. If it isn't, this indicates
            # an ELL node that was inserted independently of the import, e.g.
            # a SinkNode
            if group_id:
                if group_id in group_id_mapping:
                    group_id_mapping[group_id].append(node)
                else:
                    group_id_mapping[group_id] = [node]
            nodes.Next()

        return group_id_mapping

    def convert_nodes(self, model: ImporterModel, apply_ordering: bool = True):
        """
        Converts the model using the operation conversion map that was
        specified when this class was initialized.
        If apply_ordering is True, the engine will re-order nodes starting with
        input nodes, and then repeatedly adding nodes whose inputs are in the
        newly updated list list.
        """
        self.ell_model = ell.model.Model()
        self.ell_model_builder = ell.model.ModelBuilder()
        self.lookup_table = LookupTable(model.tensors)
        function_prefix = ""

        # Make a first pass through the model to set output padding. Since ELL
        # does pre-padding, a particular node must know what the input of the
        # next node wants in terms of padding.
        self.set_output_padding_for_nodes(model.nodes)

        if apply_ordering:
            ordered_nodes = self.get_nodes_in_import_order(model.nodes)
        else:
            ordered_nodes = list(model.nodes.values())

        self.ordered_importer_nodes = ordered_nodes

        if len(ordered_nodes) == 0:
            raise Exception("Cannot convert model because it is empty!")

        _logger.info("Processing the following importer nodes in order:")
        for ordered_node in ordered_nodes:
            _logger.info(ordered_node)

        _logger.info("Converting intermediate importer nodes to ELL nodes....")
        for node_to_import in ordered_nodes:
            self.convert_importer_node_to_ell_nodes(node_to_import)
        _logger.info("Done.")

        # Quick workaround to create the map's output node. The last node in
        # the ordered nodes list will be used.
        last_importer_node = ordered_nodes[-1]
        shape_entry = last_importer_node.output_shapes[0]
        ell_output_shape = memory_shapes.get_ell_shape(shape_entry[0], shape_entry[1], 0)

        last_ell_node = self.lookup_table.get_ell_node_from_importer_node_id(last_importer_node.id)

        if self.step_interval_msec is not None:
            # Add the sink node
            last_ell_node = self.ell_model_builder.AddSinkNode(
                self.ell_model, ell.nodes.PortElements(last_ell_node.GetOutputPort("output")),
                ell.model.PortMemoryLayout(ell_output_shape),
                "{}OutputCallback".format(function_prefix))

        output_node = self.ell_model_builder.AddOutputNode(
            self.ell_model, ell.model.PortMemoryLayout(ell_output_shape), ell.nodes.PortElements(
                last_ell_node.GetOutputPort("output")))
        output_node.CopyMetadataFrom(last_ell_node)
        self.lookup_table.add_ell_output(output_node)

        # Create the map
        all_outputs = self.lookup_table.get_ell_outputs()
        output_list = [ell.nodes.PortElements(e.GetOutputPort("output")) for e in all_outputs]
        self.final_ell_map = ell.model.Map(self.ell_model, ell.nodes.InputNodeList(
            self.lookup_table.get_ell_inputs()),
            ell.nodes.PortElementsList(output_list))

        # Print out the nodes and where they came from
        self.final_mapping = self.get_node_group_mapping(self.final_ell_map.GetModel())
        _logger.info("\nFinal mapping of imported nodes to ell nodes:")
        source_names = {}
        max_len = 0
        for group_id in self.final_mapping.keys():
            importer_node = model.nodes[group_id]
            text = "{}({})".format(importer_node.operation_type, group_id)
            if len(text) > max_len:
                max_len = len(text)
            source_names[group_id] = text

        for group_id in self.final_mapping.keys():
            source_name = source_names[group_id]
            indent = "    " + (" " * (max_len - len(source_name)))
            ell_nodes = ["{}({})".format(ell_node.GetRuntimeTypeName(), ell_node.GetId())
                         for ell_node in self.final_mapping[group_id]]
            _logger.info("{}{} -> {}".format(indent, source_name, ", ".join(ell_nodes)))
        _logger.info("")
        return self.final_ell_map

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
                                     "last_in_block": i == (len(converters) - 1),
                                     }
            layer = converters[i].convert(conversion_parameters)
            if layer:
                layers.append(layer)

        return layers

    def convert_importer_node_to_ell_nodes(self, node_to_import: ImporterNode):
        """
        Convert this importer node to an ELL node and add it to the ELL model.
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
        _logger.debug("Importing node {}".format(node_to_import.id))

        # Convert a block.
        for i in range(len(converters)):
            conversion_parameters = {
                "lookup_table": self.lookup_table,
                "first_in_block": i == 0,
                "last_in_block": i == (len(converters) - 1),
                "builder": self.ell_model_builder,
                "model": self.ell_model,
                "step_interval_msec": self.step_interval_msec,
                "lag_threshold_msec": self.lag_threshold_msec,
            }
            converters[i].convert_node(conversion_parameters)

        return

    def get_nodes_in_import_order(self, nodes: typing.Mapping[str, typing.Any]):
        """
        Returns the nodes in an order that is suitable to import. That means
        each node is guaranteed to appear after the nodes it relies on.
        """
        pending_nodes = list(nodes.values())
        ordered_nodes = []
        node_processed = len(pending_nodes) > 0
        outputs_available = {}
        while node_processed:
            node_processed = False
            for current_node in pending_nodes:
                # Find a node which already has all of its input nodes in the ordered list.
                if current_node.operation_type != "Skip":
                    if all((input_id in outputs_available) for input_id in current_node.inputs):
                        pending_nodes.remove(current_node)
                        ordered_nodes.append(current_node)
                        node_processed = True
                        for o in current_node.outputs:
                            outputs_available[o] = True

        pending_nodes = [n for n in pending_nodes if n.operation_type != "Skip"]
        if len(pending_nodes) > 0:
            _logger.info("### ignoring the following nodes because their inputs are not satisfiable:")
            for node in pending_nodes:
                _logger.info("    {}({})".format(node.operation_type, node.id))

        result = []
        for current_node in ordered_nodes:
            if current_node.operation_type != "Input" or any(current_node.outputs[0] in node.inputs
                                                             for node in ordered_nodes):
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
                    if next_nodes[0].operation_type in ("Splice", "Reorder", "Passthrough"):
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

    def get_importer_node_to_ell_mapping(self) \
            -> (typing.Sequence[ImporterNode], typing.Mapping[str, typing.Sequence[ell.nodes.Node]]):
        """
        Returns a tuple containing:
            (ordered input nodes, mapping of input node id to ell nodes)
        after converting the model with convert_nodes().
        This is used to help test of the model.
        """
        return (self.ordered_importer_nodes, self.final_mapping)
