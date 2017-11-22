####################################################################################################
#
# Project:  Embedded Learning Library (ELL)
# File:     cntk_converters.py (importers)
# Authors:  Byron Changuion
#
# Requires: Python 3.x, cntk-2.0-cp35
#
####################################################################################################

"""Converts CNTK data structures to ELL equivalents"""

import ell
import numpy as np

def get_float_vector_from_constant(constant, size):
    # Workaround: For some reason, np.full is not returning a type that SWIG can parse. So just manually walk the array setting the scalar
    array = np.zeros(size, dtype=np.float)
    for i in range(array.size):
        array[i] = constant
    return ell.FloatVector(array)


def get_float_vector_from_cntk_trainable_parameter(tensorParameter):
    """Returns an ell.FloatVector from a trainable parameter
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

    return ell.FloatVector(orderedWeights)


def get_float_vector_from_cntk_array(inputArray):
    """Returns an ell.FloatTensor from a 4, 3, 2, or 1 dimensional numpy array.
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
        print("Error: Input array has incorrect dimensions")
        return None

    return np.ravel(orderedWeights)


def get_float_tensor_from_cntk_dense_weight_parameter(tensorParameter):
    """Returns an ell.FloatTensor from a trainable parameter
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

    return ell.FloatTensor(orderedWeights)


def get_float_tensor_from_cntk_convolutional_weight_parameter(tensorParameter):
    """Returns an ell.FloatTensor from a trainable parameter
       Note that ELL's ordering is row, column, channel.
       4D parameters (e.g. those that represent convolutional weights) are stacked vertically in the row dimension.
       CNTK has them in filter, channel, row, column order.
    """
    tensorShape = tensorParameter.shape
    tensorValue = tensorParameter.value
    return get_float_tensor_from_cntk_convolutional_weight_value_shape(tensorParameter.value, tensorParameter.shape)


def get_float_tensor_from_cntk_convolutional_weight_value_shape(tensorValue, tensorShape):
    """Returns an ell.FloatTensor from a trainable parameter
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
    return ell.FloatTensor(orderedWeights)