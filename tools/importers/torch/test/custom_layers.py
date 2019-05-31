####################################################################################################
#
# Project:  Embedded Learning Library (ELL)
# File:     onnx_utilities.py (importers)
# Authors:  Iliass Tiendrebeogo
#
# Requires: Python 3.x, onnx-v1.22
#
####################################################################################################


import numpy as np
import torch.nn as nn
import torch.nn.functional as F


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
        self.fc1 = nn.Linear(20 * 13 * 13, 20)  # bias=False
        nn.init.uniform_(self.conv0.bias)

    def forward(self, x):
        x1 = F.max_pool2d(self.conv0(x), 2, stride=2, padding=1)
        x1 = self.conv1(x1)
        x1 = self.conv2_drop(x1)
        x1 = F.relu(x1)
        x1 = x1.view(-1, np.product(x1.shape))  # ravel
        x1 = self.fc1(x1)
        return x1


class MNist(nn.Module):
    """ A torch testing model that is good at learning the MNist dataset """
    def __init__(self, input_shape, output_shape):
        super(MNist, self).__init__()
        self.conv1 = nn.Conv2d(1, 10, kernel_size=5)
        self.conv2 = nn.Conv2d(10, 20, kernel_size=5)
        self.conv2_drop = nn.Dropout2d()
        self.fc1 = nn.Linear(20 * 4 * 4, 50)
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
