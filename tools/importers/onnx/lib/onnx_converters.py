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
import sys
import time

import numpy as np

import onnx
from onnx import ModelProto, numpy_helper, ValueInfoProto, AttributeProto, NodeProto, TensorProto  # noqa 401
from typing import Any, Text, Iterable, Dict, Tuple  # noqa 401

sys.path.append(os.path.join(os.path.dirname(os.path.abspath(__file__)), '../../utilities/pythonlibs'))
sys.path.append(os.path.join(os.path.dirname(os.path.abspath(__file__)), '..'))

import find_ell  # noqa 401
import ell
import logger
import common.importer
import common.converters

_logger = logger.get()
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


class OnnxConverter:
    pass  # forward declaration for typing


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
        _logger.info("{}.{}, inputs {} -> outputs {}".format(self.node.op_type, self.node.name, self.node.inputs,
                                                             self.node.outputs))
        _logger.info("    weights: {}".format("".join(["({}: {}{},order='{}')".format(
            w, self.node.weights[w][0], self.node.weights[w][1].shape, self.node.weights[w][2])
            for w in self.node.weights.keys()])))
        _logger.info("    attributes: {}".format(self.node.attribute))
        _logger.info("    padding: {}".format(self.node.padding))

    def convert(self, onnx_node):
        name = Text(onnx_node.name)
        if len(name) == 0:
            # the name must match the output id so we can match this up with other nodes 'inputs' list.
            # bugbug: what if there is more than one output?
            name = onnx_node.output[0]
        node = common.converters.ImporterNode(
            id=name,
            operation_type=self.op_type,
            inputs=list(id for id in onnx_node.input),
            outputs=list(id for id in onnx_node.output),
            metadata={"op_type": self.op_type, "name": name}
        )
        self.node = node

        # assuming onnx_node is of type: NodeProto, this performs some common conversion operations
        onnx_attributes = Attributes.from_onnx(onnx_node.attribute)
        node.attributes = self.get_attributes(onnx_attributes)
        node.onnx_attributes = onnx_attributes
        node.onnx_node = onnx_node
        node.input_shapes = self.get_input_shapes()
        node.output_shapes = self.get_output_shapes()
        node.padding = self.get_padding()
        node.weights = self.get_weights()
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
        pad = 0  # default padding
        if 'padding' in self.node.attributes:
            pad = self.node.attributes['padding']

        return {"size": pad, "scheme": ell.neural.PaddingScheme.zeros}

    # ===================== helpers ==========================
    def is_tensor(self, id: str):
        return self.converter.is_tensor(id)

    def get_tensor(self, id: str):
        return self.converter.get_tensor(id)

    def add_tensor(self, id: str, tensor, order=None):
        return self.converter.add_tensor(id, tensor, order)

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
        result = []
        for id in self.node.inputs:
            if id:
                result += [self.get_input_shape(id)]
            else:
                result += [None]
        return result

    def get_input_shape(self, id: str):
        """ return the shape of this input """
        shape = self.converter.get_input_shape(id)
        # tensorflow onnx is putting a -1 in some places indicating input can be any size
        # but ELL needs a real number, so we choose 1 instead.
        if np.product(shape[0]) < 0:
            reshape = tuple([x if x >= 0 else 1 for x in list(shape[0])])
            shape = (reshape, shape[1])
        return shape

    def get_node(self, id: str):
        return self.converter.get_node(id)

    def add_passthrough_tensors(self):
        tensors = self.get_input_tensors()
        if len(tensors) > 0:
            newname = self.node.id
            # if the input is constant tensor, then register the output as a tensor also, using the
            # new name of this node
            name, tensor, shape = tensors[0]
            self.converter.add_tensor(newname, tensor)

    def reshape_3d_into_2d_tensor(self, input_shape, weight, transpose):
        # if the input shape is 3 dimensional as would be the case with the output of a convolutional layer
        # then we have a tensor in the order (channel,row,col) but this is the wrong order for ELL where the
        # input to the FullyConnected layer will be in (row,col,channel) order, so here we have to reorder the
        # weights to match, then reshape it into the 2 dimensional weights that FullyConnected expects where the
        # #rows=#outputs from the layer, and #cols=#inputs to the layer.
        tensor = weight[1]
        tensor_shape = tensor.shape
        if not transpose:
            real_shape = (input_shape[0], input_shape[1], input_shape[2], tensor_shape[1])
            tensor = np.array(tensor).reshape(real_shape)
            tensor = np.moveaxis(tensor, 0, 2)  # move channel so it is row,col,channel,filter
            tensor = tensor.reshape(tensor_shape)  # ok, now back to the row-col shape.
        else:
            real_shape = (tensor_shape[0], input_shape[0], input_shape[1], input_shape[2])
            tensor = np.array(tensor).reshape(real_shape)
            tensor = np.moveaxis(tensor, 1, -1)  # move channel so it is filter,row,col,channel
            tensor = tensor.reshape(tensor_shape)  # ok, now back to the row-col shape.
            tensor = tensor.T
        # and since we are modifying this tensor, we need to update the version in the master list
        self.add_tensor(weight[0], tensor)
        return tensor

    def remove_input_tensors(self, node):
        # in some cases we manipulate the input tensors and change them, so this is a helper method
        # that stips out the old input tensors that are to be ignored.
        inputs = []
        input_shapes = []
        for i, x in enumerate(node.inputs):
            if not self.is_tensor(x):
                inputs += [x]
                input_shapes += [node.input_shapes[i]]
        node.inputs = inputs
        node.input_shapes = input_shapes


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
        node = common.converters.ImporterNode(
            input.name,
            operation_type=self.op_type,
            outputs=[name],
            input_shapes=[],
            output_shapes=[shape]
        )
        return node

    def get_tensor_type(self, tensor_type: TensorProto):
        # return common importer tensor type tuple derrived from ONNX TensorProto
        # todo: tensor type should include element type info
        shape = [d.dim_value for d in tensor_type.shape.dim]
        if len(shape) == 4:
            # then this is likely a 'batch' dimension which we don't care about.
            shape.pop(0)

        # tensorflow onnx is putting a -1 in some places indicating input can be any size
        # but ELL needs a real number, so we choose 1 instead.
        if np.product(shape) < 0:
            shape = tuple([x if x >= 0 else 1 for x in list(shape)])

        order = self.get_order(shape)
        shape = tuple(shape)
        return (shape, order)


class OnnxCastConverter(OnnxNodeConverter):
    def __init__(self, converter):
        super().init(converter, "Cast")

    def convert(self, node: NodeProto):
        node = super().convert(node)
        if self.is_constant_input(node):
            self.node.operation_type = "Skip"
            self.constant_cast()  # do it!
        return node

    def get_attributes(self, attrs: Attributes):
        attributes = {}
        enum_val = attrs["to"]
        if enum_val == TensorProto.FLOAT or enum_val == TensorProto.FLOAT16:
            # float or float16
            cast_to = ell.nodes.PortType.smallReal
        elif enum_val == TensorProto.DOUBLE:
            cast_to = ell.nodes.PortType.real
        elif enum_val in [TensorProto.INT8, TensorProto.UINT8, TensorProto.INT16, TensorProto.UINT16,
                          TensorProto.INT32, TensorProto.UINT32]:
            # small integer types
            cast_to = ell.nodes.PortType.integer
        elif enum_val == TensorProto.INT64 or enum_val == TensorProto.UINT64:
            cast_to = ell.nodes.PortType.bigInt
        elif enum_val == TensorProto.BOOL:
            cast_to = ell.nodes.PortType.boolean
        elif enum_val == TensorProto.STRING:
            # string
            cast_to = ell.nodes.PortType.categorical
        else:
            raise Exception("Cast type {} is not supported".format(enum_val))
        attributes["cast_to"] = cast_to
        return attributes

    def get_output_shapes(self):
        return [self.node.input_shapes[0]]

    def constant_cast(self):
        for t in self.get_input_tensors():
            t = t[1]
            enum_val = self.node.onnx_attributes["to"]
            if enum_val == TensorProto.FLOAT16:
                t = np.array(t).astype(np.float16)
            if enum_val == TensorProto.FLOAT:
                t = np.array(t).astype(np.float32)
            elif enum_val == TensorProto.DOUBLE:
                t = np.array(t).astype(np.float64)
            elif enum_val == TensorProto.INT8:
                t = np.array(t).astype(np.int8)
            elif enum_val == TensorProto.UINT8:
                t = np.array(t).astype(np.uint8)
            elif enum_val == TensorProto.INT16:
                t = np.array(t).astype(np.int16)
            elif enum_val == TensorProto.UINT16:
                t = np.array(t).astype(np.uint16)
            elif enum_val == TensorProto.INT32:
                t = np.array(t).astype(np.int32)
            elif enum_val == TensorProto.UINT32:
                t = np.array(t).astype(np.uint32)
            elif enum_val == TensorProto.INT64:
                t = np.array(t).astype(np.int64)
            elif enum_val == TensorProto.UINT64:
                t = np.array(t).astype(np.uint64)
            elif enum_val == TensorProto.BOOL:
                t = np.array(t).astype(np.bool)
            elif enum_val == TensorProto.STRING:
                t = np.array(t).astype(np.str)
            else:
                raise Exception("Cast type {} is not supported".format(enum_val))
            self.add_tensor(self.node.id, t)


class OnnxPlusConverter(OnnxNodeConverter):
    def __init__(self, converter):
        super().init(converter, "Plus")

    def convert(self, node: NodeProto):
        node = super().convert(node)
        return node

    def get_attributes(self, attrs: Attributes):
        attributes = {}
        if "axis" in attrs:
            attributes["axis"] = attrs["axis"]
        if "broadcast" in attrs:
            attributes["broadcast"] = attrs["broadcast"]
        return attributes

    def get_output_shapes(self):
        if self.is_tensor(self.node.inputs[0]):
            # first input is a constant, so our output shape is probably the second input
            return [self.node.input_shapes[1]]
        return [self.node.input_shapes[0]]


class OnnxSubtractConverter(OnnxNodeConverter):
    def __init__(self, converter):
        super().init(converter, "Subtract")

    def convert(self, node: NodeProto):
        node = super().convert(node)
        input_tensors = self.get_input_tensors()
        for t in input_tensors:
            tensor = t[1]
            if tensor.shape != self.node.output_shapes[0]:
                # since this is a constant, we can support a simple broadcast operation here to make ELL happy.
                a = np.zeros(self.node.output_shapes[0][0]).astype(tensor.dtype)
                tensor = np.array(tensor) + a
                self.add_tensor(t[0], tensor)  # publish this new version
        return node

    def get_attributes(self, attrs: Attributes):
        attributes = {}
        if "axis" in attrs:
            attributes["axis"] = attrs["axis"]
        if "broadcast" in attrs:
            attributes["broadcast"] = attrs["broadcast"]
        return attributes

    def get_output_shapes(self):
        if self.is_tensor(self.node.inputs[0]):
            # first input is a constant, so our output shape is probably the second input
            return [self.node.input_shapes[1]]
        return [self.node.input_shapes[0]]


class OnnxHardTanhConverter(OnnxNodeConverter):
    def __init__(self, converter):
        super().init(converter, "HardTanh")

    def get_attributes(self, attrs: Attributes):
        attributes = {
            'activation': ell.neural.ActivationType.hardTanh
        }
        return attributes


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


class OnnxMulConverter(OnnxNodeConverter):
    def __init__(self, converter):
        super().init(converter, "ElementwiseMul")

    def convert(self, node: NodeProto):
        node = super().convert(node)
        if self.is_constant_input(node):
            self.node.operation_type = "Skip"
            self.multiply_constants()  # do it!
        return node

    def get_output_shapes(self):
        input_shapes = self.node.input_shapes
        if len(input_shapes) != 2:
            raise Exception("ElementwiseMul requires 2 inputs")
        # output shape depends on behavior of broadcasting.
        if input_shapes[0] == input_shapes[1]:
            return [input_shapes[0]]
        else:
            # use numpy to compute broadcast result
            a = np.zeros(input_shapes[0][0])
            b = np.zeros(input_shapes[1][0])
            if a.shape == () or b.shape == ():
                c = np.array(a * b)
            else:
                c = np.matmul(a, b)
            s = c.shape
            return [(s, self.get_order(s))]

    def multiply_constants(self):
        tensors = self.get_input_tensors()
        a = np.array(tensors[0][1])
        b = np.array(tensors[1][1])
        if a.shape == () or b.shape == ():
            c = np.array(a * b)
        else:
            c = np.matmul(a, b)
        self.add_tensor(self.node.id, c)


class OnnxDivConverter(OnnxNodeConverter):
    def __init__(self, converter):
        super().init(converter, "ElementwiseDiv")

    def convert(self, node: NodeProto):
        node = super().convert(node)
        if self.is_constant_input(node):
            self.node.operation_type = "Skip"
            self.divide_constants()  # do it!
        return node

    def get_output_shapes(self):
        input_shapes = self.node.input_shapes
        if len(input_shapes) != 2:
            raise Exception("ElementwiseDiv requires 2 inputs")
        # output shape depends on behavior of broadcasting.
        if input_shapes[0] == input_shapes[1]:
            return [input_shapes[0]]
        else:
            # use numpy to compute broadcast result
            a = np.zeros(input_shapes[0][0])
            b = np.ones(input_shapes[1][0])
            if a.shape == () or b.shape == ():
                c = np.array(a / b)
            elif(a.shape == b.shape):
                c = np.true_divide(a, b)
            else:
                raise Exception("ElementwiseDiv requires both inputs with same dimentions")
            s = c.shape
            return [(s, self.get_order(s))]

    def divide_constants(self):
        tensors = self.get_input_tensors()
        a = np.array(tensors[0][1])
        b = np.array(tensors[1][1])
        if a.shape == () or b.shape == ():
            c = np.array(a / b)
        elif a.shape == b.shape:
            c = np.true_divide(a, b)
        else:
            raise Exception("ElementwiseDiv requires both inputs with same dimentions")
        self.add_tensor(self.node.id, c)


class OnnxConstantConverter(OnnxNodeConverter):
    def __init__(self, converter):
        super().init(converter, "Constant")

    def convert(self, node: NodeProto):
        node = super().convert(node)
        self.add_tensor(node.id, node.attributes["tensor"])
        return node

    def get_attributes(self, attrs: Attributes):
        attributes = {
            "tensor": attrs['value']
        }
        return attributes

    def get_output_shapes(self):
        s = self.node.attributes["tensor"].shape
        if s == ():
            s = (1,)
        return [(s, self.get_order(s))]


class OnnxBiasConverter(OnnxNodeConverter):
    def __init__(self, converter):
        super().init(converter, "Bias")

    def get_weights(self):
        tensors = self.get_input_tensors()
        weights = {}
        weights['bias'] = tensors[0]
        return weights


class OnnxPassthroughConverter(OnnxNodeConverter):
    def __init__(self, converter):
        super().init(converter, "Passthrough")

    def get_weights(self):
        self.add_passthrough_tensors()
        return {}


class OnnxSkipConverter(OnnxNodeConverter):
    def __init__(self, converter):
        super().init(converter, "Skip")

    def get_weights(self):
        return {}


class OnnxIdentityConverter(OnnxNodeConverter):
    def __init__(self, converter):
        super().init(converter, "Passthrough")  # Identity is a just a passthrough node
        # it has to be Passthrough and not Skip because the input has to be renamed in the process
        # so that we can satisfy dependencies that are expecting this node.id to be available.

    def get_weights(self):
        self.add_passthrough_tensors()
        return {}


class OnnxAtenConverter(OnnxNodeConverter):
    def __init__(self, converter):
        # Initially, make this a passthrough node.
        super().init(converter, "Passthrough")

    def is_equivalent_to_sign_function(self, node: NodeProto):
        op_is_greater = False
        operand_is_zero = False
        arg2_is_one = False
        arg3_is_minus_one = False

        try:
            # The sign function is equivalent to the "where" using
            # the > operator with operand 0, with input tensors of 1 and -1
            if node.attribute[0].s == b"where":
                arg1 = self.converter.model.nodes[node.input[0]]
                arg2 = self.converter.model.nodes[node.input[1]]
                arg3 = self.converter.model.nodes[node.input[2]]
                if arg1.onnx_node.op_type == "Greater":
                    op_is_greater = True
                    operand = self.converter.model.tensors[arg1.inputs[1]]
                    if operand[0] == [0]:
                        operand_is_zero = True
                if arg2.onnx_node.op_type == "ConstantFill" and arg2.attributes["value"] == 1:
                    arg2_is_one = True
                if arg3.onnx_node.op_type == "ConstantFill" and arg3.attributes["value"] == -1:
                    arg3_is_minus_one = True
        except Exception:
            return False

        if op_is_greater and operand_is_zero and arg2_is_one and arg3_is_minus_one:
            return True

        return False

    def convert(self, node: NodeProto):
        # Check whether this is equivalent to the Sign function
        if self.is_equivalent_to_sign_function(node):
            self.op_type = "Sign"

        node = super().convert(node)
        # Adjust inputs
        self.node.inputs = [self.node.inputs[0]]
        self.node.input_shapes = [self.node.input_shapes[0]]

        return node


class OnnxConstantFillConverter(OnnxNodeConverter):
    def __init__(self, converter):
        # The operator fills the elements of the output tensor with a constant value specified by the 'value' attribute.
        # This node type is now obsolete, see:
        # https://github.com/onnx/onnx/pull/1434/commits/d01874377b311120cc27bd459b2a51f8991c3fdc
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
        else:
            attributes["value"] = 0

        return attributes

    def convert(self, node: NodeProto):
        node = super().convert(node)
        if self.is_constant_input(node):
            self.node.operation_type = "Skip"
            self.constant_fill(node)  # do it!
        return node

    def get_output_shapes(self):
        # return the un-transposed shape
        shape = self.get_shape(self.node)
        return [(shape, self.get_order(shape))]

    def constant_fill(self, node):
        shape = self.get_shape(node)
        value = 0
        if "value" in node.attributes:
            value = float(node.attributes["value"])
        t = np.ones(shape) * value
        self.add_tensor(node.id, t)

    def get_shape(self, node):
        if "input_as_shape" in node.attributes:
            id = node.attributes["input_as_shape"]
            if len(node.inputs) > 0:
                id = node.inputs[0]
            t = self.converter.get_tensor(str(id))[0]
            shape = tuple(t)
        elif "shape" in node.attributes:
            shape = tuple(node.attributes["shape"])
        else:
            raise Exception("This version of obsolete ConstantFill is not implemented")
        return tuple(shape)


class OnnxTransposeConverter(OnnxNodeConverter):
    def __init__(self, converter):
        super().init(converter, "Reorder")

    def convert(self, node: NodeProto):
        node = super().convert(node)
        if self.is_constant_input(node):
            self.transpose_tensor(node)
        return node

    def transpose_tensor(self, node):
        self.node.operation_type = "Skip"
        input_tensors = self.get_input_tensors()

        if len(node.inputs) != 1:
            raise Exception("Unsupported transpose of multiple inputs on node {}_{}".format(
                node.operation_type, node.id))

        t = input_tensors[0][1]
        axes = node.attributes['order']

        t2 = np.transpose(t, axes)

        # turn the node into a constant tensor
        self.add_tensor(node.id, t2)

        return node

    def get_attributes(self, attrs: Attributes):
        attributes = {
            "order": attrs["perm"]
        }
        return attributes

    def get_output_shapes(self):
        t = self.node.input_shapes[0][0]
        test = np.zeros(t)
        axes = self.node.attributes['order']
        t2 = np.transpose(test, axes)
        return [(t2.shape, self.get_order(t2.shape))]


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
        axes = list(self.node.attributes["axes"])
        axes.sort(reverse=True)
        for axis in axes:
            s.insert(axis, 1)
        s = tuple(s)
        return [(s, self.get_order(s))]

    def unsqueeze_tensor(self, node):
        input_tensors = self.get_input_tensors()
        tensor = input_tensors[0][1]
        axes = list(node.attributes["axes"])
        axes.sort(reverse=True)
        for axis in axes:
            tensor = np.expand_dims(tensor, axis)
        # turn the node into a constant tensor and the node will be skipped.
        self.add_tensor(node.id, tensor)
        s = tensor.shape
        self.node.output_shapes = [(s, self.get_order(s))]
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
        # numpy squeeze only works on a dimension whose size is 1.
        if len(s) > axis and s[axis] == 1:
            del s[axis]
        s = tuple(s)
        return [(s, self.get_order(s))]

    def squeeze_tensor(self, node):
        input_tensors = self.get_input_tensors()
        tensor = input_tensors[0][1]
        axis = self.node.attributes["axes"][0]
        # Only squeeze along axis whose dimension is 1
        if tensor.shape[axis] == 1:
            tensor = np.squeeze(tensor, axis)
        # turn the node into a constant tensor and the node will be skipped.
        self.add_tensor(node.id, tensor)
        return node


class OnnxGatherConverter(OnnxNodeConverter):
    def __init__(self, converter):
        super().init(converter, "Passthrough")

    def get_attributes(self, attrs: Attributes):
        attributes = {
        }
        if "axis" in attrs:
            attributes["axis"] = attrs["axis"]
        else:
            attributes["axis"] = 0

        return attributes

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

        if self.is_constant_input(self.node):
            t = self.gather_tensor(self.node)  # actually do it...
            return [(t.shape, self.get_order(t.shape))]

        return [(output_shape, self.get_order(output_shape))]

    def gather_tensor(self, node):
        # turn the node into a constant tensor and the node will be skipped.
        self.node.operation_type = "Skip"
        dim = self.node.attributes["axis"]
        input_tensors = self.get_input_tensors()
        tensor = input_tensors[0][1]
        indices = input_tensors[1][1]

        if len(tensor.shape) == 1:
            # then we are picking a single value out of a vector
            if len(indices.shape) == 0:
                index = int(indices)
            else:
                index = indices[0]
            if index >= len(tensor):
                v = 0  # out of bounds
            else:
                v = tensor[index]
            t = np.array(v)
            self.add_tensor(node.id, t)
        else:
            import torch
            tensor = torch.tensor(tensor)
            indices = torch.tensor(indices).long()
            g = torch.gather(tensor, dim, indices)
            t = g.numpy()
            self.add_tensor(node.id, t)
        return t


class OnnxFlattenConverter(OnnxNodeConverter):
    def __init__(self, converter):
        super().init(converter, "Passthrough")

    def get_attributes(self, attrs: Attributes):
        attributes = {
            "axis": attrs.get("axis", 1)
        }
        return attributes

    def get_output_shapes(self):
        input_shape = self.node.input_shapes[0]
        # Flattens the input tensor into a 2D matrix. If input tensor has shape (d_0, d_1, ... d_n)
        # then the output will have shape (d_0 X d_1 ... d_(axis-1), d_axis X d_(axis+1) ... X dn).
        axis = self.node.attributes["axis"]
        s = list(input_shape[0])
        d_axis = s[axis]
        lower = s[:axis]
        upper = s[axis + 1:]
        x = np.product(lower)
        y = d_axis * np.product(upper)
        s = (int(x), int(y))
        return [(s, self.get_order(s))]


class OnnxShapeConverter(OnnxNodeConverter):
    def __init__(self, converter):
        super().init(converter, "Passthrough")

    def get_output_shapes(self):
        input_shape = self.node.input_shapes[0][0]
        self.add_tensor(self.node.outputs[0], np.array(input_shape))
        # Since this shape is known now, it can also be treated as a constant.
        return [(input_shape, self.get_order(input_shape))]


class OnnxConstantOfShapeConverter(OnnxNodeConverter):
    def __init__(self, converter):
        super().init(converter, "Passthrough")

    def get_output_shapes(self):
        input_shape = self.get_tensor(self.node.inputs[0])[0]
        self.add_tensor(self.node.outputs[0], np.zeros(input_shape))
        # Since this shape is known now, it can also be treated as a constant.
        return [(input_shape, self.get_order(input_shape))]


class OnnxSliceConverter(OnnxNodeConverter):

    def __init__(self, converter):
        super().init(converter, "Slice")
        self.INFINITY = 9223372036854775807

    def convert(self, node: NodeProto):
        node = super().convert(node)
        if self.is_constant_input(node):
            self.node.operation_type = "Skip"
            self.slice_tensor(node)
        return node

    def get_output_shapes(self):
        node = self.node
        input_shapes = node.input_shapes
        if len(input_shapes) > 1:
            raise Exception("Unsupported slice of multiple inputs on node {}_{}".format(
                node.operation_type, node.id))
        axes = self.node.attributes["axes"]
        for i in range(len(axes)):
            axis = axes[i]
            shape = list(input_shapes[0][0])
            starts = node.attributes["starts"][i]
            ends = node.attributes["ends"][i]
            if ends == self.INFINITY:
                shape[axis] -= starts
            else:
                shape[axis] = ends - starts
        shape = tuple(shape)
        order = self.get_order(shape)
        return [(shape, order)]

    def slice_tensor(self, node):
        input_tensors = self.get_input_tensors()
        if len(input_tensors) > 1:
            raise Exception("Unsupported slice of multiple inputs on node {}_{}".format(
                node.operation_type, node.id))
        axes = node.attributes["axes"]
        tensor = input_tensors[0][1]
        for i in range(len(axes)):
            axis = axes[i]
            starts = node.attributes["starts"][i]
            ends = node.attributes["ends"][i]
            # chop the tensor t and add it to the global list of tensors
            if axis == 0:
                if ends == self.INFINITY:
                    tensor = tensor[starts:, ]
                else:
                    tensor = tensor[starts:ends, ]
            elif axis == 1:
                if ends == self.INFINITY:
                    tensor = tensor[:, starts:, ]
                else:
                    tensor = tensor[:, starts:ends, ]
            elif axis == 2:
                if ends == self.INFINITY:
                    tensor = tensor[:, :, starts:, ]
                else:
                    tensor = tensor[:, :, starts:ends, ]
            elif axis == 3:
                if ends == self.INFINITY:
                    tensor = tensor[:, :, :, starts:ends, ]
                else:
                    tensor = tensor[:, :, :, starts:ends, ]
            else:
                raise Exception("Unsupported slice of {} dimensional tensor in node {}_{}".format(
                    axis, node.operation_type, node.id))

        # turn the node into a constant tensor and the node will be skipped.
        self.add_tensor(node.id, tensor)
        shape = tensor.shape
        node.output_shapes = [(shape, self.get_order(shape))]
        return node

    def get_attributes(self, attrs: Attributes):
        attributes = {
            "axes": attrs["axes"],
            "starts": attrs["starts"],
            "ends": attrs["ends"]
        }
        return attributes


class OnnxLSTMConverter(OnnxNodeConverter):
    def __init__(self, converter):
        super().init(converter, "LSTM")

    def get_attributes(self, attrs: Attributes):
        attributes = {
            "hidden_size": attrs["hidden_size"],
            # pytorch has fixed activation types
            "activation": ell.neural.ActivationType.tanh,
            "recurrent_activation": ell.neural.ActivationType.sigmoid,
            "num_directions": 1
        }
        if "direction" in attrs:
            attributes["direction"] = attrs["direction"].decode()
            if attributes["direction"] == "bidirectional":
                attributes["num_directions"] = 2
        return attributes

    def get_output_shapes(self):
        hidden_size = self.node.attributes['hidden_size']
        num_directions = self.node.attributes["num_directions"]
        seq_length, batch_size, input_size = self.node.input_shapes[0][0]
        if batch_size > 1:
            raise Exception("ELL only supports a batch size of 1")
        self.node.attributes["sequence_length"] = seq_length
        Y_shape = ((seq_length, num_directions, 1, hidden_size), "seq-length,num-directions,batch_size,hidden-size")
        Y_h_shape = ((num_directions, 1, hidden_size), "num-directions,batch_size,hidden-size")
        Y_c_shape = Y_h_shape
        # todo: we are not currently outputing Y_h and Y_c.
        return [Y_shape, Y_h_shape, Y_c_shape]

    def reform_weights(self, weights, axis):
        # onnx layer order is: input, output, forget, cell.
        # but ELL wants:       input, forget, cell, output.
        parts = np.split(weights, 4, axis)
        return np.concatenate((parts[0], parts[2], parts[3], parts[1]), axis)

    def get_weights(self):
        node = self.node
        units = int(node.attributes['hidden_size'])
        num_directions = self.node.attributes["num_directions"]

        tensors = self.get_input_tensors()
        result = {}

        if len(tensors) < 3:
            raise Exception("Expecting 2 weight tensors and a bias tensor on LSTM node but found {}".format(
                len(tensors)))

        # stacked set of (input, forget, cell, output) weights to be applied to the input
        stacked_input_weights = self.reform_weights(tensors[0][1], axis=1)
        # stacked set of (input, forget, cell, output) weights to be applied to the hidden state
        stacked_hidden_weights = self.reform_weights(tensors[1][1], axis=1)
        # the stacked bias comes in as one tensor which we have to then split
        size = units * 4 * num_directions
        bias_weights = tensors[2][1].reshape((2, size))
        # stacked set of (input, forget, cell, output) input biases
        stacked_input_bias = self.reform_weights(bias_weights[0], axis=0)
        # stacked set of (input, forget, cell, output) hidden biases
        stacked_hidden_bias = self.reform_weights(bias_weights[1], axis=0)

        # we have to invent new unique id's for the tensors since we created more than we had in input_tensors.
        unique_id = "{}_{}_".format(node.operation_type, node.id)
        tensors = {
            'input_weights': stacked_input_weights,
            'hidden_weights': stacked_hidden_weights,
            'input_bias': stacked_input_bias,
            'hidden_bias': stacked_hidden_bias
        }

        # register these as global tensors so the importer can find them.
        for key in tensors:
            t = tensors[key]
            id = unique_id + key
            self.add_tensor(id, t)
            result[key] = (id, t, self.get_order(t))

        # remove all the old inputs since we have reshaped them and created new tensors for them, the only input
        # that survives is the input buffer.
        self.remove_input_tensors(node)

        return result


class OnnxFastGRNNConverter(OnnxNodeConverter):
    def __init__(self, converter):
        super().init(converter, "FastGRNN")

    def get_nonlinearity(self, nonlinearity):
        if nonlinearity == "sigmoid":
            return ell.neural.ActivationType.sigmoid
        elif nonlinearity == "tanh":
            return ell.neural.ActivationType.tanh
        # we need to add these activations
        elif nonlinearity == "hardSigmoid":
            return ell.neural.ActivationType.hardSigmoid
        elif nonlinearity == "hardTanh":
            return ell.neural.ActivationType.hardTanh

    def get_attributes(self, attrs: Attributes):
        attributes = {
            "hidden_size": attrs["hidden_size"],
            "gate_nonlinearity": self.get_nonlinearity(attrs["gate_nonlinearity"].decode()),
            "update_nonlinearity": self.get_nonlinearity(attrs["update_nonlinearity"].decode()),
            "num_directions": 1
        }
        if "wRank" in attrs:
            attributes["wRank"] = attrs["wRank"]
        else:
            attributes["wRank"] = 0
        if "uRank" in attrs:
            attributes["uRank"] = attrs["uRank"]
        else:
            attributes["uRank"] = 0
        if "direction" in attrs:
            attributes["direction"] = attrs["direction"].decode()
            if attributes["direction"] == "bidirectional":
                attributes["num_directions"] = 2
        return attributes

    def get_output_shapes(self):
        hidden_size = self.node.attributes['hidden_size']
        num_directions = self.node.attributes["num_directions"]
        input_shape = self.node.input_shapes[0][0]
        if len(input_shape) == 3:
            seq_length, batch_size, input_size = input_shape
        elif len(input_shape) == 4:
            seq_length, num_directions, batch_size, input_size = input_shape
        if batch_size > 1:
            raise Exception("ELL only supports a batch size of 1")
        self.node.attributes["sequence_length"] = seq_length
        Y_shape = ((seq_length, num_directions, 1, hidden_size), "seq-length,num-directions,batch_size,hidden-size")
        Y_h_shape = ((num_directions, 1, hidden_size), "num_directions,batch_size,hidden_size")
        return [Y_shape, Y_h_shape]

    def sigmoid(self, x):
        return 1 / (1 + np.exp(-x))

    def get_weights(self):
        node = self.node
        tensors = self.get_input_tensors()
        result = {}

        if len(tensors) < 6:
            raise Exception("Expecting 6 weight tensors on FastGRNN node but found {}".format(len(tensors)))

        # weights to be applied to the input, transformed so it can be used in GEMV operation (W * input)
        i = 0
        W1 = tensors[i][1]
        i += 1
        if node.attributes["wRank"] != 0:
            W2 = tensors[i][1]
            i += 1
        else:
            W2 = np.array([[0]])
        # weights to be applied to the hidden state, transformed so it can be used in GEMV operation.
        U1 = tensors[i][1]
        i += 1
        if node.attributes["uRank"] != 0:
            U2 = tensors[i][1]
            i += 1
        else:
            U2 = np.array([[0]])
        # the stacked bias comes in as one tensor which we have to then split
        bias_gate = tensors[i][1]
        i += 1
        # input biases
        bias_update = tensors[i][1]
        i += 1
        # hidden biases
        zeta = self.sigmoid(tensors[i][1])
        i += 1
        nu = self.sigmoid(tensors[i][1])

        # ONNX order is update, reset, hidden

        # we have to invent new unique id's for the tensors since we created more than we had in input_tensors.
        unique_id = "{}_{}_".format(node.operation_type, node.id)
        tensors = {
            'W1': W1,
            'W2': W2,
            'U1': U1,
            'U2': U2,
            'bias_gate': bias_gate,
            'bias_update': bias_update,
            'zeta': zeta,
            'nu': nu
        }

        # register these as global tensors so the importer can find them.
        for key in tensors:
            t = tensors[key]
            id = unique_id + key
            self.add_tensor(id, t)
            result[key] = (id, t, self.get_order(t))

        # remove all the old inputs since we have reshaped them and created new tensors for them, the only input
        # that survives is the input buffer.
        self.remove_input_tensors(node)

        return result


class OnnxGRUConverter(OnnxNodeConverter):
    def __init__(self, converter):
        super().init(converter, "GRU")

    def get_attributes(self, attrs: Attributes):
        attributes = {
            "hidden_size": attrs["hidden_size"],
            # pytorch has fixed activation types
            "activation": ell.neural.ActivationType.tanh,
            "recurrent_activation": ell.neural.ActivationType.sigmoid,
            "num_directions": 1
        }
        if "direction" in attrs:
            attributes["direction"] = attrs["direction"].decode()
            if attributes["direction"] == "bidirectional":
                attributes["num_directions"] = 2
        return attributes

    def get_output_shapes(self):
        hidden_size = self.node.attributes['hidden_size']
        num_directions = self.node.attributes["num_directions"]
        seq_length, batch_size, input_size = self.node.input_shapes[0][0]
        if batch_size > 1:
            raise Exception("ELL only supports a batch size of 1")
        self.node.attributes["sequence_length"] = seq_length
        Y_shape = ((seq_length, num_directions, 1, hidden_size), "seq-length,num-directions,batch_size,hidden-size")
        Y_h_shape = ((num_directions, 1, hidden_size), "num_directions,batch_size,hidden_size")
        return [Y_shape, Y_h_shape]

    def get_weights(self):
        node = self.node
        units = int(node.attributes['hidden_size'])
        num_directions = self.node.attributes["num_directions"]

        tensors = self.get_input_tensors()
        result = {}

        if len(tensors) < 3:
            raise Exception("Expecting 3 weight tensors on GRU node but found {}".format(len(tensors)))

        # note: there is a 4th tensor for "initial_h" which is the initial value for hidden state, but
        # ELL doesn't support setting hidden state this way.

        # stacked set of update, reset, hidden weights to be applied to the input
        stacked_input_weights = tensors[0][1]
        # stacked set of update, reset, hidden weights to be applied to the hidden state
        stacked_hidden_weights = tensors[1][1]
        size = units * 3 * num_directions
        # the stacked bias comes in as one tensor which we have to then split
        bias_weights = tensors[2][1].reshape((2, size))
        # stacked set of update, reset, hidden input biases
        stacked_input_bias = bias_weights[0]
        # stacked set of update, reset, hidden hidden biases
        stacked_hidden_bias = bias_weights[1]

        # ONNX order is update, reset, hidden

        # we have to invent new unique id's for the tensors since we created more than we had in input_tensors.
        unique_id = "{}_{}_".format(node.operation_type, node.id)
        tensors = {
            'input_weights': stacked_input_weights,
            'hidden_weights': stacked_hidden_weights,
            'input_bias': stacked_input_bias,
            'hidden_bias': stacked_hidden_bias
        }

        # register these as global tensors so the importer can find them.
        for key in tensors:
            t = tensors[key]
            id = unique_id + key
            self.add_tensor(id, t)
            result[key] = (id, t, self.get_order(t))

        # remove all the old inputs since we have reshaped them and created new tensors for them, the only input
        # that survives is the input buffer.
        self.remove_input_tensors(node)

        return result


class OnnxBatchNormalizationConverter(OnnxNodeConverter):
    def __init__(self, converter):
        super().init(converter, "BatchNormalization")

    def get_weights(self):
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
            self.splice_constant(node)

        # we might be concatenating data with constant tensors, and in this case we need to remove the
        # input tensors.  bugbug: need to fix this mess, the common importer needs to handle input tensors
        # properly.
        self.remove_input_tensors(node)
        return node

    def get_attributes(self, attrs: Attributes):
        # The attrs["axis"] includes the batch dimension as 0, so remove it
        axis = max(0, attrs["axis"] - 1)
        attributes = {
            "dimension_to_stack": "channel",
            "axis": axis
        }
        return attributes

    def get_output_shapes(self):
        """ Computes the output shape for a concat/splice operation """
        node = self.node
        dims = []

        axis = node.attributes["axis"]

        inputs = []
        input_shapes = []
        for id in node.inputs:
            n = self.get_node(id)
            inputs += [id]
            input_shapes += [n.output_shapes[0]]

        if len(inputs) == 0:
            return [((1,), "channel")]

        if len(input_shapes) == 1:
            return [input_shapes[0]]

        # we are sometimes asked to concat something like this:
        #   (1,), (1,1,1,40), (1,)
        # and what the heck does this mean?  So we return the longest dimension and hope for the best...
        same_shapes = True
        max_dim = input_shapes[0][0]
        for i, shape in enumerate(input_shapes):
            dim = shape[0]
            if len(dim) > len(max_dim):
                same_shapes = False
                max_dim = dim

        if not same_shapes:
            # then ignore this node, it is not really a Concat.
            _logger.info("### ignoring strange concatenation of shapes: {}".format([s[0] for s in input_shapes]))
            self.node.operation_type = "Skip"
            return max_dim

        # get each input node's output channel to be added together
        for shape in input_shapes:
            dim = shape[0]  # extract shape info from the tuple
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

    def splice_constant(self, node):
        axis = node.attributes['axis']
        add = [t[1] for t in self.get_input_tensors()]
        # concat on the given axis.
        result = np.concatenate(add, axis)
        self.add_tensor(node.id, result)
        s = result.shape
        node.output_shapes = [(s, self.get_order(s))]


class OnnxSoftmaxConverter(OnnxNodeConverter):
    def __init__(self, converter):
        super().init(converter, "Softmax")

    def get_attributes(self, attrs: Attributes):
        attributes = {}
        # onnx has an "axis" attribute, does this matter?
        return attributes


class OnnxSigmoidConverter(OnnxNodeConverter):
    def __init__(self, converter):
        super().init(converter, "Sigmoid")


class OnnxAbsConverter(OnnxNodeConverter):
    def __init__(self, converter):
        super().init(converter, "Abs")


class OnnxHardSigmoidConverter(OnnxNodeConverter):
    def __init__(self, converter):
        super().init(converter, "HardSigmoid")


class OnnxSqrtConverter(OnnxNodeConverter):
    def __init__(self, converter):
        super().init(converter, "Sqrt")


class OnnxCosConverter(OnnxNodeConverter):
    def __init__(self, converter):
        super().init(converter, "Cos")


class OnnxSinConverter(OnnxNodeConverter):
    def __init__(self, converter):
        super().init(converter, "Sin")


class OnnxTanhConverter(OnnxNodeConverter):
    def __init__(self, converter):
        super().init(converter, "Tanh")


class OnnxExpConverter(OnnxNodeConverter):
    def __init__(self, converter):
        super().init(converter, "Exp")


class OnnxLogConverter(OnnxNodeConverter):
    def __init__(self, converter):
        super().init(converter, "Log")


class OnnxReshapeConverter(OnnxNodeConverter):
    def __init__(self, converter):
        super().init(converter, "Reshape")

    def convert(self, onnx_node):
        node = super().convert(onnx_node)
        self.remove_input_tensors(node)
        return node

    def get_output_shapes(self):
        # second input is the tensor that describes how to reshape the input.
        tensor = self.get_tensor(self.node.inputs[1])[0]
        input_shape = self.node.input_shapes[0][0]
        shape = list(tensor)
        minus_one = None
        for i in range(len(shape)):
            if shape[i] == 0:
                # A dimension could also be 0, in which case the actual dimension value is unchanged
                # (i.e. taken from the input tensor).
                shape[i] = input_shape[i]
            elif shape[i] == -1:
                # At most one dimension of the new shape can be -1. In this case, the value is inferred from the size
                # of the tensor and the remaining dimensions.
                shape[i] = 1
                minus_one = i
        if minus_one is not None:
            # remainder goes into the -1 dimension
            shape[minus_one] = int(np.product(input_shape) / np.product(shape))

        shape = tuple(shape)
        return [(shape, self.get_order(shape))]


class OnnxMatMulConverter(OnnxNodeConverter):
    def __init__(self, converter):
        super().init(converter, "FullyConnected")

    def get_attributes(self, attrs: Attributes):
        attributes = {"transpose": False}
        return attributes

    def squeeze(self, shape):
        # remove dimensions of size 1, since ELL can easily reinterpret those away.
        squeezed = [x for x in list(shape) if x != 1]
        return tuple(squeezed)

    def get_2d_shape(self, shape):
        shape = self.squeeze(shape)
        if len(shape) == 1:
            return (1, shape[0])
        elif len(shape) == 2:
            return shape
        elif len(shape) == 3:
            raise Exception("3d shape not supported yet")

    def get_input_shapes(self):
        result = []
        for id in self.node.inputs:
            if id:
                # re-interpret the input shape as a 2d matrix dimension.
                t = self.get_input_shape(id)
                shape = self.get_2d_shape(t[0])
                result += [(shape, self.get_order(shape))]
            else:
                result += [None]
        return result

    def get_output_shapes(self):
        # now compute the output size
        if len(self.node.input_shapes) > 2:
            # third input is the output shape
            return [self.node.input_shapes[2]]

        if len(self.node.input_shapes) == 2:
            n, m1 = self.node.input_shapes[0][0]
            m2, p = self.node.input_shapes[1][0]

            if m1 == m2:
                pass  # good
            elif m1 == p:
                # hmmm, then we need to transpose our weights...
                self.node.attributes["transpose"] = True
                m2, p = p, m2
            else:
                raise Exception("incompatible sizes in MatMul operation ({},{}) x ({},{})".format(n, m1, m2, p))

            result = (n, p)

        else:
            raise Exception("FullyConnected node is expecting two inputs, but we have {}".format(
                len(self.node.input_shapes)))

        return [(result, self.get_order(result))]

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
            if tensor_len == 2:

                if len(input_shape) == 3:
                    tensor = self.reshape_3d_into_2d_tensor()
                    tensor_shape = tensor.shape

                elif "transpose" in self.node.attributes:
                    # then the weights need to be transposed.
                    tensor = tensor.T
                    tensor_shape = tensor.shape
                    self.add_tensor(weights[0], tensor)  # re-register the transposed tensor.

                # Now, ELL flattens the input shape and the FullyConnectedLayerNode multiples in the opposite order
                # from what you'd expect doing "weight * input", not "input * weight" so we may need to transpose here
                # again to account for this.
                input_shape = (np.product(input_shape), 1)
                if tensor_shape[1] == input_shape[0]:  # make sure we have m*n and n*k.
                    pass
                elif tensor_shape[0] == input_shape[0]:
                    # then the weights need to be transposed.
                    tensor = tensor.T
                    self.add_tensor(weights[0], tensor)  # re-register the transformed version.
                else:
                    raise Exception("Cannot multiply matrices of incompatible shapes {} x {}".format(
                        tensor_shape, input_shape))

                weights = (weights[0], tensor, self.get_order(tensor.shape))

            result['weights'] = weights
        else:
            raise Exception("fully connected node missing required weights")
        if len(tensor_inputs) > 1:
            result['bias'] = tensor_inputs[1]

        self.remove_input_tensors(self.node)
        return result


class OnnxGemmConverter(OnnxNodeConverter):

    def __init__(self, converter):
        super().init(converter, "FullyConnected")

    def get_attributes(self, attrs: Attributes):
        attributes = {
            "alpha": 1,
            "beta": 1,
            "transA": False,
            "transB": False,
        }
        if "alpha" in attrs:
            attributes["alpha"] = attrs["alpha"]
        if "beta" in attrs:
            attributes["beta"] = attrs["beta"]
        if "transA" in attrs:
            attributes["transA"] = attrs["transA"]
        if "transB" in attrs:
            attributes["transB"] = attrs["transB"]

        if attributes["transA"]:
            raise Exception("### Error: transposing of the input to a fully connected node is not supported yet")
        if attributes["alpha"] != 1:
            raise Exception("### Error: full Gemm op support not available yet")
        if attributes["beta"] != 1:
            raise Exception("### Error: scaling of the input to a fully connected node is not supported yet")
        return attributes

    def get_output_shapes(self):
        # now compute the output size
        if len(self.node.input_shapes) > 2:
            # third input is the output shape
            return [self.node.input_shapes[2]]

        attributes = self.node.attributes
        if len(self.node.input_shapes) == 2:
            n, m1 = self.node.input_shapes[0][0]
            if attributes["transA"]:
                n, m1 = m1, n

            m2, p = self.node.input_shapes[1][0]
            if attributes["transB"]:
                m2, p = p, m2

            if m1 == m2:
                result = (n, p)
            elif n == p:
                # hmmm, then we still need to transpose...
                attributes["transB"] = True
                m2, p = p, m2
                result = (n, p)

        else:
            raise Exception("Gemm operation is expecting two inputs, but we have {}".format(
                len(self.node.input_shapes)))

        return [(result, self.get_order(result))]

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
            transpose = self.node.attributes["transB"]
            if tensor_len == 2:
                if len(input_shape) == 3:
                    tensor = self.reshape_3d_into_2d_tensor(input_shape, weights, transpose)
                    tensor_shape = tensor.shape
                elif transpose:
                    tensor = tensor.T
                    tensor_shape = tensor.shape

                # Now, ELL flattens the input shape and the FullyConnectedLayerNode multiples in the opposite order
                # from what you'd expect doing "weight * input", not "input * weight" so we may need to transpose here
                # again to account for this.
                input_shape = (np.product(input_shape), 1)
                if tensor_shape[1] == input_shape[0]:  # make sure we have m*n and n*k.
                    pass
                elif tensor_shape[0] == input_shape[0]:
                    # then the weights need to be transposed.
                    tensor = tensor.T
                    self.add_tensor(weights[0], tensor)  # re-register the transformed version.
                else:
                    raise Exception("Cannot multiply matrices of incompatible shapes {} x {}".format(
                        tensor_shape, input_shape))

                weights = (weights[0], tensor, self.get_order(tensor.shape))

            result['weights'] = weights
        else:
            raise Exception("Gemm node missing required weights")
        if len(tensor_inputs) > 1:
            result['bias'] = tensor_inputs[1]

        return result


class ReceptiveFieldConverter(OnnxNodeConverter):
    def __init__(self, converter):
        super().init(converter, "ReceptiveField")
        self.kernel_shape = None
        self.strides = None
        self.padding = None
        self.dilations = None

    def get_attributes(self, attrs: Attributes):
        attributes = {}
        # bugbug: need to support auto_pad
        # bugbug: need to support rectangular kernel shape and strides
        if 'kernel_shape' in attrs:
            self.kernel_shape = attrs['kernel_shape']
            attributes['size'] = self.kernel_shape[0]
            if not all(y == self.kernel_shape[0] for y in self.kernel_shape):
                raise Exception("Receptive field kernel shape {} is not supported".format(self.kernel_shape))
        if 'strides' in attrs:
            self.strides = attrs['strides']
            attributes['stride'] = self.strides[0]
            if not all(y == self.strides[0] for y in self.strides):
                raise Exception("Receptive field stride {} is not supported".format(self.strides))
        if 'pads' in attrs:
            self.padding = attrs['pads']
            attributes['padding'] = self.padding[0]
            if not all(y == self.padding[0] for y in self.padding):
                raise Exception("Receptive field padding {} is not supported".format(self.padding))
        if 'dilations' in attrs:
            self.dilations = attrs['dilations']
            attributes['dilation'] = self.dilations[0]
            if not all(y == self.dilations[0] for y in self.dilations):
                raise Exception("Receptive field dilations {} is not supported".format(self.dilations))
        return attributes

    def get_padding(self):
        pad = 0  # default padding
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

        k_row, k_col = 1, 1
        if isinstance(self.kernel_shape, list):
            k_row, k_col = self.kernel_shape[0], self.kernel_shape[1]

        p_row, p_col = 0, 0
        if isinstance(self.padding, list):
            p_row, p_col = self.padding[0], self.padding[1]

        d_row, d_col = 1, 1
        if isinstance(self.dilations, list):
            d_row, d_col = self.dilations[0], self.dilations[1]

        s_row, s_col = 1, 1
        if isinstance(self.strides, list):
            s_row, s_col = self.strides[0], self.strides[1]

        input_shape = node.input_shapes[0][0]  # get shape tuple from list of shapes
        channel, row, col = input_shape
        f = self.get_number_of_output_channels()
        if f is not None:
            channel = f

        out_row = (row + 2 * p_row - d_row * (k_row - 1) - 1) / s_row + 1
        out_col = (col + 2 * p_col - d_col * (k_col - 1) - 1) / s_col + 1

        shape = (channel, int(out_row), int(out_col))

        return [(shape, self.get_order(shape))]

    def get_number_of_output_channels(self):
        """
        Return the number of output channels
        """
        return None


class OnnxMaxPoolingConverter(ReceptiveFieldConverter):
    def __init__(self, converter):
        super(OnnxMaxPoolingConverter, self).__init__(converter)
        self.op_type = "MaxPooling"
        # bugbug: need to support auto_pad, ceil_mode


class OnnxAveragePoolingConverter(ReceptiveFieldConverter):
    def __init__(self, converter):
        super(OnnxAveragePoolingConverter, self).__init__(converter)
        self.op_type = "AveragePooling"


class OnnxGlobalAveragePoolConverter(OnnxAveragePoolingConverter):
    def __init__(self, converter):
        super(OnnxGlobalAveragePoolConverter, self).__init__(converter)
        self.op_type = "AveragePooling"

    def get_attributes(self, attrs: Attributes):
        attributes = {}
        # GlobalAveragePool consumes an input tensor X and applies average pooling across the values in the same
        # channel.  This is equivalent to AveragePool with kernel size equal to the spatial dimension of input tensor.
        input_shapes = self.get_input_shapes()
        shape = list(input_shapes[0][0])
        shape.pop(0)  # remove channels
        self.kernel_shape = shape
        attributes['size'] = self.kernel_shape[0]  # bugbug: need to support non-rectangular kernels.
        attributes['stride'] = 1
        attributes['padding'] = 0
        return attributes


class OnnxConvolutionConverter(ReceptiveFieldConverter):
    def __init__(self, converter):
        super(OnnxConvolutionConverter, self).__init__(converter)
        self.op_type = "Convolution"

    def convert(self, node: NodeProto):
        node = super().convert(node)
        return node

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

        for i in range(len(tensor_inputs)):
            if len(tensor_inputs[i][1].shape) == 4:
                result['weights'] = tensor_inputs[i]
            else:
                result['bias'] = tensor_inputs[i]

        return result

    def get_number_of_output_channels(self):
        """
        Return the number of output channels
        """
        shapes = self.node.input_shapes
        if len(shapes) > 1:
            # the second input is the weight tensor which has the filter info we need
            # for example a shape of (10,1,5,5) indicates 10 filters
            return shapes[1][0][0]
        return None


ONNX_OP_TYPE_TO_CONVERTER_MAP = {
    "Abs"                     : OnnxAbsConverter,  # noqa E203
    "Add"                     : OnnxPlusConverter,  # noqa E203
    "ATen"                    : OnnxAtenConverter,  # noqa E203
    "AveragePool"             : OnnxAveragePoolingConverter,  # noqa E203
    "BatchNormalization"      : OnnxBatchNormalizationConverter,  # noqa E203
    "Bias"                    : OnnxBiasConverter,  # noqa E203
    "Cast"                    : OnnxCastConverter,  # noqa E203
    "Concat"                  : OnnxConcatConverter,  # noqa E203
    "Conv"                    : OnnxConvolutionConverter,  # noqa E203
    "Convolution"             : OnnxConvolutionConverter,  # noqa E203
    "Constant"                : OnnxConstantConverter,  # noqa E203
    "ConstantFill"            : OnnxConstantFillConverter,  # noqa E203
    "ConstantOfShape"         : OnnxConstantOfShapeConverter,  # noqa E203
    "Cos"                     : OnnxCosConverter,  # noqa E203
    "Div"                     : OnnxDivConverter,  # noqa E203
    "Dropout"                 : OnnxPassthroughConverter,  # noqa E203
    "Exp"                     : OnnxExpConverter,  # noqa E203
    "FastGRNN"                : OnnxFastGRNNConverter,  # noqa E203
    "Flatten"                 : OnnxFlattenConverter,  # noqa E203
    "Gather"                  : OnnxGatherConverter,  # noqa E203
    "Gemm"                    : OnnxGemmConverter,  # noqa E203
    "GlobalAveragePool"       : OnnxGlobalAveragePoolConverter,  # noqa E203
    "Greater"                 : OnnxPassthroughConverter,  # noqa E203
    "GRU"                     : OnnxGRUConverter,  # noqa E203
    "HardSigmoid"             : OnnxHardSigmoidConverter,  # noqa E203
    "HardTanh"                : OnnxHardTanhConverter,  # noqa E203
    "Identity"                : OnnxIdentityConverter,  # noqa E203
    "Input"                   : OnnxInputConverter,  # noqa E203
    "InputNode"               : OnnxInputConverter,  # noqa E203
    "LeakyRelu"               : OnnxLeakyReLuConverter,  # noqa E203
    "LogSoftmax"              : OnnxSoftmaxConverter,  # noqa E203
    "Log"                     : OnnxLogConverter,  # noqa E203
    "LSTM"                    : OnnxLSTMConverter,  # noqa E203
    "MaxPool"                 : OnnxMaxPoolingConverter,  # noqa E203
    "Mul"                     : OnnxMulConverter,  # noqa E203
    "MatMul"                  : OnnxMatMulConverter,  # noqa E203
    "Pad"                     : OnnxPassthroughConverter,  # noqa E203
    "Passthrough"             : OnnxPassthroughConverter,  # noqa E203
    "Relu"                    : OnnxReLuConverter,  # noqa E203
    "ReLU"                    : OnnxReLuConverter,  # noqa E203
    "Reshape"                 : OnnxReshapeConverter,  # noqa E203
    "ReduceMean"              : OnnxPassthroughConverter,  # noqa E203
    "Shape"                   : OnnxShapeConverter,  # noqa E203
    "Sigmoid"                 : OnnxSigmoidConverter,  # noqa E203
    "Sin"                     : OnnxSinConverter,  # noqa E203
    "Sqrt"                    : OnnxSqrtConverter,  # noqa E203
    "Slice"                   : OnnxSliceConverter,  # noqa E203
    "Squeeze"                 : OnnxSqueezeConverter,  # noqa E203
    "Softmax"                 : OnnxSoftmaxConverter,  # noqa E203
    "Sub"                     : OnnxSubtractConverter,  # noqa E203
    "Sum"                     : OnnxPlusConverter,  # noqa E203
    "Tanh"                    : OnnxTanhConverter,  # noqa E203
    "Transpose"               : OnnxTransposeConverter,  # noqa E203
    "Unsqueeze"               : OnnxUnsqueezeConverter,  # noqa E203
}


class OnnxConverter:
    def __init__(self):
        self.model = None
        self.input_tensors = None
        self.next_name = 0
        self.nodes = []
        self.output_shapes = {}  # id of node to ouput_shape

    def _load_onnx(self, path):
        _logger.info("loading the ONNX model from: " + path)
        try:
            start = time.time()
            if isinstance(path, onnx.ModelProto):
                onnx_model = path
            else:
                onnx_model = onnx.load(path)

            end = time.time()
            seconds = end - start
            _logger.info("Loaded ONNX model in {:.3f} seconds.".format(seconds))
            # Check that the IR is well formed
            # onnx.checker.check_model(onnx_model)

            # onnx IR version
            _logger.info("ONNX IR_version {}".format(onnx_model.ir_version))

        except Exception as ex:
            _logger.error("Error occurred when loading onnx model file: " + str(ex))
            raise ex

        _logger.info("ONNX Graph producer: {} version {}".format(
            onnx_model.producer_name, onnx_model.producer_version))
        _logger.info("ONNX Graph total len: {}".format(len(onnx_model.graph.input)))
        return onnx_model.graph

    def get_optype(self, name):
        if name not in ONNX_OP_TYPE_TO_CONVERTER_MAP:
            return None
        return ONNX_OP_TYPE_TO_CONVERTER_MAP[name](self).op_type

    def add_tensor(self, id, tensor, order=None):
        if order is None:
            order = self.get_order(tensor.shape)
        return self.model.add_tensor(id, tensor, order)

    def load_model(self, path):
        """ Return a list of ONNX nodes """
        graph = self._load_onnx(path)
        return self.set_graph(graph)

    def set_graph(self, graph):
        self.model = common.importer.ImporterModel()

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
        self.define_constant_inputs(node)

        optype = node.operation_type
        if hasattr(node, "onnx_node"):
            optype = node.onnx_node.op_type

        attrs = dict((k, node.attributes[k]) for k in node.attributes)
        if "tensor" in attrs:
            attrs["tensor"] = "..."
        _logger.info("{} {} Inputs {} {} Outputs: {} {} Attributes: {}".format(
            optype, node.id, node.inputs, node.input_shapes, node.outputs, node.output_shapes, attrs))

        self.nodes.append(node)
        self.model.add_node(node.id, node)
        for i in range(len(node.outputs)):
            id = node.outputs[i]
            shape = node.output_shapes[i]
            self.output_shapes[id] = shape

    def define_constant_inputs(self, node):
        for x in node.inputs:
            if self.is_tensor(x) and x not in self.model.nodes:
                t = self.get_tensor(x)[0]
                self.add_constant_node(x, t)

    def add_constant_node(self, name, tensor):
        node = common.converters.ImporterNode(
            id=name,
            operation_type="Constant",
            inputs=[],
            outputs=[name]
        )

        # assuming onnx_node is of type: NodeProto, this performs some common conversion operations
        node.attributes = {'tensor': tensor}
        node.input_shapes = []
        node.output_shapes = [(tensor.shape, self.get_order(tensor.shape))]
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
        if tensor_len == 4:
            return 'filter_channel_row_column'
        elif tensor_len == 3:
            return 'channel_row_column'
        elif tensor_len == 2:
            return 'row_column'
        elif tensor_len == 1:
            return 'channel'
        else:
            return 'scalar'
