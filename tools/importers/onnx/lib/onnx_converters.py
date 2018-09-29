####################################################################################################
#
# Project:  Embedded Learning Library (ELL)
# File:     onnx_converters.py (importers)
# Authors:  Chris Lovett
#
# Requires: Python 3.x, onnx-v1.22
#
####################################################################################################

from __future__ import absolute_import
from __future__ import division
from __future__ import print_function
from __future__ import unicode_literals

import os
import argparse
from typing import Text
import sys
import logging
import time

import numpy as np

import onnx
from onnx import  ModelProto, helper, numpy_helper, ValueInfoProto, AttributeProto, GraphProto, NodeProto, TensorProto, TensorShapeProto
from typing import Any, Text, Iterable, List, Dict, Sequence, Optional, Tuple, Union

sys.path.append(os.path.join(os.path.dirname(os.path.abspath(__file__)), '../../utilities/pythonlibs'))
sys.path.append(os.path.join(os.path.dirname(os.path.abspath(__file__)), '..'))

import find_ell
import ell
import common.importer
import common.converters

_logger = logging.getLogger(__name__) 
AttributeValue = Any 

class Attributes(Dict[Text, Any]):
    @staticmethod
    def from_onnx(args):  # type: (Iterable[AttributeProto]) -> Attributes
        d = Attributes()
        for arg in args:
            d[arg.name] = Attributes._convertAttributeProto(arg)
        return d

    @staticmethod
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

class OnnxConverter: pass # forward declaration for typing

class OnnxNodeConverter(object):
    """
    Base class for all onnx node node converters.
    This class also provides some helper methods that are handy for those converters
    """
    def init(self, converter: OnnxConverter, op_type: str):
        self.converter = converter
        self.op_type = op_type

    def log(self):
        """
        Used for diagnostics
        """
        _logger.info("{}.{}, inputs {} -> outputs {}".format(self.node.op_type, self.node.name, self.node.inputs, self.node.outputs))
        _logger.info("    weights: {}".format("".join(["({}: {}{},order='{}')".format(w, self.node.weights[w][0], self.node.weights[w][1].shape, self.node.weights[w][2]) for w in self.node.weights.keys()])))
        _logger.info("    attributes: {}".format(self.node.attribute))
        _logger.info("    padding: {}".format(self.node.padding))
        
    def convert(self, onnx_node):
        name = Text(onnx_node.name)
        if len(name) == 0:
            # the name must match the output id so we can match this up with other nodes 'inputs' list.
            # bugbug: what if there is more than one output?
            name = onnx_node.output[0]

        node = common.converters.ImporterNode( id = name, 
                                        operation_type = self.op_type,
                                        inputs = list(id for id in onnx_node.input if id != ''),
                                        outputs = list(id for id in onnx_node.output if id != ''),
                                        metadata = { "op_type": self.op_type, "id": name } 
                                        ) 
        self.node = node

        # assuming onnx_node is of type: NodeProto, this performs some common conversion operations
        node.prune           = True # whether to automatically prune tensor inputs.
        onnx_attributes      = Attributes.from_onnx(onnx_node.attribute)
        node.attributes      = self.get_attributes(onnx_attributes)
        node.onnx_attributes = onnx_attributes,
        node.onnx_node       = onnx_node
        node.input_shapes    = self.get_input_shapes()
        node.output_shapes   = self.get_output_shapes()
        node.padding         = self.get_padding()
        node.weights         = self.get_weights()   
        return node

    def get_attributes(self, attrs: Attributes):
        return {}
    
    def get_output_shapes(self):
        return self.node.input_shapes

    def get_weights(self):
        return {}

    def get_padding(self):
        """
        Derived classes can override. Return is a dict:
        {"size": size_value, "scheme": scheme_value} 
        where:
        size - size of padding
        scheme - padding scheme to use, see ell.neural.PaddingScheme
        """
        pad = 0 # default padding
        if 'padding' in self.node.attributes:
            pad = self.node.attributes['padding']

        return {"size": pad, "scheme": ell.neural.PaddingScheme.zeros}

    #===================== helpers ==========================
    def is_tensor(self, id: str):
        return self.converter.is_tensor(id)

    def get_tensor(self, id: str):
        return self.converter.get_tensor(id)

    def add_tensor(self, id: str, tensor):
        return self.converter.add_tensor(id, tensor)

    def get_input_tensors(self):
        tensor_inputs = [x for x in list(self.node.inputs) if self.is_tensor(x)]
        tensors = []
        for tensor_id in tensor_inputs:
            tensor = self.get_tensor(tensor_id)
            t = (tensor_id, tensor[0], tensor[1])
            tensors += [t]
        return tensors

    def is_constant_input(self, node):
        return all(self.is_tensor(x) for x in node.inputs)

    def get_unique_name(self):
        return self.converter.get_unique_name()

    def get_order(self, shape: Tuple):
        return self.converter.get_order(shape)

    def get_input_shapes(self):
        return [self.get_input_shape(id) for id in self.node.inputs]

    def get_input_shape(self, id: str):
        """ return the shape of this input """
        return self.converter.get_input_shape(id)

    def get_node(self, id: str):
        return self.converter.get_node(id)


class OnnxInputConverter(OnnxNodeConverter):
    def __init__(self, converter):
        super().init(converter, "Input")

    def convert(self, input: ValueInfoProto):
        # This converter is different from the rest in that it is converting
        # ValueInfoProto and not NodeProto
        name = Text(input.name)
        if len(name) == 0:
            name = self.get_unique_name()

        # translate the ValueInfoProto to get input node info
        shape = self.get_tensor_type(input.type.tensor_type)
        node = common.converters.ImporterNode(input.name,
            operation_type = self.op_type,
            outputs = [name],
            input_shapes =  [],
            output_shapes = [shape]
        )
        node.prune = False
        return node

    def get_tensor_type(self, tensor_type: TensorProto): 
        # return common importer tensor type tuple derrived from ONNX TensorProto 
        type = tensor_type.elem_type # todo: tensor type should include element type info
        shape = [d.dim_value for d in tensor_type.shape.dim]
        if len(shape) == 4:
            # then this is likely a 'batch' dimension which we don't care about.
            shape.pop(0)
        order = self.get_order(shape)        
        shape = tuple(shape)
        return (shape, order)


class OnnxPlusConverter(OnnxNodeConverter):
    def __init__(self, converter):
        super().init(converter, "Plus")

    def convert(self, node: NodeProto):
        node = super().convert(node)
        node.prune = False
        return node

    def get_attributes(self, attrs: Attributes):
        attributes = {}
        if "axis" in attrs:
            attributes["axis"] = attrs["axis"]
        if "broadcast" in attrs:
            attributes["broadcast"] = attrs["broadcast"]
        return attributes

    def get_output_shapes(self):        
        return [self.node.input_shapes[0]]
        

class OnnxReLuConverter(OnnxNodeConverter):
    def __init__(self, converter):
        super().init(converter, "ReLU")

    def get_attributes(self, attrs: Attributes):
        attributes = {
            'activation': ell.neural.ActivationType.relu
        }
        return attributes

        
class OnnxLeakyReLuConverter(OnnxNodeConverter):
    def __init__(self, converter):
        super().init(converter, "LeakyReLU")

    def get_attributes(self, attrs: Attributes):
        attributes = {
            'activation': ell.neural.ActivationType.leaky,
            "alpha": attrs['alpha']
        }
        return attributes
   

class OnnxElementTimesConverter(OnnxNodeConverter):
    def __init__(self, converter):
        super().init(converter, "ElementTimes")
  

class OnnxConstantConverter(OnnxNodeConverter):
    def __init__(self, converter):
        super().init(converter, "Constant")

    def get_attributes(self, attrs: Attributes):
        attributes = {
            "tensor": attrs['value']
        }
        return attributes
        
    def get_output_shapes(self):
        s = self.node.attributes["tensor"].shape
        if s == ():
            s = (1,)
        return [ (s, self.get_order(s)) ]


class OnnxBiasConverter(OnnxNodeConverter):
    def __init__(self, converter):
        super().init(converter, "Bias")

    def get_weights(self):
        tensors = self.get_input_tensors()
        weights['bias'] = tensors[0]
        return weights


class OnnxPassthroughConverter(OnnxNodeConverter):
    def __init__(self, converter):
        super().init(converter, "Passthrough")


class OnnxConstantFillConverter(OnnxNodeConverter):
    def __init__(self, converter):
        # The operator fills the elements of the output tensor with a constant value specified by the 'value' attribute.      
        super().init(converter, "Skip")
        
    def get_attributes(self, attrs: Attributes):
        attributes = {}
        if "dtype " in attrs:
            attributes["dtype"] = attrs["dtype"]
        else:
            attributes["dtype"] = 1

        if "input_as_shape" in attrs:
            attributes["input_as_shape"] = attrs["input_as_shape"]

        if "shape" in attrs:
            attributes["shape"] = attrs["shape"]

        if "extra_shape" in attrs:
            attributes["extra_shape"] = attrs["extra_shape"]

        if "value" in attrs:
            attributes["value"] = attrs["value"]

        return attributes
        
    def get_output_shapes(self):
        # return the un-transposed shape
        if "input_as_shape" in self.node.attributes:
            id = self.node.attributes["input_as_shape"]
            t = self.converter.get_tensor(str(id))[0]
            shape = t.shape
        elif "shape" in self.node.attributes:
            shape = self.node.attributes["shape"]

        return [(shape, self.get_order(shape))]

class OnnxTransposeConverter(OnnxNodeConverter):
    def __init__(self, converter):
        super().init(converter, "Transpose")

    def convert(self, node: NodeProto):
        node = super().convert(node)
        if self.is_constant_input(node):
            self.transpose_tensor(node)
        else:
            raise Exception("non-constant transpose is not yet supported")
        return node
    
    def transpose_tensor(self, node):
        self.node.operation_type = "Skip"
        input_tensors = self.get_input_tensors()

        if len(node.inputs) != 1:
            raise Exception("Unsupported transpose of multiple inputs on node {}_{}".format(node.operation_type, node.id))
        
        t = input_tensors[0][1]
        axes = node.attributes['perm']

        t2 = np.transpose(t, axes)
        
        # turn the node into a constant tensor
        self.add_tensor(node.id, t2)

        return node
    
    def get_attributes(self, attrs: Attributes):
        attributes = {
            "perm": attrs["perm"]
        }
        return attributes

    def get_output_shapes(self):
        # return the un-transposed shape
        t = self.get_input_tensors()[0][1]
        return [(t.shape, self.get_order(t.shape))]
        

class OnnxUnsqueezeConverter(OnnxNodeConverter):
    def __init__(self, converter):
        super().init(converter, "Passthrough")

    def get_attributes(self, attrs: Attributes):
        attributes = {
            "axes": attrs["axes"]
        }
        return attributes

    def convert(self, node: NodeProto):
        node = super().convert(node)
        if self.is_constant_input(node):
            self.node.operation_type = "Skip"
            self.unsqueeze_tensor(node)
        return node

    def get_output_shapes(self):
        input_shape = self.node.input_shapes[0]
        s = list(input_shape[0])
        axis = self.node.attributes["axes"][0]
        s.insert(axis, 1)
        s = tuple(s)
        return [(s, self.get_order(s))]

    def unsqueeze_tensor(self, node):
        input_tensors = self.get_input_tensors()        
        tensor = input_tensors[0][1]
        axis = node.attributes["axes"][0]
        tensor = np.expand_dims(tensor, axis)
        # turn the node into a constant tensor and the node will be skipped.
        self.add_tensor(node.id, tensor)
        return node

class OnnxSqueezeConverter(OnnxNodeConverter):
    def __init__(self, converter):
        super().init(converter, "Passthrough")

    def convert(self, node: NodeProto):
        node = super().convert(node)
        if self.is_constant_input(node):
            self.node.operation_type = "Skip"
            self.squeeze_tensor(node)
        return node

    def get_attributes(self, attrs: Attributes):
        attributes = {
            "axes": attrs["axes"]
        }
        return attributes

    def get_output_shapes(self):
        node = self.node
        input_shape = node.input_shapes[0]
        s = list(input_shape[0])
        axis = node.attributes["axes"][0]
        s.insert(axis, 1)
        s = tuple(s)
        return [(s, self.get_order(s))]

    def squeeze_tensor(self, node):
        input_tensors = self.get_input_tensors()        
        tensor = input_tensors[0][1]
        axis = self.node.attributes["axes"][0]
        tensor = np.squeeze(tensor, axis)
        # turn the node into a constant tensor and the node will be skipped.
        self.add_tensor(node.id, tensor)
        return node

class OnnxGatherConverter(OnnxNodeConverter):
    def __init__(self, converter):
        super().init(converter, "Passthrough")

    def get_output_shapes(self):
        # Given data tensor of rank r >= 1, and indices tensor of rank q, gather entries of the axis 
        # dimension of data (by default outer-most one as axis=0) indexed by indices, and 
        # concatenates them in an output tensor of rank q + (r - 1). 
        # Example 1: 
        #     data = [ [1.0, 1.2], [2.3, 3.4], [4.5, 5.7], ] 
        #     indices = [ [0, 1], [1, 2], ] 
        #     output = [ [ [1.0, 1.2], [2.3, 3.4], ], [ [2.3, 3.4], [4.5, 5.7], ], ] 
        # Example 2: 
        #     data = [ [1.0, 1.2, 1.9], [2.3, 3.4, 3.9], [4.5, 5.7, 5.9], ] 
        #     indices = [ [0, 2], ] axis = 1, 
        #     output = [ [ [1.0, 1.9], [2.3, 3.9], [4.5, 5.9], ], ]
        input_shape = self.node.input_shapes[0][0]
        indices_shape = self.node.input_shapes[1][0]
        output_shape = tuple(list(indices_shape) + [input_shape[-1]])
        return [(output_shape, self.get_order(output_shape))]


class OnnxFlattenConverter(OnnxNodeConverter):
    def __init__(self, converter):
        super().init(converter, "Passthrough")

    def get_attributes(self, attrs: Attributes):
        attributes = {
            "axes": attrs["axes"]
        }
        return attributes

    def get_output_shapes(self):
        input_shape = self.node.input_shapes[0]
        # Flattens the input tensor into a 2D matrix. If input tensor has shape (d_0, d_1, ... d_n)
        # then the output will have shape (d_0 X d_1 ... d_(axis-1), d_axis X d_(axis+1) ... X dn).
        axis = self.node.attributes['axes'][0]
        s = list(input_shape[0])
        d0 = s[0]
        d_axis = s[axis]
        lower = s[:,axis]
        upper = s[axis+1,:]
        x = d0 * np.product(lower)
        y = d_axis * np.product(upper)
        s = (x,y)
        return [(s, self.get_order(s))]


class OnnxShapeConverter(OnnxNodeConverter):
    def __init__(self, converter):
        super().init(converter, "Passthrough")

    def get_output_shapes(self):
        input_shape = self.node.input_shapes[0]
        # Takes a tensor as input and outputs an 1D int64 tensor containing the shape of the input tensor.
        return [input_shape]


class OnnxSliceConverter(OnnxNodeConverter):
    def __init__(self, converter):
        super().init(converter, "Slice")

    def convert(self, node: NodeProto):
        node = super().convert(node)
        if self.is_constant_input(node):
            self.node.operation_type = "Skip"
            self.slice_tensor(node)
        return node

    def slice_tensor(self, node):
        input_tensors = self.get_input_tensors()
        if len(input_tensors) > 1:
            raise Exception("Unsupported slice of multiple inputs on node {}_{}".format(node.operation_type, node.id))
        axes = node.attributes["axes"]
        if len(axes) > 1:
            raise Exception("Unsupported slice of multiple axes at once on node {}_{}".format(node.operation_type, node.id))
        tensor = input_tensors[0][1]
        axis = axes[0]
        starts = node.attributes["starts"][0]
        ends = node.attributes["ends"][0]
        # chop the tensor t and add it to the global list of tensors
        if axis == 0:
            tensor = tensor[starts:ends,]
        elif axis == 1:
            tensor =tensor[:,starts:ends,]
        elif axis == 2:
            tensor = tensor[:,:,starts:ends,]
        elif axis == 3:
            tensor = tensor[:,:,:,starts:ends,]
        else:
            raise Exception("Unsupported slice of {} dimensiontal tensor in node {}_{}".format(axis, node.operation_type, node.id))

        # turn the node into a constant tensor and the node will be skipped.
        self.add_tensor(node.id, tensor)
        shape = tensor.shape
        node.output_shapes = [ (shape, self.get_order(shape)) ]
        return node
        
    def get_attributes(self, attrs: Attributes):
        attributes = {
            "axes": attrs["axes"],
            "starts": attrs["starts"],
            "ends": attrs["ends"]
        }
        return attributes


class OnnxGRUConverter(OnnxNodeConverter):
    def __init__(self, converter):
        super().init(converter, "GRU")

    def get_attributes(self, attrs: Attributes):
        attributes = {
            "hidden_size": attrs["hidden_size"],
            # pytorch has fixed activation types
            "activation": ell.neural.ActivationType.tanh,
            "recurrent_activation":  ell.neural.ActivationType.sigmoid
        }
        return attributes
        
    def get_output_shapes(self):
        hidden_size = self.node.attributes['hidden_size']
        shape = (hidden_size,)
        return [(shape, "channel")]

    def get_weights(self):
        node = self.node
        units = int(node.attributes['hidden_size'])
        
        tensors = self.get_input_tensors()
        result = {}

        if len(tensors) != 3:
            raise Exception("Expecting 3 weight tensors on GRU node but found {}".format(len(tensors)))

        # stacked set of update, reset, hidden weights to be applied to the input
        stacked_input_weights = tensors[0][1]
        # stacked set of update, reset, hidden weights to be applied to the hidden state
        stacked_hidden_weights = tensors[1][1]
        # stacked set of update, reset, hidden biases
        stacked_bias = tensors[2][1]

        # ONNX order is update, reset, hidden

        # now slice and concatentate what we need for ELL 
        update_weights = np.concatenate([stacked_input_weights[:,0:units,],stacked_hidden_weights[:,0:units,]], 2)
        reset_weights = np.concatenate([stacked_input_weights[:,units:2*units,],stacked_hidden_weights[:,units:2*units,]], 2)
        hidden_weights = np.concatenate([stacked_input_weights[:,2*units:3*units,],stacked_hidden_weights[:,2*units:3*units,]], 2)

        udpate_bias = stacked_bias[:,0:units]
        reset_bias = stacked_bias[:,units:2*units]
        hidden_bias = stacked_bias[:,2*units:3*units]
                
        # bugbug: why is stacked_bias_dim double what we need, seems pytorch is giving us an input bias and a hidden bias
        udpate_bias2 = stacked_bias[:,3*units:4*units]
        reset_bias2 = stacked_bias[:,4*units:5*units]
        hidden_bias2 = stacked_bias[:,5*units:6*units]

        # we have to invent new unique id's for the tensors since we created more than we had in input_tensors.
        unique_id = "{}_{}_".format(node.operation_type, node.id)
        tensors = {
            'update_weights': update_weights,
            'reset_weights': reset_weights,
            'hidden_weights': hidden_weights,
            'update_bias': udpate_bias,
            'reset_bias': reset_bias,
            'hidden_bias': hidden_bias
        }

        # register these as global tensors so the importer can find them.
        for key in tensors:
            t = tensors[key]
            id = unique_id + key
            self.add_tensor(id, t)
            result[key] = (id, t, self.get_order(t))

        return result


class OnnxBatchNormalizationConverter(OnnxNodeConverter):
    def __init__(self, converter):
        super().init(converter, "BatchNormalization")

    def get_weights(self):
        input_node = self.get_node(self.node.inputs[0])
        weights = {}
        tensors = self.get_input_tensors()
        weights['scale'] = tensors[0]
        weights['bias'] = tensors[1]
        weights['mean'] = tensors[2]
        weights['variance'] = tensors[3]
        return weights
        

class OnnxConcatConverter(OnnxNodeConverter):
    def __init__(self, converter):
        super().init(converter, "Splice")

    def convert(self, node: NodeProto):
        node = super().convert(node)
        if self.is_constant_input(node):
            self.node.operation_type = "Skip"
            self.splice_tensor(node)
        return node

    def get_attributes(self, attrs: Attributes):
        attributes = {            
            "dimension_to_stack": "channel",
            "axis": attrs["axis"]
        }
        return attributes
        
    def get_output_shapes(self):
        """ Computes the output shape for a concat/splice operation """
        node = self.node
        dims = []
        
        axis = node.attributes['axis']

        inputs = []
        input_shapes = []
        for id in node.inputs:
            n = self.get_node(id)
            inputs += [id]
            input_shapes += [n.output_shapes[0]]

        if len(inputs) == 0:
            return [((1,),"channel")]
        
        if len(input_shapes) == 1:
            return input_shapes[0]

        # we are sometimes asked to concat something like this:
        #   (1,), (1,1,1,40), (1,)
        # and what the heck does this mean?  So we return the longest dimension and hope for the best...
        same_shapes = True
        max_dim = input_shapes[0][0]
        max_index = 0
        for i, shape in enumerate(input_shapes):
            dim = shape[0]
            if len(dim) > len(max_dim):
                same_shapes = False
                max_dim = dim
                max_index = i

        if not same_shapes:
            # then ignore this node, it is not really a Concat.
            self.node.operation_type = "Skip"
            return max_dim

        # get each input node's output channel to be added together 
        for shape in input_shapes:
            dim = shape[0] # extract shape info from the tuple
            if len(dim) == 0:
                # ignore inputs that have no dimensions (happens with some odd Constants)
                continue
            if len(dims) > 0:
                dim = self.normalize_concat_dimensions(axis, dims[0], dim)
            dims.append(dim)

        # Stack up axis channel:
        shape = list(dims[0])
        shape[axis] = 0
        for d in dims:
            shape[axis] += d[axis]

        shape = tuple(shape)
        order = self.get_order(shape)
        return [(shape, order)]

    def normalize_concat_dimensions(self, axis, first_dim, dim):
        # sometimes we see incompatible shapes in concat, so here we normalize that, but in some cases

        a = list(first_dim)
        b = list(dim)
        a[axis] = 1
        b[axis] = 1

        p1 = np.product(a)
        p2 = np.product(b)
        if p1 != p2:
            # then reshape along the stacked axis to make the non-stacked dimensions the same
            f = int(p2 / p1)  # factor to increase stacked axis by
            a[axis] = dim[axis] * f
            dim = tuple(a)

        return dim

    def splice_tensor(self, node):
        axis = node.attributes['axis']
        add = [t[1] for t in self.get_input_tensors()]
        # concat on the given axis.
        result = np.concatenate(add, axis)
        self.add_tensor(node.id, result)
        s = result.shape
        node.output_shapes = [ (s, self.get_order(s)) ]

class OnnxSoftmaxConverter(OnnxNodeConverter):
    def __init__(self, converter):
        super().init(converter, "Softmax")

    def get_attributes(self, attrs: Attributes):
        attributes = {}
        # onnx has an "axis" attribute, does this matter?
        return attributes


class OnnxReshapeConverter(OnnxNodeConverter):
    def __init__(self, converter):
        super().init(converter, "Reshape")

    def get_output_shapes(self):
        # second input is the tensor that describes how to reshape the input.
        tensor = self.get_tensor(self.node.inputs[1])[0]
        a = np.zeros(self.node.input_shapes[0][0])
        b = a.reshape(tensor)
        s = b.shape
        return [(s, self.get_order(s))]


class OnnxFullyConnectedConverter(OnnxNodeConverter):
    def __init__(self, converter):
        super().init(converter, "FullyConnected")

    def get_attributes(self, attrs: Attributes):
        attributes = {}
        # todo: Gemm nodes have attributes 'alpha', 'beta', 'broadcast', and 'transB'...
        return attributes
        
    def get_output_shapes(self):
        # now compute the output size
        channel = 0 
        if len(self.node.input_shapes) > 2:
            # third input is the output shape
            return [self.node.input_shapes[2]]

        if len(self.node.input_shapes) > 1:
            # second input is is the matrix, and the output size is the first dimension of this matrix.
            output_shape = self.node.input_shapes[1]
            channel = output_shape[0][0]

        dims = (channel, ) # make it a tuple
        return [(dims, 'channel')]

    def get_weights(self):
        input_node = self.get_node(self.node.inputs[0])
        if input_node.operation_type == "Reshape":
            # then we need to find the un-reshaped input shape in order to know how to reshape our weights 
            # correctly.
            input_shape = input_node.input_shapes[0][0]
        else:
            input_shape = self.node.input_shapes[0][0]

        tensor_inputs = self.get_input_tensors()
        
        result = {}
        if len(tensor_inputs) > 0:
            weights = tensor_inputs[0]
            tensor = weights[1]
            tensor_shape = tensor.shape
            tensor_len = len(tensor_shape)
            if tensor_len == 2 :     
                # if the input shape is 3 dimensional as would be the case with the output of a convolutional layer
                # then we have a tensor in the order (channel,row,col) but this is the wrong order for ELL where the
                # input to the FullyConnected layer will be in (row,col,channel) order, so here we have to reorder the
                # weights to match, then reshape it into the 2 dimensional weights that FullyConnected expects where the
                # #rows=#outputs from the layer, and #cols=#inputs to the layer.                
                if len(input_shape) == 3:
                    size = np.product(input_shape)
                    if tensor_shape[0] == size:
                        real_shape = (input_shape[0],input_shape[1],input_shape[2],tensor_shape[1])
                        tensor = np.array(tensor).reshape(real_shape)
                        tensor = np.moveaxis(tensor,0,2) # move channel so it is row,col,channel,filter
                        tensor = tensor.reshape(tensor_shape) # ok, now back to the row-col shape.
                        tensor = tensor.T
                        tensor_shape = tensor.shape
                    else:
                        real_shape = (tensor_shape[0],input_shape[0],input_shape[1],input_shape[2])
                        tensor = np.array(tensor).reshape(real_shape)
                        tensor = np.moveaxis(tensor,1,-1) # move channel so it is filter,row,col,channel
                        tensor = tensor.reshape(tensor_shape) # ok, now back to the row-col shape.
                    # and since we are modifying this tensor, we need to update the version in the master list
                    self.add_tensor(weights[0], tensor)
                weights = (weights[0], tensor, self.get_order(tensor.shape))
                

            result['weights'] = weights
        else:
            raise Exception("fully connected node missing required weights")
        if len(tensor_inputs) > 1:
            result['bias'] = tensor_inputs[1]
        
        return result
            

class ReceptiveFieldConverter (OnnxNodeConverter):   
    def __init__(self, converter):
        super().init(converter, "ReceptiveField")
        self.kernel_shape = None
        self.strides = None
        self.padding = None
        self.dilations = None    

    def get_attributes(self, attrs: Attributes):
        attributes = {}
        # bugbug: need to support rectangular kernel shape and strides
        if 'kernel_shape' in attrs:
            self.kernel_shape = attrs['kernel_shape']
            attributes['size'] = self.kernel_shape[0]
        if 'strides' in attrs:
            self.strides = attrs['strides']
            attributes['stride'] = self.strides[0]
        if 'pads' in attrs:
            self.padding = attrs['pads']
            attributes['padding'] = self.padding[0]
        if 'dilations' in attrs:
            self.dilations = attrs['dilations']
            attributes['dilation'] = self.dilations[0]
        return attributes
    
    def get_padding(self):
        pad = 0 # default padding
        if 'padding' in self.node.attributes:
            pad = self.node.attributes['padding']

        scheme = ell.neural.PaddingScheme.zeros
        if self.op_type == "MaxPooling":
            scheme = ell.neural.PaddingScheme.min
        return {"size": pad, "scheme": scheme}

    def get_output_shapes(self):
        """
        return node output shape:
        """
        node = self.node

        k_row, k_col = self.kernel_shape, self.kernel_shape 
        if isinstance(self.kernel_shape, list):
            k_row, k_col = self.kernel_shape[0], self.kernel_shape[1]

        p_row, p_col = self.padding, self.padding
        if isinstance(self.padding, list):
            p_row, p_col = self.padding[0], self.padding[1]
            
        d_row, d_col = 1, 1
        if isinstance(self.dilations, list):
             d_row, d_col = self.dilations[0], self.dilations[1]
    
        s_row, s_col = self.strides, self.strides
        if isinstance(self.strides, list):
            s_row, s_col = self.strides[0], self.strides[1]

        input_shape = node.input_shapes[0][0] # get shape tuple from list of shapes
        channel, row, col = input_shape
        f = self.get_number_of_output_channels()
        if f is not None:
            channel = f
        
        out_row = (row + 2*p_row - d_row*(k_row - 1) - 1)/s_row + 1
        out_col = (col + 2*p_col - d_col*(k_col - 1) - 1)/s_col + 1

        shape = (channel, int(out_row), int(out_col)) 

        return [(shape, self.get_order(shape))]

    def get_number_of_output_channels(self):
        """
        Subclasses can override this if they generate a different number of channels
        on the output.
        """
        return None
    

class OnnxMaxPoolingConverter(ReceptiveFieldConverter ):
    def __init__(self, converter):
        super(OnnxMaxPoolingConverter, self).__init__(converter)    
        self.op_type = "MaxPooling"


class OnnxAveragePoolingConverter(ReceptiveFieldConverter ):
    def __init__(self, converter):
        super(OnnxAveragePoolingConverter, self).__init__(converter)    
        self.op_type = "AveragePooling"


class OnnxConvolutionConverter(ReceptiveFieldConverter ):
    def __init__(self, converter):
        super(OnnxConvolutionConverter, self).__init__(converter)   
        self.op_type = "Convolution"

    def get_weights(self):
        """ 
        Get the weights of conv2d node and return a dict of weights in the format:
        { 
          'weights' : (weight_index, tensor, 'filter_channel_row_column')
          'bias': (weight_index, tensor, 'channel')
        }
        """
        tensor_inputs = self.get_input_tensors()
        result = {}
        if len(tensor_inputs) > 0:
            result['weights'] = tensor_inputs[0]
        if len(tensor_inputs) > 1:
            result['bias'] = tensor_inputs[1]
        
        return result
        
    def get_number_of_output_channels(self):
        """
        Return the number of filter used for this node operation
        this is needed for the spatial output_shape
        """
        shapes = self.node.input_shapes
        if len(shapes) > 1:
            # the second input is the weight tensor which has the filter info we need
            # for example a shape of (10,1,5,5) indicates 10 filters
            return shapes[1][0][0]
        return None


ONNX_OP_TYPE_TO_CONVERTER_MAP  = {
    "Input"                   : OnnxInputConverter,
    "InputNode"               : OnnxInputConverter,
    # Arithmetic Operations
    "Add"                     : OnnxPlusConverter,
    # Basic neural net functions
    "Conv"                    : OnnxConvolutionConverter,
    "Convolution"             : OnnxConvolutionConverter,
    "GRU"                     : OnnxGRUConverter, # "GRU",
    "Mul"                     : OnnxElementTimesConverter, # "ElementTimes",
    "MatMul"                  : OnnxFullyConnectedConverter, # "FullyConnected",
    "Bias"                    : OnnxBiasConverter, # "Bias",
    "Constant"                : OnnxConstantConverter, # "Constant",
    "ConstantFill"            : OnnxConstantFillConverter, # "ConstantFill",
    "Dropout"                 : OnnxPassthroughConverter, # "Passthrough",
    "Relu"                    : OnnxReLuConverter, # "ReLU",
    "LeakyRelu"               : OnnxLeakyReLuConverter, #"LeakyReLU",
    "ReLU"                    : OnnxReLuConverter, # "ReLU",
    "AveragePool"             : OnnxAveragePoolingConverter, # "AveragePooling",
    "MaxPool"                 : OnnxMaxPoolingConverter, # "MaxPooling",
    "GlobalAveragePool"       : OnnxMaxPoolingConverter, # "Maxpooling", # Temp: not implemented
    "Reshape"                 : OnnxReshapeConverter, # "Reshape",
    "ReduceMean"              : OnnxPassthroughConverter, # "Passthrough",
    "Softmax"                 : OnnxSoftmaxConverter, # "Softmax",
    "LogSoftmax"              : OnnxSoftmaxConverter, # "Softmax", Temp: do we need a logsoftmax here?
    "Gemm"                    : OnnxFullyConnectedConverter, # "FullyConnected",
    "FullyConnected"          : OnnxFullyConnectedConverter, # "FullyConnected",
    "BatchNormalization"      : OnnxBatchNormalizationConverter, # "BatchNormalization",
    # Reshape 
    "Concat"                  : OnnxConcatConverter, # "Splice",
    "Transpose"               : OnnxTransposeConverter, # "Transpose",
    # "Flatten"                 : "Flatten",
    "Gather"                  : OnnxGatherConverter, # "Passthrough",
    "Flatten"                 : OnnxFlattenConverter, # "Passthrough",
    "Slice"                   : OnnxSliceConverter, # "Slice", 
    "Shape"                   : OnnxShapeConverter, # "Passthrough",
    "Passthrough"             : OnnxPassthroughConverter, # "Passthrough",
    "Unsqueeze"               : OnnxUnsqueezeConverter, # "Passthrough", # Temp: not implemented
    "Squeeze"                 : OnnxSqueezeConverter, # "Passthrough", # Temp: Not implemented,
    "ImageScaler"             : "ImageScaler", 
}


class OnnxConverter:
    def __init__(self):
        self.model = None
        self.input_tensors = None
        self.next_name = 0
        self.nodes = []
        self.output_shapes = {} # id of node to ouput_shape

    def _load_onnx(self, path):
        _logger.info("loading the ONNX model from: " + path)
        try:
            start = time.time()
            if isinstance(path, onnx.ModelProto): 
                onnx_model = path
            else:
                onnx_model = ModelProto()

            with open(path, 'rb') as f:
                content = f.read()
                onnx_model.ParseFromString(content)

            end = time.time()
            seconds = end - start
            _logger.info("Loaded ONNX model in {:.3f} seconds.".format(seconds))
            # Check that the IR is well formed
            # onnx.checker.check_model(onnx_model)

            # onnx IR version
            _logger.info("ONNX IR_version {}".format(onnx_model.ir_version))

        except Exception as ex :
            _logger.error("Error occurred when loading onnx model file: " + str(ex))
            raise ex 

        _logger.info("ONNX Graph producer: {} version {}".format(onnx_model.producer_name,
                                                      onnx_model.producer_version))
        _logger.info("ONNX Graph total len: {}".format(len(onnx_model.graph.input)))
        return onnx_model.graph

    def get_optype(self, name):        
        if not name in ONNX_OP_TYPE_TO_CONVERTER_MAP:
            return None
        return ONNX_OP_TYPE_TO_CONVERTER_MAP[name](self).op_type

    def add_tensor(self, id, tensor):
        return self.model.add_tensor(id, tensor, self.get_order(tensor.shape))
     
    def load_model(self, path):
        """ Return a list of ONNX nodes """
        self.model = common.importer.ImporterModel()

        graph = self._load_onnx(path)
        #self.nodes  = utils.ONNX(self.graph).parse_onnx_model()

        input_tensors = {
            t.name: numpy_helper.to_array(t) for t in graph.initializer
        }

        for id in input_tensors:
            self.add_tensor(id, input_tensors[id])
            
        # add input_node first
        for i in graph.input:
            if i.name not in input_tensors:
                inode = ONNX_OP_TYPE_TO_CONVERTER_MAP["Input"](self).convert(i)
                self.add_node(inode)        

        for onnx_node in graph.node:
            # we need to visit the nodes in order such that all "inputs" to each node are
            # processed before this node is processed... fortunately the onnx graph.node
            # list is already sorted topologically.
            node = self.get_converter(onnx_node).convert(onnx_node)
            self.add_node(node)

        return self.model

    def add_node(self, node):
        if node.id in self.model.nodes:
            raise Exception("Already added node '{}'".format(node.id))
            
        if node.prune:
            self.prune_inputs(node)
        else:
            self.define_constant_inputs(node)

        optype = node.operation_type
        if hasattr(node, "onnx_node"):
            optype = node.onnx_node.op_type
        _logger.info("{} {} Inputs {} {} Outputs: {} {} Attributes: {}".format(optype, node.id, node.inputs, node.input_shapes, node.outputs, node.output_shapes, node.attributes))

        self.nodes.append(node)
        self.model.add_node(node.id, node)
        self.output_shapes[node.id] = node.output_shapes[0]

    def prune_inputs(self, node):
        # in ONNX, the inputs also list any tensors that are 'inputs' to this node, but the common
        # importer can't mix node ids and tensor ids in the same list, so we prune out the tensors here.
        inputs = []
        input_shapes = []
        for i, x in enumerate(node.inputs):
            if not self.is_tensor(x) and self.get_node(x).operation_type != 'Skip':
                inputs += [x]
                input_shapes += [ node.input_shapes[i] ]
        node.inputs = inputs
        node.input_shapes = input_shapes

    def define_constant_inputs(self, node):
        for x in node.inputs:
            if self.is_tensor(x):
                t = self.get_tensor(x)[0]
                self.add_constant_node(x, t)

    def add_constant_node(self, name, tensor):
        node = common.converters.ImporterNode( id = name, 
                                operation_type = "Constant",
                                inputs = [],
                                outputs = [name]
                                ) 

        # assuming onnx_node is of type: NodeProto, this performs some common conversion operations
        node.prune          = False
        node.attributes      = { 'tensor': tensor }
        node.input_shapes    = []
        node.output_shapes   = [ (tensor.shape, self.get_order(tensor.shape))]
        self.add_node(node)

    def get_node(self, id):
        if id in self.model.nodes:
            return self.model.nodes[id]
        return None
        
    def get_converter(self, node):
        if node.op_type in ONNX_OP_TYPE_TO_CONVERTER_MAP:
            return ONNX_OP_TYPE_TO_CONVERTER_MAP[node.op_type](self)
        raise Exception("Unimplemented node type: {}".format(node.op_type))

    def get_unique_name(self):
        result = self.next_name        
        self.next_name += 1
        return "_" + str(result)

    def get_input_shape(self, id):
        if id in self.output_shapes:
            return self.output_shapes[id]
        if id in self.model.tensors:
            tensor = self.model.tensors[id]
            return (tensor[0].shape, tensor[1])
        raise Exception("Output shape for {} not found".format(id))

    def is_tensor(self, id):
        return id in self.model.tensors

    def get_tensor(self, id):
        if id in self.model.tensors:
            return self.model.tensors[id]
        if id in self.model.nodes:
            node = self.model.nodes[id]
            if node.operation_type == "Constant":
                tensor = node.attributes["tensor"]
                return (tensor, self.get_order(tensor.shape))
        raise Exception("Input tensor {} not found".format(id))

    def get_order(self, shape):
        tensor_len = len(shape)
        if tensor_len  == 4:
            return 'filter_channel_row_column'
        elif tensor_len == 3 :
            return 'channel_row_column'
        elif tensor_len == 2 :
            return 'row_column'
        else :
            return 'channel'