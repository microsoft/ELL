####################################################################################################
#
# Project:  Embedded Learning Library (ELL)
# File:     converters.py (importers)
# Authors:  Byron Changuion
#
# Requires: Python 3.x
#
####################################################################################################

import logging
from enum import Enum, auto
import typing

import numpy as np
import ell
import common.memory_shapes as memory_shapes

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
        output_shapes: typing.Sequence[typing.Any] = [],
        metadata: typing.Mapping[str, str] = {}):
        """
        id: unique identifier for this node
        operation_type: string name of the operation type to be imported.
            This will get mapped to an ELL operation via the operation_map.
        inputs: array of strings representing where the input comes from.
            The string is the 'id' of another ImporterNode.
        outputs: array of strings representing the output tensors.        
            The string is the 'id' of another ImporterNode.
        weights: dictionary of weight parameter labels to weight names e.g. a
            convolutional node may have {'weights': 'w123', 'bias': 'b832'}.
            Dictionary keys are specific to the ELL operation.
            The value is the id of a tensor in ImporterModel.tensors.
        attributes: dictionary of attribute names and values e.g. a
            convolutional node may have {'size': 3, 'step': 1, 'pad': 0 }.
            Dictionary keys are specific to the ELL operation.
        padding: dictionary of padding size and padding scheme e.g.
                {"size": 0, "scheme": ell.neural.PaddingScheme.zeros}
                [chris] why isn't this just a type of attribute?
        input_shapes: array of tuples representing input shapes and ordering
                e.g. ((3,64,64), "channel_row_column").
            The ImporterEngine will take care of reshaping everything to 
            match the order required by ELL.
        output_shapes: array of tuples representing output shapes and ordering
                e.g. ((32,8,8), "channel_row_column").
        metadata: optional additional metadata to store in the ell_nodes.
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
        self.metadata = metadata

    def __repr__(self):
        _print_line = ""
        _print_line += "{} {}: {} -> {}, attributes {}\n".format(self.operation_type, self.id, self.inputs, self.outputs, self.attributes)
        _print_line += "    input_shape {}\n".format(self.input_shapes)
        _print_line += "    output_shape {}\n".format(self.output_shapes)
        _print_line += "    padding {}\n".format(self.padding)
        _print_line += "    output_padding {}\n".format(self.output_padding)
        _print_line += "    weights {}\n".format(self.weights.keys())
        return _print_line

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
        # Stores mapping of ELL Node id string to ELL Node
        self.ell_id_to_ell_nodes = {}
        # Stores mapping of importer node id string to ELL Node id
        self.importer_id_to_ell_ids = {}
        # Stores mapping of ell node id string to Importer Node
        self.ell_id_to_owning_importer_node = {}
        # Stores mapping of output id string to owning ELL Node id
        self.output_id_to_ell_ids = {}
        # Stores mapping of tensor ids to numpy tensor instance
        self.tensors = tensors
        # Stores input nodes. When creating an ELL map from an ELL model,
        # map inputs must be identified.
        self.input_ell_nodes = []
        # Stores output nodes When creating an ELL map from an ELL model,
        # map inputs must be identified.
        self.output_ell_nodes = []

    def add_imported_ell_node(self, importer_node: ImporterNode, ell_node: ell.nodes.Node, set_group_id=True):
        """
        Adds an ImporterNode and associated ELL node to the lookup.
        """
        # Add to mapping of ELL Node id to ELL Node
        ell_node_id = ell_node.GetId()
        self.add_ell_node(ell_node)
        # Add ImporterNode id to ELL Node id mapping
        if importer_node.id in self.importer_id_to_ell_ids:
            self.importer_id_to_ell_ids[importer_node.id].append(ell_node_id)
        else:
            self.importer_id_to_ell_ids[importer_node.id] = [ell_node_id]
        _logger.debug("ImporterNode {} -> intermediate ELL nodes {}".format(importer_node.id, self.importer_id_to_ell_ids[importer_node.id]))
        # Add output id to owner mapping.
        for output_id in importer_node.outputs:
            self.set_owning_node_for_output(output_id, ell_node)
        if set_group_id:
            # Set the node's metadata to show where this node came from
            ell_node.SetMetadataValue("GroupId", importer_node.id)
        
        # concatenate any importer_node metadata provided by importer 
        if importer_node.metadata != None:
            for key in importer_node.metadata:
                value = importer_node.metadata[key]
                ell_node.SetMetadataValue(key, value)


        # Add owning id mapping
        self.ell_id_to_owning_importer_node[ell_node_id] = importer_node

    def add_ell_node(self, ell_node: ell.nodes.Node):
        """
        Adds an ELL node to the lookup.
        """
        ell_node_id = ell_node.GetId()
        self.ell_id_to_ell_nodes[ell_node_id] = ell_node

    def get_ell_node_from_id(self, node_id: str):
        return self.ell_id_to_ell_nodes[node_id]

    def get_ell_id(self, importer_node_id: str):
        """
        Return the id of the last ELL node associated with this importer node.
        """
        id = None
        if importer_node_id in self.importer_id_to_ell_ids:
            id = self.importer_id_to_ell_ids[importer_node_id][-1]
        return id

    def get_ell_node_from_importer_node_id(self, importer_node_id: str):
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
        if not uid in self.tensors:
            raise Exception("Required tensor {} not found".format(uid))
        original_tensor, order = self.tensors[uid]

        return memory_shapes.get_tensor_in_ell_order(original_tensor, order)
        # original_shape = original_tensor.shape
        # if order == "filter_channel_row_column":
        #     ordered_weights = np.moveaxis(original_tensor, 1, -1)
        #     ordered_weights = ordered_weights.ravel().astype(np.float).reshape(
        #         original_shape[0] * original_shape[2], original_shape[3], original_shape[1])
        # elif order == "row_column":
        #     ordered_weights = np.moveaxis(original_tensor, 0, -1)
        #     ordered_weights = ordered_weights.ravel().astype(
        #         np.float).reshape(original_shape[1], original_shape[0], 1)
        # elif order == "channel":
        #     ordered_weights = original_tensor.ravel().astype(
        #         np.float).reshape(1, 1, original_shape.size)
        # elif order == "channel_row_column_filter":
        #     ordered_weights = np.moveaxis(original_tensor, 0, -1)
        #     ordered_weights = np.moveaxis(ordered_weights, 2, 0)
        #     ordered_weights = ordered_weights.ravel().astype(np.float).reshape(
        #     original_shape[3] * original_shape[1], original_shape[2], original_shape[0])
        # else:
        #     raise NotImplementedError(
        #         "Unsupported tensor order {}, for {}".format(order, uid))

        # return ordered_weights

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

    def get_port_elements_for_input(self, importer_node: ImporterNode, input_index = 0) -> ell.nodes.PortElements:
        """
        Returns an ell.nodes.PortElements for the corresponding ImporterNode.
        """
        try:
            # First check whether this importer node has any corresponding
            # ELL nodes yet:
            # - If it does, grab the output of the last ELL node which
            #   is designated as the input to this node.
            # - If it doesn't, grab the output of the last ELL node which
            #   the Importer's input is tied to.
            owning_node = self.get_ell_node_from_importer_node_id(importer_node.id)
            if owning_node is None:
                owning_node_id = self.output_id_to_ell_ids[importer_node.inputs[input_index]]
                owning_node = self.ell_id_to_ell_nodes[owning_node_id]
        except BaseException as exception:
            raise Exception("Cannot get input port elements for {}, missing ELL owning node".format(importer_node.id))
        return self.get_output_port_elements_for_node(owning_node)

    def get_port_elements_and_memory_layout_for_input(self, importer_node: ImporterNode, input_index = 0) -> (ell.nodes.PortElements, ell.model.PortMemoryLayout):
        """
        Returns an (ell.nodes.PortElements, ell.nodes.PortMemoryLayout) for the corresponding input of the ImporterNode.
        """
        try:
            owning_ell_node = self.get_owning_node_for_output(importer_node.inputs[input_index])
            owning_importer_node = self.ell_id_to_owning_importer_node[owning_ell_node.GetId()]
            padding = owning_importer_node.output_padding["size"]
            output_shape = owning_importer_node.output_shapes[0]
            port_elements = self.get_port_elements_for_input(importer_node, input_index)
            port_memory_layout = memory_shapes.get_ell_port_memory_layout(output_shape[0], output_shape[1], padding)
        except BaseException as exception:
            raise Exception("Could not get PortMemoryElements or PortMemoryLayout for importer node {}, input {}".format(importer_node.id, input_index))
        return (port_elements, port_memory_layout)

    def get_output_port_elements_for_node(self, ell_node: ell.nodes.Node,
        output_label: str = "output"):
        """
        Returns an ell.nodes.PortElements for the corresponding ELL node's
        output port that corresponds to 'output_label'.
        """
        try:
            output_link = ell_node.GetOutputPort(output_label)
        except BaseException as exception:
            raise Exception("Cannot get output port {} for {}".format(output_label, ell_node.GetId()))
        return ell.nodes.PortElements(output_link)

    def get_owning_node_for_output(self, output_id: str) -> ell.nodes.Node:
        """
        Gets the ELL node that owns the output identified by output_id.
        """
        try:
            ell_node_id = self.output_id_to_ell_ids[output_id]
            ell_node = self.ell_id_to_ell_nodes[ell_node_id]
        except BaseException as exception:
            raise Exception("Cannot find owning ELL node for output {}".format(output_id))
        return ell_node

    def get_originating_importer_node_for_output(self, output_id: str) -> ImporterNode:
        """
        Gets the originating ImporterNode for the output identified by output_id.
        """
        try:
            ell_node_id = self.output_id_to_ell_ids[output_id]
            importer_node = self.ell_id_to_owning_importer_node[ell_node_id]
        except BaseException as exception:
            raise Exception("Cannot find originating ImporterNode node for output {}".format(output_id))
        return importer_node

    def set_owning_node_for_output(self, output_id: str, ell_node: ell.nodes.Node):
        """
        Sets the mapping for the ELL node that owns the output identified
        by output_id.
        """
        self.output_id_to_ell_ids[output_id] = ell_node.GetId()

    def add_ell_input(self, ell_node: ell.nodes.Node):
        self.input_ell_nodes = [ell_node] + self.input_ell_nodes

    def get_ell_inputs(self):
        return self.input_ell_nodes

    def add_ell_output(self, ell_node: ell.nodes.Node):
        self.output_ell_nodes = [ell_node] + self.output_ell_nodes

    def get_ell_outputs(self):
        return self.output_ell_nodes

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
        self.optional = False

    def can_convert(self) -> bool:
        """
        Verify that the node contains the necessary inputs, weights and
        attributes to convert. Nodes that cannot be converted due to
        missing weights or attributes are deemed optional and are skipped.
        See comments in operation_map for examples.
        """
        for w in self.required_weights:
            if w not in self.importer_node.weights:
                if not self.optional:
                    raise Exception("Missing required weight '{}' on node {}_{}".format(w, self.importer_node.operation_type, self.importer_node.id))
                return False
        for attr in self.required_attributes:
            if attr not in self.importer_node.attributes:
                if not self.optional:
                    raise Exception("Missing required attribute {} on node {}_{}".format(attr, self.importer_node.operation_type, self.importer_node.id))
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
        
        return ell.neural.LayerParameters(input_shape, input_padding, output_shape, output_padding, ell.nodes.PortType.smallReal)

    def get_ell_shape(self, shape: tuple, order: str, padding: int = 0):
        """
        Return the shape in ELL canonical order
        """
        return memory_shapes.get_ell_shape(shape, order, padding)

    def get_ell_tensor(self, uid: str, conversion_parameters: typing.Mapping[str, typing.Any]):
        """
        Returns a weight tensor as an ELL tensor
        """
        lookup_table = conversion_parameters["lookup_table"]
        return ell.math.DoubleTensor(lookup_table.get_tensor_in_ell_order(uid))

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
        return ell.math.DoubleVector(self.get_vector(uid, conversion_parameters))

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
        alpha = 0.01
        if "alpha" in self.importer_node.attributes:
            alpha = self.importer_node.attributes["alpha"]
        if (activation == ell.neural.ActivationType.leaky):
            return ell.neural.LeakyReLUActivationLayer(layer_parameters, alpha)
        else:
            return ell.neural.ActivationLayer(layer_parameters, activation)

    def convert_node(self, conversion_parameters: typing.Mapping[str, typing.Any]):
        """
        Derived classes override to convert the importer node to appropriate ELL node(s)
        and insert into the model
        """
        model = conversion_parameters["model"]
        builder = conversion_parameters["builder"]
        lookup_table = conversion_parameters["lookup_table"]
        # Create the activation layer
        activation_layer = self.convert(conversion_parameters)
        # Get the port elements from the input
        input_port_elements = lookup_table.get_port_elements_for_input(self.importer_node)
        # Add the ActivationLayerNode to the model
        ell_node = builder.AddActivationLayerNode(model, input_port_elements, activation_layer)
        # Register the mapping
        lookup_table.add_imported_ell_node(self.importer_node, ell_node)

class OptionalConvertActivation(ConvertActivation):
    """
    Optional converter for Activation
    """
    def __init__(self, node: ImporterNode):
        super().__init__(node)
        self.optional = True

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
        return ell.neural.PoolingLayer(layer_parameters,
            pooling_parameters, ell.neural.PoolingType.mean)

    def convert_node(self, conversion_parameters: typing.Mapping[str, typing.Any]):
        """
        Derived classes override to convert the importer node to appropriate ELL node(s)
        and insert into the model
        """
        model = conversion_parameters["model"]
        builder = conversion_parameters["builder"]
        lookup_table = conversion_parameters["lookup_table"]
        # Create the pooling layer
        pooling_layer = self.convert(conversion_parameters)
        # Get the port elements from the input
        input_port_elements = lookup_table.get_port_elements_for_input(self.importer_node)
        # Add the PoolingLayerNode to the model
        ell_node = builder.AddPoolingLayerNode(model, input_port_elements, pooling_layer)
        # Register the mapping
        lookup_table.add_imported_ell_node(self.importer_node, ell_node)


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

        return ell.neural.BatchNormalizationLayer(layer_parameters,
            mean_vector, variance_vector, self.epsilon,
            ell.neural.EpsilonSummand.variance)

    def convert_node(self, conversion_parameters: typing.Mapping[str, typing.Any]):
        """
        Derived classes override to convert the importer node to appropriate ELL node(s)
        and insert into the model
        """
        model = conversion_parameters["model"]
        builder = conversion_parameters["builder"]
        lookup_table = conversion_parameters["lookup_table"]
        # Create the batch normalization layer
        batch_normalization_layer = self.convert(conversion_parameters)
        # Get the port elements from the input
        input_port_elements = lookup_table.get_port_elements_for_input(self.importer_node)
        # Add the BatchNormalizationLayerNode to the model
        ell_node = builder.AddBatchNormalizationLayerNode(model, input_port_elements, batch_normalization_layer)
        # Register the mapping
        lookup_table.add_imported_ell_node(self.importer_node, ell_node)

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

        return ell.neural.BiasLayer(layer_parameters, bias)

    def convert_node(self, conversion_parameters: typing.Mapping[str, typing.Any]):
        """
        Derived classes override to convert the importer node to appropriate ELL node(s)
        and insert into the model
        """
        model = conversion_parameters["model"]
        builder = conversion_parameters["builder"]
        lookup_table = conversion_parameters["lookup_table"]
        # Create the bias layer
        bias_layer = self.convert(conversion_parameters)
        # Get the port elements from the input
        input_port_elements = lookup_table.get_port_elements_for_input(self.importer_node)
        # Add the BiasLayerNode to the model
        ell_node = builder.AddBiasLayerNode(model, input_port_elements, bias_layer)
        # Register the mapping
        lookup_table.add_imported_ell_node(self.importer_node, ell_node)

class OptionalConvertBias(ConvertBias):
    """
    Optional converter for Bias
    """
    def __init__(self, node: ImporterNode):
        super().__init__(node)
        self.optional = True

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
        layer_parameters = self.get_layer_parameters(conversion_parameters)
        layer_parameters.inputPaddingParameters

        weights = self.get_ell_tensor(
            self.importer_node.weights["weights"][0], conversion_parameters)

        attributes = self.importer_node.attributes
        convolutional_parameters = ell.neural.BinaryConvolutionalParameters(
            attributes["size"], attributes["stride"], ell.neural.BinaryConvolutionMethod.bitwise,
            ell.neural.BinaryWeightsScale.none)

        return ell.neural.BinaryConvolutionalLayer(layer_parameters, convolutional_parameters, weights)

    def convert_node(self, conversion_parameters: typing.Mapping[str, typing.Any]):
        """
        Derived classes override to convert the importer node to appropriate ELL node(s)
        and insert into the model
        """
        model = conversion_parameters["model"]
        builder = conversion_parameters["builder"]
        lookup_table = conversion_parameters["lookup_table"]
        # Create the convolutional layer
        convolutional_layer = self.convert(conversion_parameters)
        # Get the port elements from the input
        input_port_elements = lookup_table.get_port_elements_for_input(self.importer_node)

        # If we require padding but the input doesn't provide it
        # (which can happen when a single node output is used as input to
        # multiple nodes), ensure correct padding with a ReorderDataNode.
        owning_node_for_input = lookup_table.get_originating_importer_node_for_output(self.importer_node.inputs[0])
        padding = self.importer_node.padding["size"]
        if (owning_node_for_input.output_padding["size"] != padding):
            input_node = lookup_table.get_ell_node_from_importer_node_id(owning_node_for_input.id)
            port_elements = lookup_table.get_output_port_elements_for_node(input_node)
            shape_entry = owning_node_for_input.output_shapes[0]
            input_memory_layout = memory_shapes.get_ell_port_memory_layout(shape_entry[0], shape_entry[1], 0)
            output_memory_layout = memory_shapes.get_ell_port_memory_layout(shape_entry[0], shape_entry[1], padding)
            # Create the reorder node
            reorder_node = builder.AddReorderDataNode(model, port_elements, input_memory_layout, output_memory_layout, [0, 1, 2])
            # Register the mapping
            lookup_table.add_imported_ell_node(self.importer_node, reorder_node)
            input_port_elements = lookup_table.get_output_port_elements_for_node(reorder_node)

        # Add the ConvolutionalLayerNode to the model
        ell_node = builder.AddBinaryConvolutionalLayerNode(model, input_port_elements, convolutional_layer)
        # Register the mapping
        lookup_table.add_imported_ell_node(self.importer_node, ell_node)


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

        return ell.neural.ConvolutionalLayer(layer_parameters,
            convolutional_parameters, weights)

    def convert_node(self, conversion_parameters: typing.Mapping[str, typing.Any]):
        """
        Derived classes override to convert the importer node to appropriate ELL node(s)
        and insert into the model
        """
        model = conversion_parameters["model"]
        builder = conversion_parameters["builder"]
        lookup_table = conversion_parameters["lookup_table"]
        # Create the convolutional layer
        convolutional_layer = self.convert(conversion_parameters)
        # Get the port elements from the input
        input_port_elements = lookup_table.get_port_elements_for_input(self.importer_node)

        # If we require padding but the input doesn't provide it
        # (which can happen when a single node output is used as input to
        # multiple nodes), ensure correct padding with a ReorderDataNode.
        owning_node_for_input = lookup_table.get_originating_importer_node_for_output(self.importer_node.inputs[0])
        padding = self.importer_node.padding["size"]
        if (owning_node_for_input.output_padding["size"] != padding):
            input_node = lookup_table.get_ell_node_from_importer_node_id(owning_node_for_input.id)
            port_elements = lookup_table.get_output_port_elements_for_node(input_node)
            shape_entry = owning_node_for_input.output_shapes[0]
            input_memory_layout = memory_shapes.get_ell_port_memory_layout(shape_entry[0], shape_entry[1], 0)
            output_memory_layout = memory_shapes.get_ell_port_memory_layout(shape_entry[0], shape_entry[1], padding)
            # Create the reorder node
            reorder_node = builder.AddReorderDataNode(model, port_elements, input_memory_layout, output_memory_layout, [0, 1, 2])
            # Register the mapping
            lookup_table.add_imported_ell_node(self.importer_node, reorder_node)
            input_port_elements = lookup_table.get_output_port_elements_for_node(reorder_node)

        # Add the ConvolutionalLayerNode to the model
        ell_node = builder.AddConvolutionalLayerNode(model, input_port_elements, convolutional_layer)
        # Register the mapping
        lookup_table.add_imported_ell_node(self.importer_node, ell_node)


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

        return ell.neural.FullyConnectedLayer(
            layer_parameters, weights)

    def convert_node(self, conversion_parameters: typing.Mapping[str, typing.Any]):
        """
        Derived classes override to convert the importer node to appropriate ELL node(s)
        and insert into the model
        """
        model = conversion_parameters["model"]
        builder = conversion_parameters["builder"]
        lookup_table = conversion_parameters["lookup_table"]
        # Create the fully connected layer
        fully_connected_layer = self.convert(conversion_parameters)
        # Get the port elements from the input
        input_port_elements = lookup_table.get_port_elements_for_input(self.importer_node)
        # Add the FullyConnectedLayerNode to the model
        ell_node = builder.AddFullyConnectedLayerNode(model, input_port_elements, fully_connected_layer)
        # Register the mapping
        lookup_table.add_imported_ell_node(self.importer_node, ell_node)


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
        Return the appropriate ELL node
        """
        layer_parameters = self.get_layer_parameters(conversion_parameters)
        scale = self.get_ell_vector(
            self.importer_node.weights["scale"][0], conversion_parameters)

        return ell.neural.ScalingLayer(layer_parameters, scale)

    def convert_node(self, conversion_parameters: typing.Mapping[str, typing.Any]):
        """
        Derived classes override to convert the importer node to appropriate ELL node(s)
        and insert into the model
        """
        model = conversion_parameters["model"]
        builder = conversion_parameters["builder"]
        lookup_table = conversion_parameters["lookup_table"]
        # Create the scaling layer
        scaling_layer = self.convert(conversion_parameters)
        # Get the port elements from the input
        input_port_elements = lookup_table.get_port_elements_for_input(self.importer_node)
        # Add the ScalingLayerNode to the model
        ell_node = builder.AddScalingLayerNode(model, input_port_elements, scaling_layer)
        # Register the mapping
        lookup_table.add_imported_ell_node(self.importer_node, ell_node)


class ConvertGRU(ConvertBase):
    """
    Converter for Gated Recurrent Unit (GRU). If the GRU node has 2 inputs,
    the second input is used as the trigger, otherwise a constant node is inserted as the
    trigger.
    """
    def __init__(self, node: ImporterNode):
        super().__init__(node)
        self.required_weights = ["input_weights", "hidden_weights", "input_bias", "hidden_bias" ]
        self.required_attributes = ["hidden_size", "activation", "recurrent_activation"]

    def convert(self, conversion_parameters: typing.Mapping[str, typing.Any]):
        """
        Return the appropriate ELL layer
        """
        raise Exception("No corresponding ELL layer for GRU. Use node instead.")
        

    def convert_node(self, conversion_parameters: typing.Mapping[str, typing.Any]):
        """
        Derived classes override to convert the importer node to appropriate ELL node(s)
        and insert into the model
        """
        model = conversion_parameters["model"]
        builder = conversion_parameters["builder"]
        lookup_table = conversion_parameters["lookup_table"]

        # Get the port elements from the input
        input_port_elements = lookup_table.get_port_elements_for_input(self.importer_node)
        layer_parameters = self.get_layer_parameters(conversion_parameters)

        # create constant nodes for the weights
        input_weights = self.get_ell_tensor(
            self.importer_node.weights["input_weights"][0], conversion_parameters)
        hidden_weights = self.get_ell_tensor(
            self.importer_node.weights["hidden_weights"][0], conversion_parameters)
        input_bias = self.get_ell_tensor(
            self.importer_node.weights["input_bias"][0], conversion_parameters)
        hidden_bias = self.get_ell_tensor(
            self.importer_node.weights["hidden_bias"][0], conversion_parameters)

        input_weights_node = builder.AddConstantNode(model, input_weights.data, ell.nodes.PortType.smallReal)
        hidden_weights_node = builder.AddConstantNode(model, hidden_weights.data, ell.nodes.PortType.smallReal)
        input_bias_node = builder.AddConstantNode(model, input_bias.data, ell.nodes.PortType.smallReal)
        hidden_bias = builder.AddConstantNode(model, hidden_bias.data, ell.nodes.PortType.smallReal)

        hidden_size = self.importer_node.attributes["hidden_size"]
        activation = self.importer_node.attributes["activation"]
        recurrentActivation = self.importer_node.attributes["recurrent_activation"]

        # Get the port elements for the reset trigger
        if len(self.importer_node.inputs) > 1:
            reset_port_elements, reset_memory_layout = lookup_table.get_port_elements_and_memory_layout_for_input(self.importer_node, 1)
        else:
            # Create a constant node as the trigger. The trigger fires on value change,
            # so will never fire in this case.
            reset_node = builder.AddConstantNode(model, [0], ell.nodes.PortType.integer)
            reset_port_elements = ell.nodes.PortElements(reset_node.GetOutputPort("output"))

        # Add the GRUNode to the model
        ell_node = builder.AddGRUNode(model, input_port_elements, reset_port_elements, hidden_size,
            ell.nodes.PortElements(input_weights_node.GetOutputPort("output")), 
            ell.nodes.PortElements(hidden_weights_node.GetOutputPort("output")), 
            ell.nodes.PortElements(input_bias_node.GetOutputPort("output")), 
            ell.nodes.PortElements(hidden_bias.GetOutputPort("output")), 
            activation, recurrentActivation)

        # Register the mapping
        lookup_table.add_imported_ell_node(self.importer_node, ell_node)


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

    def convert_node(self, conversion_parameters: typing.Mapping[str, typing.Any]):
        """
        Derived classes override to return the appropriate ELL node
        """
        model = conversion_parameters["model"]
        builder = conversion_parameters["builder"]
        lookup_table = conversion_parameters["lookup_table"]
        step_interval_msec = conversion_parameters["step_interval_msec"]
        lag_threshold_msec = conversion_parameters["lag_threshold_msec"]
        function_prefix = ""
        # Add the InputNode to the model
        shape_entry = self.importer_node.output_shapes[0]
        ell_shape = self.get_ell_shape(shape_entry[0], shape_entry[1], 0)
        original_input_node = None
        if step_interval_msec is not None:
            # in the steppable case the input is a clock ticks (which is a double)
            input_node = builder.AddInputNode(
                model, ell.math.TensorShape(1, 1, 1), ell.nodes.PortType.real)

            if lag_threshold_msec is None:
                lag_threshold_msec = 2 * step_interval_msec

            clock_node = builder.AddClockNode(
                model, ell.nodes.PortElements(input_node.GetOutputPort("output")),
                float(step_interval_msec), float(lag_threshold_msec),
                "{}LagNotification".format(function_prefix))
            source_node = builder.AddSourceNode(
                model, ell.nodes.PortElements(clock_node.GetOutputPort("output")),
                ell.nodes.PortType.smallReal, ell_shape,
                "{}InputCallback".format(function_prefix))
            
            original_input_node = input_node
            input_node = source_node
        else:
            input_node = builder.AddInputNode(
                model, ell_shape, ell.nodes.PortType.smallReal)
            original_input_node = input_node

        # Register the mapping
        lookup_table.add_imported_ell_node(self.importer_node, input_node)
        
        if step_interval_msec is not None:
            lookup_table.add_imported_ell_node(self.importer_node, clock_node)
            lookup_table.add_imported_ell_node(self.importer_node, source_node)

        # Special case: If output requires padding e.g. Input is connected to a
        # Convolutional node that requires padding, add a ReorderData node to
        # ensure proper memory layout. This can be skipped once Input supports
        # different memory layouts of the output.
        padding = self.importer_node.output_padding["size"]
        if padding > 0:
            # Create the reorder node
            port_elements = lookup_table.get_output_port_elements_for_node(input_node)
            input_memory_layout = memory_shapes.get_ell_port_memory_layout(shape_entry[0], shape_entry[1], 0)
            output_memory_layout = memory_shapes.get_ell_port_memory_layout(shape_entry[0], shape_entry[1], padding)
            reorder_node = builder.AddReorderDataNode(model, port_elements, input_memory_layout, output_memory_layout, [0, 1, 2])
            # Register the mapping
            lookup_table.add_imported_ell_node(self.importer_node, reorder_node)

        lookup_table.add_ell_input(original_input_node)


class ConvertLeakyReLU(ConvertActivation):
    """
    Converter for LeakyReLU, which is equivalent to
    Activation
    """
    def __init__(self, node: ImporterNode):
        super().__init__(node)
        self.required_weights = []
        self.required_attributes = []
        self.importer_node.attributes["activation"] = ell.neural.ActivationType.leaky


class ConvertLSTM(ConvertBase):
    """
    Converter for Long Short-Term Memory (LSTM) unit. If the LSTM node has 2 inputs,
    the second input is used as the trigger, otherwise a constant node is inserted as the
    trigger.
    """
    def __init__(self, node: ImporterNode):
        super().__init__(node)
        self.required_weights = ["input_weights", "hidden_weights", "input_bias", "hidden_bias" ]
        self.required_attributes = ["hidden_size", "activation", "recurrent_activation"]

    def convert(self, conversion_parameters: typing.Mapping[str, typing.Any]):
        """
        Return the appropriate ELL layer
        """
        raise Exception("No corresponding ELL layer for GRU. Use node instead.")
        

    def convert_node(self, conversion_parameters: typing.Mapping[str, typing.Any]):
        """
        Derived classes override to convert the importer node to appropriate ELL node(s)
        and insert into the model
        """
        model = conversion_parameters["model"]
        builder = conversion_parameters["builder"]
        lookup_table = conversion_parameters["lookup_table"]

        # Get the port elements from the input
        input_port_elements = lookup_table.get_port_elements_for_input(self.importer_node)
        layer_parameters = self.get_layer_parameters(conversion_parameters)

        # create constant nodes for the weights
        input_weights = self.get_ell_tensor(
            self.importer_node.weights["input_weights"][0], conversion_parameters)
        hidden_weights = self.get_ell_tensor(
            self.importer_node.weights["hidden_weights"][0], conversion_parameters)
        input_bias = self.get_ell_tensor(
            self.importer_node.weights["input_bias"][0], conversion_parameters)
        hidden_bias = self.get_ell_tensor(
            self.importer_node.weights["hidden_bias"][0], conversion_parameters)

        input_weights_node = builder.AddConstantNode(model, input_weights.data, ell.nodes.PortType.smallReal)
        hidden_weights_node = builder.AddConstantNode(model, hidden_weights.data, ell.nodes.PortType.smallReal)
        input_bias_node = builder.AddConstantNode(model, input_bias.data, ell.nodes.PortType.smallReal)
        hidden_bias = builder.AddConstantNode(model, hidden_bias.data, ell.nodes.PortType.smallReal)

        hidden_size = self.importer_node.attributes["hidden_size"]
        activation = self.importer_node.attributes["activation"]
        recurrentActivation = self.importer_node.attributes["recurrent_activation"]

        # Get the port elements for the reset trigger
        if len(self.importer_node.inputs) > 1:
            reset_port_elements, reset_memory_layout = lookup_table.get_port_elements_and_memory_layout_for_input(self.importer_node, 1)
        else:
            # Create a constant node as the trigger. The trigger fires on value change,
            # so will never fire in this case.
            reset_node = builder.AddConstantNode(model, [0], ell.nodes.PortType.integer)
            reset_port_elements = ell.nodes.PortElements(reset_node.GetOutputPort("output"))

        # Add the GRUNode to the model
        ell_node = builder.AddLSTMNode(model, input_port_elements, reset_port_elements, hidden_size,
            ell.nodes.PortElements(input_weights_node.GetOutputPort("output")), 
            ell.nodes.PortElements(hidden_weights_node.GetOutputPort("output")), 
            ell.nodes.PortElements(input_bias_node.GetOutputPort("output")), 
            ell.nodes.PortElements(hidden_bias.GetOutputPort("output")), 
            activation, recurrentActivation)

        # Register the mapping
        lookup_table.add_imported_ell_node(self.importer_node, ell_node)


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
        return ell.neural.PoolingLayer(layer_parameters,
            pooling_parameters, ell.neural.PoolingType.max)

    def convert_node(self, conversion_parameters: typing.Mapping[str, typing.Any]):
        """
        Derived classes override to convert the importer node to appropriate ELL node(s)
        and insert into the model
        """
        model = conversion_parameters["model"]
        builder = conversion_parameters["builder"]
        lookup_table = conversion_parameters["lookup_table"]
        # Create the pooling layer
        pooling_layer = self.convert(conversion_parameters)
        # Get the port elements from the input
        input_port_elements = lookup_table.get_port_elements_for_input(self.importer_node)
        # Add the PoolingLayerNode to the model
        ell_node = builder.AddPoolingLayerNode(model, input_port_elements, pooling_layer)
        # Register the mapping
        lookup_table.add_imported_ell_node(self.importer_node, ell_node)


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

        return ell.neural.BiasLayer(layer_parameters, bias)

    def convert_node(self, conversion_parameters: typing.Mapping[str, typing.Any]):
        """
        Derived classes override to convert the importer node to appropriate ELL node(s)
        and insert into the model
        """
        model = conversion_parameters["model"]
        builder = conversion_parameters["builder"]
        lookup_table = conversion_parameters["lookup_table"]
        # Create the bias layer
        bias_layer = self.convert(conversion_parameters)
        # Get the port elements from the input
        input_port_elements = lookup_table.get_port_elements_for_input(self.importer_node)
        # Add the BiasLayerNode to the model
        ell_node = builder.AddBiasLayerNode(model, input_port_elements, bias_layer)
        # Register the mapping
        lookup_table.add_imported_ell_node(self.importer_node, ell_node)


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

    def convert_node(self, conversion_parameters: typing.Mapping[str, typing.Any]):
        """
        Derived classes override to convert the importer node to appropriate ELL node(s)
        and insert into the model
        """
        lookup_table = conversion_parameters["lookup_table"]
        # Set owner of this output to be the Passthrough node's input node
        if len(self.importer_node.inputs) == 0:
            raise Exception("### Passthrough node {}({}) has no inputs".format(self.importer_node.operation_type, self.importer_node.id))
        input_owner = lookup_table.get_owning_node_for_output(self.importer_node.inputs[0])
        lookup_table.add_imported_ell_node(self.importer_node, input_owner, set_group_id=False)


class ConvertPlus(ConvertBase):
    """
    Converter for Plus
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

    def convert_node(self, conversion_parameters: typing.Mapping[str, typing.Any]):
        """
        Derived classes override to convert the importer node to appropriate ELL node(s)
        and insert into the model
        """
        model = conversion_parameters["model"]
        builder = conversion_parameters["builder"]
        lookup_table = conversion_parameters["lookup_table"]
        # Get the port elements and memory layout from the two inputs.
        # Since the 2 inputs and output could have different padding,
        # we need both the port elements and the memory layouts for each.
        input1_port_elements, input1_port_memory_layout = lookup_table.get_port_elements_and_memory_layout_for_input(
            self.importer_node, 0)
        input2_port_elements, input2_port_memory_layout = lookup_table.get_port_elements_and_memory_layout_for_input(
            self.importer_node, 1)
        output_shape_tuple = self.importer_node.output_shapes[0]
        output_port_memory_layout = memory_shapes.get_ell_port_memory_layout(
            output_shape_tuple[0],
            output_shape_tuple[1],
            self.importer_node.output_padding["size"])
        # Add the BinaryOperationNode to the model.
        ell_node = builder.AddBinaryOperationNodeWithMemoryLayout(
            model,
            input1_port_elements, input1_port_memory_layout,
            input2_port_elements, input2_port_memory_layout,
            output_port_memory_layout,
            ell.nodes.BinaryOperationType.add)
        # Register the mapping
        lookup_table.add_imported_ell_node(self.importer_node, ell_node)


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
        self.required_weights = ["alpha"]
        self.required_attributes = []

    def convert(self, conversion_parameters: typing.Mapping[str, typing.Any]):
        """
        Return the appropriate ELL node
        """
        layer_parameters = self.get_layer_parameters(conversion_parameters)
        alpha = self.get_ell_tensor(
            self.importer_node.weights["alpha"][0], conversion_parameters)
        return ell.neural.PReLUActivationLayer(layer_parameters, alpha)

    def convert_node(self, conversion_parameters: typing.Mapping[str, typing.Any]):
        """
        Derived classes override to convert the importer node to appropriate ELL node(s)
        and insert into the model
        """
        model = conversion_parameters["model"]
        builder = conversion_parameters["builder"]
        lookup_table = conversion_parameters["lookup_table"]
        # Create the activation layer
        activation_layer = self.convert(conversion_parameters)
        # Get the port elements from the input
        input_port_elements = lookup_table.get_port_elements_for_input(self.importer_node)
        # Add the ActivationLayerNode to the model
        ell_node = builder.AddActivationLayerNode(model, input_port_elements, activation_layer)
        # Register the mapping
        lookup_table.add_imported_ell_node(self.importer_node, ell_node)


class ConvertReLU(ConvertActivation):
    """
    Converter for ReLU, which is equivalent to
    Activation
    """
    def __init__(self, node: ImporterNode):
        super().__init__(node)
        self.required_weights = []
        self.required_attributes = []
        self.importer_node.attributes["activation"] = ell.neural.ActivationType.relu


class ConvertRegion(ConvertBase):
    """
    Converter for region detection layer
    """
    def __init__(self, node: ImporterNode):
        super().__init__(node)
        self.required_weights = []
        self.required_attributes = ["width", "height", "numBoxesPerCell", "numClasses", "numAnchors", "applySoftmax"]

    def convert(self, conversion_parameters: typing.Mapping[str, typing.Any]):
        """
        Return the appropriate ELL node
        """
        layer_parameters = self.get_layer_parameters(conversion_parameters)

        attributes = self.importer_node.attributes

        region_detection_parameters = ell.neural.RegionDetectionParameters(
                attributes["width"],
                attributes["height"],
                attributes["numBoxesPerCell"],
                attributes["numClasses"],
                attributes["numAnchors"],
                attributes["applySoftmax"]
            )
        
        return ell.neural.FullyConnectedLayer(
            layer_parameters, region_detection_parameters)

    def convert_node(self, conversion_parameters: typing.Mapping[str, typing.Any]):
        """
        Derived classes override to convert the importer node to appropriate ELL node(s)
        and insert into the model
        """
        model = conversion_parameters["model"]
        builder = conversion_parameters["builder"]
        lookup_table = conversion_parameters["lookup_table"]
        # Create the region detection layer
        region_layer = self.convert(conversion_parameters)
        # Get the port elements from the input
        input_port_elements = lookup_table.get_port_elements_for_input(self.importer_node)
        # Add the RegionDetectionLayerNode to the model
        ell_node = builder.AddRegionDetectionLayerNode(model, input_port_elements, region_layer)
        # Register the mapping
        lookup_table.add_imported_ell_node(self.importer_node, ell_node)

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

        return ell.neural.ScalingLayer(layer_parameters, scale)

    def convert_node(self, conversion_parameters: typing.Mapping[str, typing.Any]):
        """
        Derived classes override to convert the importer node to appropriate ELL node(s)
        and insert into the model
        """
        model = conversion_parameters["model"]
        builder = conversion_parameters["builder"]
        lookup_table = conversion_parameters["lookup_table"]
        # Create the scaling layer
        scaling_layer = self.convert(conversion_parameters)
        # Get the port elements from the input
        input_port_elements = lookup_table.get_port_elements_for_input(self.importer_node)
        # Add the ScalingLayerNode to the model
        ell_node = builder.AddScalingLayerNode(model, input_port_elements, scaling_layer)
        # Register the mapping
        lookup_table.add_imported_ell_node(self.importer_node, ell_node)


class OptionalConvertScaling(ConvertScaling):
    """
    Optional converter for Scaling
    """
    def __init__(self, node: ImporterNode):
        super().__init__(node)
        self.optional = True

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
        return ell.neural.SoftmaxLayer(layer_parameters)

    def convert_node(self, conversion_parameters: typing.Mapping[str, typing.Any]):
        """
        Derived classes override to convert the importer node to appropriate ELL node(s)
        and insert into the model
        """
        model = conversion_parameters["model"]
        builder = conversion_parameters["builder"]
        lookup_table = conversion_parameters["lookup_table"]
        # Create the softmax layer
        softmax_layer = self.convert(conversion_parameters)
        # Get the port elements from the input
        input_port_elements = lookup_table.get_port_elements_for_input(self.importer_node)
        # Add the SoftmaxLayerNode to the model
        ell_node = builder.AddSoftmaxLayerNode(model, input_port_elements, softmax_layer)
        # Register the mapping
        lookup_table.add_imported_ell_node(self.importer_node, ell_node)


class ConvertSplice(ConvertBase):
    """
    Converter for Splice, which for now is Output followed by
    Reshape
    """
    def __init__(self, node: ImporterNode):
        super().__init__(node)
        self.required_weights = []
        self.required_attributes = ["dimension_to_stack"]

    def convert(self, conversion_parameters: typing.Mapping[str, typing.Any]):
        """
        Return the appropriate ELL node
        """
        return None

    def convert_node(self, conversion_parameters: typing.Mapping[str, typing.Any]):
        """
        Derived classes override to convert the importer node to appropriate ELL node(s)
        and insert into the model
        """
        model = conversion_parameters["model"]
        builder = conversion_parameters["builder"]
        lookup_table = conversion_parameters["lookup_table"]
        first_in_block = conversion_parameters["first_in_block"]
        last_in_block = conversion_parameters["last_in_block"]

        pre_order = [0,1,2]
        post_order = [0,1,2]
        if self.importer_node.attributes["dimension_to_stack"] == "channel":
            # When output from nodes are concatenated together in the
            # order (channel, row, column), they effectively stack in the
            # channel dimension.
            pre_order = [2,0,1]
            # The final order must put the elements back into row, column,
            # channel order.
            post_order = [1,2,0]
        elif self.importer_node.attributes["dimension_to_stack"] == "row":
            # When output from nodes are concatenated together in the
            # order (row, column, channel), they effectively stack in the
            # row dimension.
            pre_order = [0,1,2]
            # The final order must put the elements back into row, column,
            # channel order.
            post_order = [0,1,2]
            # NOTE: The ReorderDataNodes that are inserted will be removed by the
            # optimizer since they'll be redundant
        elif self.importer_node.attributes["dimension_to_stack"] == "column":
            # When output from nodes are concatenated together in the
            # order (column, row, channel), they effectively stack in the
            # column dimension.
            pre_order = [1,0,2]
            # The final order must put the elements back into row, column,
            # channel order.
            post_order = [1,0,2]
        else:
            raise Exception("Splice does not yet support stacking along dimension {}, just row, column or channel".format(self.required_attributes["dimension_to_stack"]))

        # Loop over all inputs and for each, insert a reorder node to
        # put into specified order.
        reorder_nodes = []
        for input_index in range(len(self.importer_node.inputs)):
            # Create the reorder node
            input_node = lookup_table.get_owning_node_for_output(self.importer_node.inputs[input_index])
            input_port_elements = lookup_table.get_output_port_elements_for_node(input_node)

            # Take the active region of inputs
            port_elements, input_port_memory_layout = lookup_table.get_port_elements_and_memory_layout_for_input(
                self.importer_node, input_index)
            reorder_node = builder.AddReorderDataNode(model, input_port_elements, input_port_memory_layout, input_port_memory_layout, pre_order)
            reorder_nodes.append(reorder_node)
            # Register the mapping
            lookup_table.add_imported_ell_node(self.importer_node, reorder_node)

        # Insert an ConcatenationNode together the reorder nodes
        output_shape, output_padding = self.get_output_parameters(last_in_block)
        reordered_output_shape = ell.math.TensorShape(output_shape.channels, output_shape.rows, output_shape.columns)
        input_port_elements_list = []
        for ell_node in reorder_nodes:
            portElements = lookup_table.get_output_port_elements_for_node(ell_node)
            input_port_elements_list.append(portElements)
        concatenation_node = builder.AddConcatenationNode(model, reordered_output_shape, ell.nodes.PortElementsList(input_port_elements_list))
        # Register the mapping
        lookup_table.add_imported_ell_node(self.importer_node, concatenation_node)

        # Insert a reorder node to to be in row, column, channel order.
        port_elements = lookup_table.get_output_port_elements_for_node(concatenation_node)
        padding_size = output_padding.paddingSize

        reorderedPortMemoryLayout = ell.model.PortMemoryLayout([reordered_output_shape.rows, reordered_output_shape.columns, reordered_output_shape.channels], [reordered_output_shape.rows, reordered_output_shape.columns, reordered_output_shape.channels], [0, 0, 0], pre_order)
        outputPortMemoryLayout = ell.model.PortMemoryLayout([output_shape.rows - 2*padding_size, output_shape.columns - 2*padding_size, output_shape.channels],
                                                            [padding_size, padding_size, 0])
        final_reorder_node = builder.AddReorderDataNode(model, port_elements, reorderedPortMemoryLayout, outputPortMemoryLayout, [0, 1, 2], 0)
        # Register the mapping
        lookup_table.add_imported_ell_node(self.importer_node, final_reorder_node)


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

    def convert_node(self, conversion_parameters: typing.Mapping[str, typing.Any]):
        """
        Derived classes override to convert the importer node to appropriate ELL node(s)
        and insert into the model
        """
        lookup_table = conversion_parameters["lookup_table"]
        # Quick workaround for unnecessary reshapes: Set owner of this output
        # to be the reshape's input node
        input_owner = lookup_table.get_owning_node_for_output(self.importer_node.inputs[0])
        lookup_table.add_imported_ell_node(self.importer_node, input_owner, set_group_id=False)

class ConvertConstant(ConvertBase):
    """
    Converter for Constant nodes
    """
    def __init__(self, node: ImporterNode):
        super().__init__(node)
        self.required_weights = []
        self.required_attributes = [ 'tensor' ]

    def convert(self, conversion_parameters: typing.Mapping[str, typing.Any]):
        """
        Return the appropriate ELL node
        """
        return None

    def convert_node(self, conversion_parameters: typing.Mapping[str, typing.Any]):
        """
        Derived classes override to convert the importer node to appropriate ELL node(s)
        and insert into the model
        """
        model = conversion_parameters["model"]
        builder = conversion_parameters["builder"]
        lookup_table = conversion_parameters["lookup_table"]
        tensor = self.importer_node.attributes["tensor"]
        port_type = ell.nodes.PortType.real
        if tensor.dtype == np.float32:
            port_type = ell.nodes.PortType.smallReal
        elif tensor.dtype == np.int:
            port_type = ell.nodes.PortType.integer
        elif tensor.dtype == np.int64:
            port_type = ell.nodes.PortType.bigInt
        elif tensor.dtype == np.bool:
            port_type = ell.nodes.PortType.boolean

        ell_node = builder.AddConstantNode(model, tensor.ravel().astype(np.float64), port_type)
        lookup_table.add_imported_ell_node(self.importer_node, ell_node)


class ConvertVAD(ConvertBase):
    """
    Converter for Voice Activity Detector.
    """
    def __init__(self, node: ImporterNode):
        super().__init__(node)
        self.required_weights = []
        self.required_attributes = ["sampleRate", "frameDuration", "tauUp", "tauDown", "largeInput", "gainAtt", "thresholdUp", "thresholdDown", "levelThreshold"]

    def convert(self, conversion_parameters: typing.Mapping[str, typing.Any]):
        """
        Return the appropriate ELL layer
        """
        raise Exception("No corresponding ELL layer for Voice Actvitity Detector (VAD). Use node instead.")

    def convert_node(self, conversion_parameters: typing.Mapping[str, typing.Any]):
        """
        Derived classes override to convert the importer node to appropriate ELL node(s)
        and insert into the model
        """
        model = conversion_parameters["model"]
        builder = conversion_parameters["builder"]
        lookup_table = conversion_parameters["lookup_table"]

        sample_rate = self.importer_node.attributes["sampleRate"]
        frame_duration = self.importer_node.attributes["frameDuration"]
        tau_up = self.importer_node.attributes["tauUp"]
        tau_down = self.importer_node.attributes["tauDown"]
        large_input = self.importer_node.attributes["largeInput"]
        gain_att = self.importer_node.attributes["gainAtt"]
        threshold_up = self.importer_node.attributes["thresholdUp"]
        threshold_down = self.importer_node.attributes["thresholdDown"]
        level_threshold = self.importer_node.attributes["levelThreshold"]

        # Create the VAD node
        ell_node = builder.AddVoiceActivityDetectorNode(model, input_port_elements, 
            sample_rate, frame_duration, tau_up, tau_down, large_input, gain_att, 
            threshold_up, threshold_down, level_threshold)

        # Register the mapping
        lookup_table.add_imported_ell_node(self.importer_node, ell_node)
