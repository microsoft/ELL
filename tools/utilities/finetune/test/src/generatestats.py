####################################################################################################
#
#  Project:  Embedded Learning Library (ELL)
#  File:     generatestats.py
#  Authors:  Chuck Jacobs
#
#  Requires: Python 3.x
#
####################################################################################################

import numpy as np


def get_random_vector(size):
    return np.random.randint(0, 100, size) / 10.0


def get_random_array(shape):
    return np.random.randint(0, 100, shape) / 10.0


def get_vector_dataset(size, count):
    return get_random_array([count, size])


def get_array_dataset(shape, count):
    return get_random_array([count] + shape)


def get_vector_mean(dataset):
    return dataset.mean(0)


def get_vector_std(dataset):
    return dataset.std(0)


def get_scalar_mean(dataset):
    return dataset.mean()


def get_scalar_std(dataset):
    return dataset.std()
