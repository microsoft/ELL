####################################################################################################
#
# Project:  Embedded Learning Library (ELL)
# File:     _graph_parser.py (util)
# Authors:  Iliass Tiendrebeogo
#
# Requires: Python 3.x, onnx-v1.22
#
####################################################################################################

from __future__ import absolute_import
from __future__ import division
from __future__ import print_function
from __future__ import unicode_literals

from onnx import numpy_helper, ValueInfoProto, AttributeProto, GraphProto, NodeProto, TensorProto, TensorShapeProto
from typing import Any, Text, Iterable, List, Dict, Sequence, Optional, Tuple, Union
import numpy as np
import logging


_logger = logging.getLogger(__name__)


ONNX_OP_TYPE_TO_ELL_MAP  = {
    "Input"                  : "Input",
    "InputNode"              : "Input",
    # Arithmetic Operations
    "Add": "Plus",
    # Basic neural net functions
    "Conv"                    : "Convolution",
    "Convolution"             : "Convolution",
    "Mul"                     : "ElementTimes",
    # "MatMul"                  : "ElementTimes",
    "Bias"                    : "Bias",
    "Constant"                : "Skip",
    "Dropout"                 : "Skip",
    "Relu"                    : "ReLU",
    "LeakyRelu"               : "LeakyReLU",
    "ReLU"                    : "ReLU",
    "AveragePool"             : "AveragePooling",
    "MaxPool"                 : "MaxPooling",
    "GlobalAveragePool"       : "Maxpooling", # Temp: not implemented
    "Reshape"                 : "Reshape",
    "Softmax"                 : "Softmax",
    "LogSoftmax"              : "Softmax",
    "Gemm"                    : "FullyConnected",
    "FullyConnected"          : "FullyConnected",
    "BatchNormalization"      : "BatchNormalization",
    # Reshape 
    "Concat"                  : "Splice",
    "Transpose"               : "Skip",
    # "Flatten"                 : "Flatten",   
    "Flatten"                 : "Skip",   
    "Passthrough"             : "Passthrough",
    "Unsqueeze"               : "Bias", # Temp: not implemented
    "Squeeze"                 : "Constant", # Temp: Not implemented,
    "ImageScaler"             : "ImageScaler", 
}


EdgeInfo = Tuple[Text, Any, TensorShapeProto]
AttributeValue = Any # TODO Union[Sequence[float], Sequence[int], Sequence[Text], Sequence[TensorProto], Sequence[GraphProto]]

def _input_from_onnx_input(input):  # type: (ValueInfoProto) -> EdgeInfo
    name = input.name
    type = input.type.tensor_type.elem_type
    shape = tuple([d.dim_value for d in input.type.tensor_type.shape.dim])
    return (name, type, shape)

def _make_input_node(input):
    name = input.name
    type = input.type.tensor_type.elem_type
    shape = tuple([d.dim_value for d in input.type.tensor_type.shape.dim])
    shape_ = []
    # shape_.append(shape[0])
    shape_.extend(list(shape[1:]))
    
    first_node = Node.make_input(name)
    first_node.input_shape = [(tuple(shape_), "channel_row_column")]
    first_node.output_shape = [(tuple(shape_), "channel_row_column")]
    print(first_node.name," :", first_node.op_type, first_node.inputs ,"->",first_node.outputs,"Attr: ", first_node.attribute)
    return first_node 


def _convertAttributeProto(onnx_arg):  # type: (AttributeProto) -> AttributeValue
    """
    Convert an ONNX AttributeProto into an appropriate Python object
    for the type.
    NB: Tensor attribute gets returned as numpy array
    """
    if onnx_arg.HasField('f'):
        return onnx_arg.f
    elif onnx_arg.HasField('i'):
        return onnx_arg.i
    elif onnx_arg.HasField('s'):
        return onnx_arg.s
    elif onnx_arg.HasField('t'):
        return numpy_helper.to_array(onnx_arg.t)
    elif len(onnx_arg.floats):
        return list(onnx_arg.floats)
    elif len(onnx_arg.ints):
        return list(onnx_arg.ints)
    elif len(onnx_arg.strings):
        return list(onnx_arg.strings)
    else:
        raise ValueError("Unsupported ONNX attribute: {}".format(onnx_arg))


class Attributes(Dict[Text, Any]):
    @staticmethod
    def from_onnx(args):  # type: (Iterable[AttributeProto]) -> Attributes
        d = Attributes()
        for arg in args:
            d[arg.name] = _convertAttributeProto(arg)
        return d


class Node(object):
    def __init__(self,
                 name,  # type: Optional[Text]
                 op_type,  # type: Text
                 attrs,  # type: Dict[Text, AttributeValue]
                 inputs,  # type: List[Text]
                 outputs,  # type: List[Text]
                 ):
        # type: (...) -> None
        self.name = name
        self.op_type = ONNX_OP_TYPE_TO_ELL_MAP[op_type]
        self.attribute = attrs
        self.inputs = inputs
        self.outputs = outputs
        self.input_tensors = {}  # type: Dict[Text, np._ArrayLike[Any]]
        self.parents = []  # type: List[Node]
        self.children = []  # type: List[Node]
        self.input_shape = [] # type: List
        self.output_shape = [] # type: List[]
        self.weights = {} # type: Dict[String, List[tuple]]
        self.padding = {} 
        self.metadata = {}  # type: Dict[Any, Any]
        self.value_info = ""

    def get_metadata(self, node):
        return node.metadata

    def add_parent(self, parent_node):  # type: (Node) -> None
        assert parent_node not in self.parents
        self.parents.append(parent_node)
        if self not in parent_node.children:
            parent_node.children.append(self)

    def add_child(self, child_node):  # type: (Node) -> None
        assert child_node not in self.children
        self.children.append(child_node)
        if self not in child_node.parents:
            child_node.parents.append(self)

    def get_only_parent(self):  # type: () -> Node
        if len(self.parents) != 1:
            raise ValueError('Node ({}) expected to have 1 parent. Found {}.'
                             .format(self, len(self.parents)))
        return self.parents[0]

    @staticmethod
    def from_onnx(node):  # type: (NodeProto) -> Node
        attrs = Attributes.from_onnx(node.attribute)
        name = Text(node.name)
        if len(name) == 0:
            # name = "_".join(node.output)
            name = node.output[0]
        return Node(
            name, node.op_type, attrs, list(node.input), list(node.output)
        )

    @staticmethod
    def make_input(name):
        name = Text(name)
        if len(name) == 0:
            name = "0"
        return Node(name, 'InputNode', {}, [], [name])
    

class Graph(object):
    def __init__(self,
                 nodes,  # type: List[Node]
                 inputs,  # type: List[EdgeInfo]
                 outputs,  # type: List[EdgeInfo]
                 shape_dict, # type: Dict[Text,Tuple[int,...]]
                 tensor_nodes, # type: Dict[Text,Tuple[int,...]]
                 input_tensors,
                 ):
        # type: (...) -> None
        self.nodes = nodes
        self.inputs = inputs
        self.outputs = outputs
        self.shape_dict = shape_dict  # data blob name to its shape
        self.tensor_nodes = tensor_nodes  # nodes from initializer and value_info
        self.input_tensors = input_tensors

        # data blob name to the list of op types it feeds into
        self.blob_to_op_type = {} # type: Dict[Text, List[Text]]
        # data blob name to the op_type that generates it
        self.blob_from_op_type = {}  # type: Dict[Text, Text]

        for node_ in nodes:
            for input_ in node_.inputs:
                if input_ in self.blob_to_op_type:
                    self.blob_to_op_type[input_].append(node_.op_type)
                else:
                    self.blob_to_op_type[input_] = [node_.op_type]
            for output_ in node_.outputs:
                if output_ in self.blob_from_op_type:
                    print("Data blob: {}, is generated by more than 1 op: {} {}".format(output_, self.blob_from_op_type[output_], node_.name))
                    raise ValueError("Data blob: %s, is generated by more than 1 op" %(output_))
                self.blob_from_op_type[output_] = node_.op_type


    def transformed(self, transformers):  # type: (Iterable[Transformer]) -> Graph
        graph = self
        for transformer in transformers: 
            graph = transformer(graph)
        return graph

    def has_edge_name(self, name):  # type: (Text) -> bool
        '''
        Check if name is already used for graph inputs/outputs or for nodes
        inputs/outputs
        '''
        names = set()
        for input in self.inputs:
            names.add(input[0])
        for output in self.outputs:
            names.add(output[0])
        for node in self.nodes:
            names.update(node.inputs)
            names.update(node.outputs)
        return name in names

    def get_unique_edge_name(self, name):  # type: (Text) -> Text
        n_ = name
        i = 0
        while self.has_edge_name(n_):
            n_ = "{}_{}".format(name, i)
            i += 1
        return n_

    @staticmethod
    def from_onnx(graph):  # type: (GraphProto) -> Graph
        
        input_tensors = {
            t.name: numpy_helper.to_array(t) for t in graph.initializer
        }

        nodes_ = []
        # add input_node first
        for i in graph.input:
            if i.name not in input_tensors:
                nodes_.append(_make_input_node(i))

        nodes_by_input = {}  # type: Dict[Text, List[Node]]
        nodes_by_output = {}
        for node in graph.node: 
            node_ = Node.from_onnx(node) 
            for input_ in node_.inputs:
                if input_ in input_tensors:
                    node_.input_tensors[input_] = input_tensors[input_]
                else:
                    if input_ in nodes_by_input:
                        input_nodes = nodes_by_input[input_]
                    else:
                        input_nodes = []
                        nodes_by_input[input_] = input_nodes
                    input_nodes.append(node_)
            for output_ in node_.outputs:
                nodes_by_output[output_] = node_
            nodes_.append(node_)

        inputs = []
        for i in graph.input:
            if i.name not in input_tensors:
                inputs.append(_input_from_onnx_input(i))

        outputs = []
        for o in graph.output:
            outputs.append(_input_from_onnx_input(o))

        for node_ in nodes_:
            for input_ in node_.inputs:
                if input_ in nodes_by_output:
                    node_.parents.append(nodes_by_output[input_])
            for output_ in node_.outputs:
                if output_ in nodes_by_input:
                    node_.children.extend(nodes_by_input[output_])

        # Used to concat models with multiples outputs nodes
        to_concat = []
        for node_ in nodes_:
            if node_.op_type != "Input" and node_.children is None or len(node_.children) == 0: # sink node
                to_concat.append(node_.name)

        # Dictionary to hold the "value_info" field from ONNX graph
        shape_dict = {} # type: Dict[Text,Tuple[int,...]]
        dim_params = {} # type: Dict[Text, Tuple[int,...]]
              

        def extract_value_info(shape_dict, # type: Dict[Text,Tuple[int,...]]
                               value_info, # type: ValueInfoProto[...]
                               ):
            # type: (...) -> None
            shape_dict[value_info.name] = tuple([int(dim.dim_value) for dim in value_info.type.tensor_type.shape.dim])
            # dim_params[value_info.name] = tuple([dim.dim_param for dim in value_info.type.tensor_type.shape.dim])

        for value_info in graph.value_info:
            extract_value_info(shape_dict, value_info)   
        for value_info in graph.input:
            extract_value_info(shape_dict, value_info)
        for value_info in graph.output:
            extract_value_info(shape_dict, value_info)
        
        class Node_:
            "Transform dict to Node object"
            def __init__(self, node_):
                for k, v in node_.items():
                    setattr(self, k, v)

        # def make_node_from_value_info(shape_dict)
        auxiliary_nodes = {}

        for name in shape_dict:
            shape = shape_dict[name]           
            op_type = "Constant"           
            if len(shape) == 1:
                op_type = "Bias"

            node_obj = {'name'       : name, 
                        'op_type'    : op_type,
                        'attribute'  : {},
                        'input'      : [],
                        'output'     : [name]}
            node_ = Node_(node_obj) # make it object
            node = Node.from_onnx(node_)  
            auxiliary_nodes[name] = node

        ################################################################################
        print("Sink nodes", to_concat)
        # Make the last node
        last_node_dict_obj = {  'name'       : 'lastNode', 
                                'op_type'    : 'Concat',
                                'attribute'  : {},
                                'input'      : to_concat,
                                'output'     : ['lastNode']}
        lastnode_ = Node_(last_node_dict_obj) # make it object
        lastnode = Node.from_onnx(lastnode_)
        # only add concat if model has multiple sink nodes
        if len(to_concat) > 1:
            nodes_.append(lastnode)

        return Graph(nodes_, inputs, outputs, shape_dict, auxiliary_nodes, input_tensors)
