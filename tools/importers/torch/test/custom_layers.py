####################################################################################################
#
# Project:  Embedded Learning Library (ELL)
# File:     onnx_utilities.py (importers)
# Authors:  Iliass Tiendrebeogo
#
# Requires: Python 3.x, onnx-v1.22
#
####################################################################################################
import os
import sys
import logging
import time
import math

import numpy as np 
import torch
import torch.nn as nn
import torch.nn.functional as F
from torch.nn.modules.utils import _single, _pair, _triple

# class _ConvNd(nn.Module):

#     def __init__(self, in_channels, out_channels, kernel_size, weight, stride,
#                  padding, dilation, transposed, output_padding, groups,  bias):
#         super(_ConvNd, self).__init__()
#         if in_channels % groups != 0:
#             raise ValueError('in_channels must be divisible by groups')
#         if out_channels % groups != 0:
#             raise ValueError('out_channels must be divisible by groups')
#         self.in_channels = in_channels
#         self.out_channels = out_channels
#         self.kernel_size = kernel_size
#         self.stride = stride
#         self.padding = padding
#         self.dilation = dilation
#         self.transposed = transposed
#         self.output_padding = output_padding
#         self.groups = groups
#         self.custom_weight = weight
#         self.custom_bias = bias
#         if transposed:
#             self.weight = nn.Parameter(torch.Tensor(
#                 in_channels, out_channels // groups, *kernel_size))
#         else:
#             self.weight = nn.Parameter(torch.Tensor(
#                 out_channels, in_channels // groups, *kernel_size))
#         if bias is not None:
#             self.bias = nn.Parameter(torch.Tensor(out_channels))
#         else:
#             self.register_parameter('bias', None)
#         self.reset_parameters()

#     def reset_parameters(self):
#         n = self.in_channels
#         for k in self.kernel_size:
#             n *= k
#         stdv = 1. / math.sqrt(n)
#         self.weight.data = self.custom_weight
#         if self.bias is not None:
#             self.bias.data = self.custom_bias

#     def __repr__(self):
#         s = ('{name}({in_channels}, {out_channels}, kernel_size={kernel_size}'
#              ', stride={stride}')
#         if self.padding != (0,) * len(self.padding):
#             s += ', padding={padding}'
#         if self.dilation != (1,) * len(self.dilation):
#             s += ', dilation={dilation}'
#         if self.output_padding != (0,) * len(self.output_padding):
#             s += ', output_padding={output_padding}'
#         if self.groups != 1:
#             s += ', groups={groups}'
#         if self.bias is None:
#             s += ', bias=False'
#         s += ')'
#         return s.format(name=self.__class__.__name__, **self.__dict__)

# class Conv2d_custom(_ConvNd):
#     def __init__(self, in_channels, out_channels, kernel_size, weight, stride=1,
#                  padding=0, dilation=1, groups=1, bias=True):
#         kernel_size = _pair(kernel_size)
#         stride = _pair(stride)
#         padding = _pair(padding)
#         dilation = _pair(dilation)
#         super(Conv2d_custom, self).__init__(
#             in_channels, out_channels, kernel_size, weight, stride, padding, dilation,
#             False, 0, groups, bias)

#     def forward(self, input):
#         return F.conv2d(input, self.weight, self.bias, self.stride,
#                         self.padding, self.dilation, self.groups)

# class FC_custom(nn.Module):
#     def __init__(self, input_features, output_features, weight, bias=None):
#         super(FC_custom, self).__init__()
#         self.input_features = input_features
#         self.output_features = output_features
 
#         # nn.Parameter is a special kind of Tensor, that will get
#         # automatically registered as Module's parameter once it's assigned
#         # as an attribute. Parameters and buffers need to be registered, or
#         # they won't appear in .parameters() (doesn't apply to buffers), and
#         # won't be converted when e.g. .cuda() is called. You can use
#         # .register_buffer() to register buffers.
#         # nn.Parameters require gradients by default.

#         self.weight = nn.Parameter(torch.Tensor(output_features, input_features))
#         if bias is not None:
#             self.bias = nn.Parameter(torch.Tensor(output_features))
#         else:
#             self.register_parameter('bias', None)

#         # initialize weights and bias
#         if isinstance(weight, torch.Tensor) == False:
#             weight = torch.from_numpy(weight)
#         self.weight.data = weight
#         if bias is not None:
#             if isinstance(bias, torch.Tensor) == False:
#                 bias = torch.from_numpy(bias)
#             self.bias.data = bias

#     def linear(self, input, weight, bias=None):
#         output = input.mm(weight.t())
#         if bias is not None:
#             output += bias.unsqueeze(0).expand_as(output)
#         return output

#     def forward(self, input):
#         return self.linear(input, self.weight, self.bias)

# def max_pool2d(input, kernel_size, stride=None, padding=0, dilation=1,
#                ceil_mode=False, return_indices=False):
#     r"""Applies a 2D max pooling over an input signal composed of several input
#     planes.

#     See :class:`~torch.nn.MaxPool2d` for details.
#     """
#     ret = torch._C._nn.max_pool2d_with_indices(input, kernel_size, stride, padding, dilation, ceil_mode)
#     return ret if return_indices else ret[0]


# A torch model for testing
class TestNet(nn.Module): 
    """
    This Neural net class is for testing and experiment
    play around, change the architecture to test different types of NN 
    """
    def __init__(self, input_shape, output_shape):
        super(TestNet, self).__init__()
        self.conv0 = nn.Conv2d(3, 10, kernel_size=5, stride=1, bias=True, padding=0)
        self.conv1 = nn.Conv2d(10, 20, kernel_size=5, stride=1, bias=True, padding=1)
        self.conv2_drop = nn.Dropout2d()
        self.fc1 = nn.Linear( 20*13*13, 20) # , bias=False
        nn.init.uniform_(self.conv0.bias)


    def forward(self, x):
        x1 = F.max_pool2d(self.conv0(x), 2, stride=2, padding=1)
        x1 = self.conv1(x1)
        x1 = self.conv2_drop(x1)
        x1 = F.relu(x1)
        x1 = x1.view(-1, np.product(x1.shape)) # ravel
        x1 = self.fc1(x1)
        return x1 

class MNist(nn.Module):
    """ A torch testing model that is good at learning the MNist dataset """
    def __init__(self, input_shape, output_shape):
        super(MNist, self).__init__()
        self.conv1 = nn.Conv2d(1, 10, kernel_size=5)
        self.conv2 = nn.Conv2d(10, 20, kernel_size=5)
        self.conv2_drop = nn.Dropout2d()
        self.fc1 = nn.Linear(20*4*4, 50)
        self.fc2 = nn.Linear(50, 10)

    def forward(self, x):
        # 1x28x28
        x = self.conv1(x)
        # 10x24x24
        x = F.relu(F.max_pool2d(x, 2))
        # 10x12x12
        x = self.conv2_drop(self.conv2(x))
        # 20x8x8
        x = F.relu(F.max_pool2d(x, 2)) 
        # 20x4x4
        x = x.view(-1, 320)
        # 320
        x = self.fc1(x)
        x = F.relu(x)
        # 50
        x = F.dropout(x, training=self.training)
        x = self.fc2(x)
        # 10
        x = F.log_softmax(x, dim=1)
        return x
