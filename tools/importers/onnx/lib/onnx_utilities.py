####################################################################################################
#
# Project:  Embedded Learning Library (ELL)
# File:     onnx_utilities.py (importers)
# Authors:  Iliass Tiendrebeogo
#
# Requires: Python 3.x, onnx-v1.22
#
####################################################################################################

import argparse
import typing
from typing import Text
import logging 

import numpy as np
import onnx
from onnx import ModelProto, helper, numpy_helper

import find_ell
import ell
from common.importer import *
from common.converters import *
from lib._graph_parser import Graph 

_logger = logging.getLogger(__name__)

#####################################################################################
##############  Util class
#####################################################################################
class Utilities(object):
    """
    Utility class for processing ONNX models.
    """
    
    def __init__(self):
        self._shape_dict = {} # weights indexes and shape: Example {'node_id': ['w', 'b']}
        self._tensor_dict = {} # weights
        self._all_nodes = {} # all processed nodes
        self._auxiliary_nodes = {} # nodes get from value_info, they only carry weights or bias ...

    def get_shape_dict(self):
        return self._shape_dict

    def get_all_nodes(self):
        return self._all_nodes

    def get_padding(self, node):
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
        pad = 0 # default padding
        if 'padding' in node.attribute:
            pad = node.attribute['padding']

        return {"size": pad, "scheme": ell.neural.PaddingScheme.zeros}

    def get_weights(self, node):
        if node.op_type == "Convolution":
            return self._conv_weights(node)
        elif node.op_type == "BatchNormalization":
            return self._batch_norm_weights(node)
        else:
            return self._get_weights(node)
    
    def _get_weights(self, node):
        """
        return a dict of weights in the format:
        e.g: for a conv2d node
        { 
          'weights' : (weight_index, tensor, 'filter_channel_row_column')
          'bias': (weight_index, tensor, 'channel')
         }
        weights['weights'] = (weight_index, tensor, 'filter_channel_row_column')
        weights['bias'] = (weight_index, tensor, 'channel')
        """
        if node.op_type == "Bias": # Weights already processed
            return 

        _weights = list(node.input_tensors)
        if not _weights:
            return {}
        result = {}
        tensor = None
        if node.op_type == "BatchNormalization" :
            bias = _weights[0]
            scale = _weights[1]
            mean = _weights[2]
            variance = _weights[3]
            result['bias'] = (bias, node.input_tensors[bias], "channel")
            result['scale'] = (scale, node.input_tensors[scale], "channel")
            result['mean'] = (mean, node.input_tensors[mean], "channel")
            result['variance'] = (variance, node.input_tensors[variance], "channel")
        else:
            for weight_index in _weights:
                tensor = node.input_tensors[weight_index]
                tensor_len = len(tensor.shape)
                if tensor_len  == 4:
                    result['weights'] = (weight_index, tensor, 'filter_channel_row_column')
                elif tensor_len == 3 :
                    result['weights'] = (weight_index, tensor, 'channel_row_column')
                elif tensor_len == 2 :
                    result['weights'] = (weight_index, tensor, 'row_column')
                else :
                    result['bias'] = (weight_index, tensor, 'column')

        return result

    # TODO: call this in get_weights
    def _conv_weights(self, node):
        """
        """
        _weights = list(node.input_tensors)
        if not _weights:
            return {}
        result = {}
        tensor = None 

        for weight_index in _weights:
            tensor = node.input_tensors[weight_index]
            tensor_len = len(tensor.shape)
            if tensor_len  == 4:
                result['weights'] = (weight_index, tensor, 'filter_channel_row_column')
            elif tensor_len == 3:
                result['weights'] = (weight_index, tensor, 'channel_row_column')
            elif tensor_len == 2:
                result['weights'] = (weight_index, tensor, 'row_column')
            elif tensor_len == 1:
                result['bias'] = (weight_index, tensor, 'column')

        
        return result

    # TODO: Call this in conv2d
    def _conv_attributes(self, node):
        """
        Return dict of attribute in the format 
        attributes['size'] = node.attribute['kernel_shape'][0]
        attributes['stride'] = node.attribute['strides'][0]
        """
        attributes = {}
        if 'kernel_shape' in node.attribute:
            attributes['size'] = node.attribute['kernel_shape'][0]
            attributes['stride'] = node.attribute['strides'][0]
        if 'pads' in node.attribute:
            attributes['padding'] = node.attribute['pads'][0]
        if 'dilations' in node.attribute:
            attributes['dilations'] = node.attribute['dilations'][0]

        return attributes
    
    def conv2d(self, node):
        """
        Parse convolution node
        """
        # make shape inference using node attributes
        
        f = None
        p = None
        s = None 

        if 'kernel_shape' in node.attribute:
            f = node.attribute['kernel_shape'][0]

        if 'pads' in node.attribute:
            p = node.attribute['pads'][0]

        if 'strides' in node.attribute:
            s = node.attribute['strides'][0]

        self.get_input_shape(node)
        self.get_conv_con2d_out_dims(node, node.input_shape, f, p, s)
        self._conv_weights(node)
        self._conv_attributes(node)
        return node

    def batch_norn(self, node):
        """
        """
        self.get_input_shape(node)
        self._batch_norm_weights(node)

    # TODO: call this in get_weights
    def _batch_norm_weights(self, node):
        """
        Get weights for a batch normalization node
        """
        
        _weights = list(node.input_tensors)
        if not _weights:
            return {}
        result = {}

        scale = _weights[0]
        bias = _weights[1]
        mean = _weights[2]
        variance = _weights[3]
        result['bias'] = (bias, node.input_tensors[bias], "channel")
        result['scale'] = (scale, node.input_tensors[scale], "channel")
        result['mean'] = (mean, node.input_tensors[mean], "channel")
        result['variance'] = (variance, node.input_tensors[variance], "channel")
        return result

    def get_activation_type(self, node):
        """Returns an ell.neural.ActivationType from the list of nodes"""
        if node.op_type == 'ReLU':
            return ell.neural.ActivationType.relu
        elif node.op_type == 'Sigmoid':
            return ell.neural.ActivationType.sigmoid
        elif node.op_type == 'LeakyReLU':
            return ell.neural.ActivationType.leaky 
        else:
            return None
        
    def get_attributes(self, node):
        """
        Return dict of attribute in the format 
        attributes['size'] = node.attribute['kernel_shape'][0]
        attributes['stride'] = node.attribute['strides'][0]
        """
        attributes = {}
        if 'kernel_shape' in node.attribute:
            attributes['size'] = node.attribute['kernel_shape'][0]
            attributes['stride'] = node.attribute['strides'][0]
        if 'pads' in node.attribute:
            attributes['padding'] = node.attribute['pads'][0]
        if 'dilations' in node.attribute:
            attributes['dilations'] = node.attribute['dilations'][0]

        if node.op_type == "Splice":
            attributes['dimension_to_stack'] = 'channel'
            attributes['axis'] = 1
        if self.get_activation_type(node) is not None:
            attributes['activation'] = self.get_activation_type(node)

        return attributes


    def get_input_shape(self, node):
        """
        Return a tuple shape in the format shape order
        Example: [((64,224,224),"channel_row_column")]
        """
        shape = []
        # if  node.op_type == 'Softmax':
        #     return self.get_softmax_shape_(node)
        if node.op_type == "FullyConnected":
            return self.flatten_shape(node)

        for in_node in node.inputs:
            if in_node in self._all_nodes:
                if self._all_nodes[in_node] is not None and len(self._all_nodes[in_node].output_shape) > len(shape):
                    shape = self._all_nodes[in_node].output_shape
        return shape

    def get_output_shape(self, node):
        """
        Computes the spatial output shape
        """

        if  node.op_type == 'Input':
            return node.output_shape
        
        input_shape = node.input_shape

        # make shape inference using node attributes
        
        f = None
        p = None
        s = None 

        if 'kernel_shape' in node.attribute:
            f = node.attribute['kernel_shape'][0]

        if 'pads' in node.attribute:
            p = node.attribute['pads'][0]

        if 'strides' in node.attribute:
            s = node.attribute['strides'][0]

        if node.op_type == "Convolution":
            return self.get_conv_con2d_out_dims(node, f, p, s)
        elif node.op_type == "MaxPooling":
            return self.get_maxpool_out_dims( node, f, p, s)
        elif node.op_type == "FullyConnected":
            return self.get_fc_out_dims(node, f, p, s)
        elif node.op_type == "Splice":
            return self.get_concat_out_dims(node, f, p, s)
        # elif node.op_type == "Softmax":
        #     return self.get_softmax_shape_(node)
        elif node.op_type == "Flatten":
            return self.flatten_shape(node)
        # elif node.op_type == "Reshape":
        #     return self.get_reshape_out_dims(node)
        elif node.name in self._shape_dict:
            return self.get_sink_node_dims(node)
        else:
            return input_shape

    def get_number_of_filter(self, node):
        """
        Return the number of filter used for this node operation
        this is needed for the spatial output_shape
        """
        filters = None
        shape_index = list(node.input_tensors)
        if node.name in self._shape_dict:
            filters = self._shape_dict[node.name]
        if len(shape_index) > 0:
            filters = self._shape_dict[shape_index[0]]
        return filters

    def get_sink_node_dims(self, node):
        """ Output spatial dimension for the graph's output/sink node"""

        shape = self._shape_dict[node.name]
        order = "channel_row_column"
        if len(shape) == 2:
            order = "channel_row"
        elif len(shape) == 1:
            order = "channel"
        channel = list(shape)[-1]
        dims = node.input_shape 

        # replace the first dimension with channel
        dims[0] = channel
        return [(dims, order)]


    def get_conv_con2d_out_dims(self, node, kernel_size, padding, strides):
        """
        Compute the spatial output shape of the convolutional node 
        using filter_size, paddings, strides ...
        Formula: Dim = (input_dim - filter_size + 2*padding)/strides + 1
        """
        channel, row, col = node.input_shape[0][0]
        f = kernel_size
        p = padding
        s = strides

        if self.get_number_of_filter(node):
            channel = self.get_number_of_filter(node)[0]
        
        out_row = (row - f + 2*p)/s + 1
        out_col = (col - f + 2*p)/s + 1

        dims = (channel, int(out_row), int(out_col)) 

        return [(dims, 'channel_row_column')]

    def get_maxpool_out_dims(self, node, kernel_size, padding, strides):

        channel, row, col = node.input_shape[0][0]
        f = kernel_size
        p = padding
        s = strides

        if not node.input_shape[0] or node.input_shape[0][0] == 0:
            if self.get_number_of_filter(node):
                channel = self.get_number_of_filter(node)[0]

        out_row = (row - f)/s + 1
        out_col = (col - f)/s + 1

        dims = (channel, int(out_row), int(out_col)) 

        return [(dims, 'channel_row_column')]

    def get_reshape_out_dims(self, node):
        channel = 1
        row     = 1 
        col     = 1
        if len(node.input_shape) > 1:
            channel, row, col = node.input_shape[0][0]
        
        if self.get_number_of_filter(node) is not None:
            channel = self.get_number_of_filter(node)[0]
        else: 
            return node.input_shape

        shape_ = []
        shape_.append(channel)
        # shape_ = input1_out_shape
        shape_.extend(node.input_shape[1:])
        shape_ = tuple(shape_)
        order = 'channel_row_column'
        if len(shape_) == 2:
            order = 'channel_row'
        if len(shape_) == 1:
            order = 'channel'
        return [(shape_, order)]

    def flatten_shape(self, node):
        """
        Flatten the spatial shape. This is needed to be passed to a 
        FullyConnected layer or other layers...
        """
        input_shape = []
        for node in node.inputs:
            if node in self._all_nodes and len(self._all_nodes[node].output_shape) > len(input_shape):
                input_shape = self._all_nodes[node].output_shape

        if len(input_shape[0][0]) == 1:
            return input_shape

        channel, row, col = input_shape[0][0]

        dims = (channel*row*col,)

        return [(dims, 'channel')]

    def get_fc_out_dims(self, node, kernel_size, padding, strides):
        input_shape = node.input_shape
        channel, row, col = 0, 0, 0
        if len(input_shape[0][0]) > 2:
            channel, row, col = node.input_shape[0][0]
        
        if self.get_number_of_filter(node) is not None:
            channel = self.get_number_of_filter(node)[0]

        dims = (channel,) # make it tuple

        shape = node.input_shape
        # if node.name in self._shape_dict:
        #     shape = self._shape_dict[node.name]
        

        return [(dims, 'channel')]

    def get_concat_out_dims(self, node, kernel_size, padding, strides):
        """ Computes the output shape for a concat/slpice node 
        """
        input_shape = node.input_shape
        dims = []
        shape = []
        dim0 = 0

        if len(node.inputs) < 2:
            print("Single input Concatenation")
            return input_shape
        # get each input node's output channel to be added together 
        for in_node in node.inputs:
            if in_node in self._all_nodes:
                dim = self._all_nodes[in_node].output_shape[0][0] # tuple: shape
                dims.append(dim[0]) 
                # Update shape to the biggest shape
                if len(dim) > len(shape):
                    shape = dim
        # get the number of filters directly from the child node
        # this avoid any mismatch
        if node.children is not None:
            for child in node.children:
                child_tensor_shape = self.get_number_of_filter(child)
                if child_tensor_shape is not None:
                    if len(child_tensor_shape) == 1:
                        dim0 = child_tensor_shape[0]
                    else:
                        dim0 = child_tensor_shape[1]

        # Stack up (add) the channels from both nodes
        if dim0 == 0:        
            for x in dims:
                dim0 += x

        # axis = node.attribute['axis']
        
        # if axis != 1:
        #     raise NotImplementedError("Only support channel wise concatenation for now")
        
        shape_ = []
        shape_.append(dim0)
        # shape_ = input1_out_shape
        shape_.extend(shape[1:])
        shape_ = tuple(shape_)
        order = 'channel_row_column'
        if len(shape_) == 2:
            order = 'channel_row'
        if len(shape_) == 1:
            order = 'channel'
        return [(shape_, order)]

    def get_softmax_shape_(self, node):
        """
        Compute the input and output shape for softmax node
        """
        channel = None
        if self._shape_dict[node.name] is not None:
            channel = self._shape_dict[node.name][1]
        dims = (channel,) # make it tuple
        return [(dims, 'channel')]

    def get_shape_from_list_of_nodes(self, nodelist):
        """
        Return the most relevant output shape from a list of nodes
        """
        shape_ = []
        for node in nodelist:
            if node in self._all_nodes and len(self._all_nodes[node].output_shape) > len(shape_):
                shape_ = self._all_nodes[node].output_shape
        return shape_

    ##################### Auxillary Nodes parsing ##########################
    def _get_aux_node_weights(self, node):
        name = node.name
        tensor = None
        order = "channel"
        tensor_type = "bias"
        weights = {}
        if name in self._tensor_dict:
            tensor = self._tensor_dict[name]
            if len(tensor.shape) > 1 : 
                order = "filter_channel_row_column"
                tensor_type = "weights"

        weights = { tensor_type : (name, tensor, order) }

        return weights

    def _get_aux_node_shape(self, node):
        shape = self._shape_dict[node.name]
        order = "channel_row_column"
        if len(shape) == 1:
            order = "channel"

        return [(shape, order)]

    def add_auxiliary_node(self, name, nodes_):
        """
        Return a parsed external(node without parent) node
        """
        if name in self._auxiliary_nodes:
            node =  self._auxiliary_nodes[name]
        else:
            raise KeyError("Node {} not in Auxillary nodes dict", name)
        prev_node = nodes_[-1]
        node.inputs = prev_node.outputs
        print("Aux input", node.name, node.inputs)                        
        node.input_shape = prev_node.output_shape# self._get_aux_node_shape(node)
        node.output_shape = prev_node.output_shape # self._get_aux_node_shape(node)
        node.padding = self.get_padding(node)
        node.weights = self._get_aux_node_weights(node)
        nodes_.append(node)
        self._all_nodes[name] = node
        print("Dependent node {}:{} added to processed nodes".format(node.name, node.op_type))
        return node 

    # @staticmethod
    def parse_node(self, node):
        """
        Do parsing
        return a node with Importer node parameter
        """
        node.input_shape = self.get_input_shape(node)
        node.output_shape = self.get_output_shape(node)
        node.attribute = self.get_attributes(node)
        node.padding = self.get_padding(node)
        node.weights = self.get_weights(node)
        if node.op_type == "Flatten":
            node.op_type = "Reshape"
        
        if node.op_type == "Convolution" or node.op_type == "BatchNormalization":
            node.inputs = [node.inputs[0]]

        return node
        
    #############################################################
    ######## Load onnx model 
    #############################################################

    def _parse_onnx_model(self, path):
        """Get nodes from _graph_parser and format them for ImporterNode field
        Returns a list of the high-level nodes (i.e. function blocks) that make up the ONNX model
        """
        _logger.info("loading the model")
        try:
            if isinstance(path, onnx.ModelProto): 
                onnx_model = path
            else:
                onnx_model = ModelProto()

            with open(path, 'rb') as f:
                content = f.read()
                onnx_model.ParseFromString(content)

            # Check that the IR is well formed
            # onnx.checker.check_model(onnx_model)

        except Exception :
            _logger.error("Error occurred when loading onnx model file ")
            raise Exception 
        _logger.info("\nFinished loading.") 

        _logger.info("Graph producer: {} version {}".format(onnx_model.producer_name,
                                    onnx_model.producer_version))
        _logger.info("Graph total len: {}".format(len(onnx_model.graph.input)))
        graph_ = Graph.from_onnx(onnx_model.graph)
        _logger.info("Graph external Input/Output node: {} -> {}".format(graph_.inputs, graph_.outputs))
        
        self._shape_dict = graph_.shape_dict
        self._auxiliary_nodes = graph_.tensor_nodes
        self._tensor_dict = graph_.input_tensors # weights and bias

        # get all tensors weights in one dict
        node_to_aux = {}
        for v in graph_.nodes:
            if v.input_tensors:
                input_tensorlist = list(v.input_tensors)
                node_to_aux[v.name] = input_tensorlist 

        # TODO: for debugging
        print("Tensor dict \n", node_to_aux)
        print("Shape dict \n", self._shape_dict)

        to_concat = [] 
        nodes_ = []
        graph_tree = graph_.nodes
        for i, node in enumerate(graph_tree): # skip input node already parsed
            # Handle Skip nodes
            if node.op_type == "Skip" or node.op_type == "Reshape":
                _logger.info("Skipping node {} ".format(node.name))
                if len(node.inputs) == 0:
                    continue
                next_node = None
                for v in graph_tree[i+1:]:
                    if node.name in v.inputs:
                        next_node = v
                if next_node.name == 'lastNode':
                    next_node = None

                if next_node is not None:                   
                    # Find and replace skipped node
                    for index, input_ in enumerate(next_node.inputs):                    
                        if input_ == node.name:
                            next_node.inputs[index] = node.inputs[0]
                    print("Skipped node {} {} : previous node {} : next node {}".format(
                                                node.name, node.op_type, node.inputs[0], 
                                                next_node.name))
                    continue
                else:
                    to_concat.append(node.name)
                    print(node.name, "Node not skipped: NextNode not found after all nodes been exausted. Must be a sink node.")

            # Handle External/Auxiliary nodes 
            if node.op_type == "Splice"  or node.op_type == "Plus" :
                # First thing first, let's add missing input nodes
                aux_node = None
                for in_node in node.inputs:
                    if in_node not in self._all_nodes:
                        aux_node = self.add_auxiliary_node(in_node, nodes_)                       
                        

            op_node = self.parse_node(node)
            nodes_.append(op_node)

            # update self._all_nodes
            self._all_nodes[op_node.name] = op_node
            
            print(op_node.name," :", op_node.op_type, op_node.inputs ,"->", 
                  op_node.outputs,"Attr: ", node.attribute, "Value Info", node.padding)
            print("Input shape :", op_node.input_shape,"-> Output shape :", op_node.output_shape, "\n") 

        return nodes_ 

# _operation_map = {
#     'Input'                    : Input,
#     # Basic neural network functions
#     'Convolution'              : Conv,
#     'Plus'                     : Add,
#     'Splice'                   : Concat,
#     'MaxPooling'               : MaxPooling,
#     "BatchNormalization"       : BatchNorm,
#     "FullyConnected"           : FullyConnected,
#     }

# _functions_map = {
#     "Convolution"   : get_conv_con2d_out_dims,
#     "Maxpooling"    : get_maxpool_out_dims,
#     "FullyConnected": get_fc_out_dims,
#     "Reshape"       : get_reshape_out_dims,
#     "Splice"        : get_concat_out_dims
# } 