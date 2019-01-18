####################################################################################################
#
#  Project:  Embedded Learning Library (ELL)
#  File:     train_test_model.py
#  Authors:  Chuck Jacobs
#
#  Requires: Python 3.x
#
####################################################################################################
import os

import numpy as np
import torch
import torch.nn as nn
import torch.nn.functional as F
import torch.onnx  # noqa: F401
import torch.optim as optim
import torchvision
import torchvision.transforms as transforms


def get_mnist(root):
    return torchvision.datasets.mnist.MNIST(root=root, transform=transforms.ToTensor())


def write_ell_dataset(dataset, filename):
    with open(filename, "w") as outfile:
        for i, (data, labelTensor) in enumerate(dataset):
            label = labelTensor.item()
            datavector = np.array(data).ravel()
            row = str(label) + '\t' + '\t'.join([str(x) for x in datavector]) + '\n'
            outfile.write(row)


class TwoLayer(nn.Module):
    def __init__(self, inputSize, hiddenSize, outputSize):
        super(TwoLayer, self).__init__()

        self.fc1 = nn.Linear(inputSize, hiddenSize)
        self.fc2 = nn.Linear(hiddenSize, outputSize)

    def forward(self, x):
        x = x.view(x.shape[0], -1)
        x = F.relu(self.fc1(x))
        x = self.fc2(x)

        return x


def train(model, dataset, numepochs, device=None):

    loss_function = nn.CrossEntropyLoss()
    optimizer = optim.SGD(model.parameters(), lr=0.001, momentum=0.9)

    # Train the model
    for epoch in range(numepochs):

        running_loss = 0.0
        for i, (inputs, labels) in enumerate(dataset, 0):
            if device:
                inputs, labels = inputs.to(device), labels.to(device)

            # zero the parameter gradients
            optimizer.zero_grad()

            # forward + backward + optimize
            pred = model(inputs)
            loss = loss_function(pred, labels)
            loss.backward()
            optimizer.step()

            running_loss += loss.item()
            if (i + 1) % 2000 == 0:  # print every 2000 mini-batches
                print('[%d, %5d] loss: %.3f' % (epoch + 1, i + 1, running_loss / 2000))
                running_loss = 0.0

    print('Finished Training')


def write_weights(model, path):
    layer1_weights = model.fc1.weight.detach().numpy()
    layer1_bias = model.fc1.bias.detach().numpy()

    layer2_weights = model.fc2.weight.detach().numpy()
    layer2_bias = model.fc2.bias.detach().numpy()

    np.savetxt(os.path.join(path), "MNISTLayer1Weights.inc", layer1_weights, delimiter=", ", newline=",\n")

    np.savetxt(os.path.join(path), "MNISTLayer2Weights.inc", layer2_weights, delimiter=", ", newline=",\n")

    np.savetxt(os.path.join(path), "MNISTLayer1Bias.inc", layer1_bias, delimiter=", ", newline=",\n")

    np.savetxt(os.path.join(path), "MNISTLayer2Bias.inc", layer2_bias, delimiter=", ", newline=",\n")
