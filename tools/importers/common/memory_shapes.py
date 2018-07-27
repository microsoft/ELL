####################################################################################################
#
# Project:  Embedded Learning Library (ELL)
# File:     memory_shapes.py (importers)
# Authors:  Byron Changuion
#
# Requires: Python 3.5+
#
####################################################################################################

import logging
import typing

import numpy as np

import ell

_logger = logging.getLogger(__name__)

def get_ell_shape(shape: tuple, order: str, padding: int = 0):
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


def get_ell_port_memory_layout(shape: tuple, order: str, padding: int = 0):
    """
    Return the port memory layout in ELL canonical order
    """
    if order == "channel_row_column":
        channels = shape[0]
        rows = shape[1]
        columns = shape[2]
    elif order == "channel":
        channels = shape[0]
        rows = 1
        columns = 1
    else:
        raise NotImplementedError(
            "Unsupported input shape order: {}".format(order))
    
    return ell.model.PortMemoryLayout([rows, columns, channels], [padding, padding, 0])


def get_tensor_in_ell_order(tensor: np.array, order: str):
    """
    Returns a numpy array in ELL order
    """ 
    original_tensor = tensor
    original_shape = original_tensor.shape
    if order == "filter_channel_row_column":
        ordered_weights = np.moveaxis(original_tensor, 1, -1)
        ordered_weights = ordered_weights.ravel().astype(np.float).reshape(
            original_shape[0] * original_shape[2], original_shape[3], original_shape[1])
    elif order == "channel_row_column":
        ordered_weights = np.moveaxis(original_tensor, 0, -1)
        ordered_weights = ordered_weights.ravel().astype(np.float).reshape(
            original_shape[1], original_shape[2], original_shape[0])
    elif order == "row_column":
        ordered_weights = np.moveaxis(original_tensor, 0, -1)
        ordered_weights = ordered_weights.ravel().astype(
            np.float).reshape(original_shape[1], 1, original_shape[0])
    elif order == "channel":
        ordered_weights = original_tensor.ravel().astype(
            np.float).reshape(1, 1, original_tensor.size)
    elif order == "channel_row_column_filter":
        ordered_weights = np.moveaxis(original_tensor, 0, -1)
        ordered_weights = np.moveaxis(ordered_weights, 2, 0)
        ordered_weights = ordered_weights.ravel().astype(np.float).reshape(
        original_shape[3] * original_shape[1], original_shape[2], original_shape[0])
    else:
        raise NotImplementedError(
            "Unsupported tensor order {}, for {}".format(order, uid))

    return ordered_weights
