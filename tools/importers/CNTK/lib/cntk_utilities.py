####################################################################################################
#
# Project:  Embedded Learning Library (ELL)
# File:     cntk_utilities.py (importers)
# Authors:  Byron Changuion, Lisa Ong
#
# Requires: Python 3.x, cntk-2.0-cp35
#
####################################################################################################

"""Internal utilities for the CNTK importer"""

import ell
from cntk import parameter, constant, load_model
from cntk.layers.typing import *
from cntk.ops import *
import cntk.logging.graph as graph
import logging

_logger = logging.getLogger(__name__)

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
    if index < len(parameters):
        return parameters[index]
    # Parameter is missing, so return None.
    return None

def find_node_by_op_name(parameters, name):
    for p in parameters:
        if (p.op_name == name):
            return p
    return None

def get_ell_activation_type(nodes):
    """Returns an ell.neural.ActivationType from the list of nodes"""
    if (find_node_by_op_name(nodes, 'ReLU') != None):
        return ell.neural.ActivationType.relu
    elif (find_node_by_op_name(nodes, 'Sigmoid') != None):
        return ell.neural.ActivationType.sigmoid
    elif (find_node_by_op_name(nodes, 'LeakyReLU') != None):
        return ell.neural.ActivationType.leaky

    return None

def get_cntk_activation_op(nodes):
    """Returns an ell.neural.ActivationType from the list of nodes"""
    if (find_node_by_op_name(nodes, 'ReLU') != None):
        return relu
    elif (find_node_by_op_name(nodes, 'Sigmoid') != None):
        return sigmoid
    elif (find_node_by_op_name(nodes, 'LeakyReLU') != None):
        return leaky_relu

    return None

def get_cntk_activation_name(nodes):
    """Returns an ell.neural.ActivationType from the list of nodes"""
    for name in ['ReLU', 'Sigmoid', 'LeakyReLU']:
        if find_node_by_op_name(nodes, name):
            return name
    return None

def is_softmax_activation(nodes):
    """Returns True is the nodes contain a softmax activation"""
    return (find_node_by_op_name(nodes, 'SoftMax') != None)


def ell_activation_type_to_string(type):
    """Returns the string representation of an ell.neural.ActivationType"""
    if (type == ell.neural.ActivationType.relu):
        return 'ReLU'
    elif (type == ell.neural.ActivationType.sigmoid):
        return 'Sigmoid'
    elif (type == ell.neural.ActivationType.leaky):
        return 'LeakyReLU'

    return ""


def ell_shape_to_string(shape):
    """Returns the string representation of an ell.math.TensorShape"""
    return (str(shape.rows) + "x" + str(shape.columns) + "x" + str(shape.channels))


def get_shape(inputShape):
    """"Returns the ell.math.TensorShape corresponding to the output shape with no adjustment for padding"""

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

    return ell.math.TensorShape(rows, columns, channels)


def get_adjusted_shape(inputShape, paddingParameters):
    """"Returns the ell.math.TensorShape corresponding to the input shape adjusted with padding"""

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

    return ell.math.TensorShape(rows, columns, channels)


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


def plot_model(root, output_file="model.png"):
    """Plots the CNTK model starting from the root node to an output image

    Pre-requisites:
        Install graphviz executables from graphviz.org
        Update your PATH environment variable to include the path to graphviz
        pip install graphviz
        pip install pydot_ng
    """

    text = graph.plot(root, output_file)
    _logger.info(text)
