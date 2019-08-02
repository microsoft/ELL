#!/usr/bin/env python3
###################################################################################################
#
#  Project:  Embedded Learning Library (ELL)
#  File:     train_classifier.py
#  Authors:  Chris Lovett
#
#  Requires: Python 3.x
#
###################################################################################################

import argparse
import json
import math
import os
import sys
import time

import numpy as np
import torch
import torch.nn as nn
import torch.nn.functional as F
import torch.optim as optim
from torch.autograd import Variable, Function
import torch.onnx
from torch.utils.data import Dataset, DataLoader

from training_config import TrainingConfig
from rnn import *


def onnx_exportable_fastgrnn(*fargs, output, hidden_size, wRank, uRank, gate_nonlinearity, update_nonlinearity):
    class RNNSymbolic(Function):
        @staticmethod
        def symbolic(g, *fargs):
            # NOTE: args/kwargs contain RNN parameters
            return g.op("FastGRNN", *fargs, outputs=1,
                        hidden_size_i=hidden_size, wRank_i=wRank, uRank_i=uRank,
                        gate_nonlinearity_s=gate_nonlinearity, update_nonlinearity_s=update_nonlinearity)

        @staticmethod
        def forward(ctx, *fargs):
            return output

        @staticmethod
        def backward(ctx, *gargs, **gkwargs):
            raise RuntimeError("FIXME: Traced RNNs don't support backward")

    output_temp = RNNSymbolic.apply(*fargs)
    return output_temp


class TriangularLR(optim.lr_scheduler._LRScheduler):
    def __init__(self, optimizer, stepsize, lr_min, lr_max, gamma):
        self.stepsize = stepsize
        self.lr_min = lr_min
        self.lr_max = lr_max
        self.gamma = gamma
        super(TriangularLR, self).__init__(optimizer)

    def get_lr(self):
        it = self.last_epoch
        cycle = math.floor(1 + it / (2 * self.stepsize))
        x = abs(it / self.stepsize - 2 * cycle + 1)
        decayed_range = (self.lr_max - self.lr_min) * self.gamma ** (it / 3)
        lr = self.lr_min + decayed_range * x
        return [lr]


class ExponentialResettingLR(optim.lr_scheduler._LRScheduler):
    def __init__(self, optimizer, gamma, reset_epoch):
        self.gamma = gamma
        self.reset_epoch = int(reset_epoch)
        super(ExponentialResettingLR, self).__init__(optimizer)

    def get_lr(self):
        epoch = self.last_epoch
        if epoch > self.reset_epoch:
            epoch -= self.reset_epoch
        return [base_lr * self.gamma ** epoch
                for base_lr in self.base_lrs]


class KeywordSpotter(nn.Module):
    """ This baseclass provides the PyTorch Module pattern for defining and training keyword spotters """

    def __init__(self, input_dim, num_keywords, batch_first=False):
        """
        Initialize the KeywordSpotter with the following parameters:
        input_dim - the size of the input audio frame in # samples
        num_keywords - the number of predictions to come out of the model.
        """
        super(KeywordSpotter, self).__init__()

        self.input_dim = input_dim
        self.num_keywords = num_keywords
        self.batch_first = batch_first
        self.training = False
        self.tracking = False

        self.init_hidden()

    def name(self):
        return "KeywordSpotter"

    def init_hidden(self):
        """ Clear any  hidden state """
        pass

    def forward(self, input):
        """ Perform the forward processing of the given input and return the prediction """
        raise Exception("need to implement the forward method")

    def export(self, name, device):
        """ Export the model to the ONNX file format """
        self.init_hidden()
        self.tracking = True
        dummy_input = Variable(torch.randn(1, 1, self.input_dim))
        if device:
            dummy_input = dummy_input.to(device)
        torch.onnx.export(self, dummy_input, name, verbose=True)
        self.tracking = False

    def batch_accuracy(self, scores, labels):
        """ Compute the training accuracy of the results of a single mini-batch """
        batch_size = scores.shape[0]
        passed = 0
        results = []
        for i in range(batch_size):
            expected = labels[i]
            actual = scores[i].argmax()
            results += [int(actual)]
            if expected == actual:
                passed += 1
        return (float(passed) * 100.0 / float(batch_size), passed, results)

    def fit(self, training_data, validation_data, options, model, device=None, detail=False, run=None):
        """
        Perform the training.  This is not called "train" because the base class already defines
        that method with a different meaning.  The base class "train" method puts the Module into
        "training mode".
        """
        print("Training {} using {} rows of featurized training input...".format(self.name(), training_data.num_rows))

        if training_data.mean is not None:
            self.mean = torch.from_numpy(np.array([[training_data.mean]])).to(device)
            self.std = torch.from_numpy(np.array([[training_data.std]])).to(device)
        else:
            self.mean = None
            self.std = None

        start = time.time()
        loss_function = nn.NLLLoss()
        initial_rate = options.learning_rate
        lr_scheduler = options.lr_scheduler
        oo = options.optimizer_options
        self.training = True

        if options.optimizer == "Adadelta":
            optimizer = optim.Adadelta(self.parameters(), lr=initial_rate, weight_decay=oo.weight_decay,
                                       rho=oo.rho, eps=oo.eps)
        elif options.optimizer == "Adagrad":
            optimizer = optim.Adagrad(self.parameters(), lr=initial_rate, weight_decay=oo.weight_decay,
                                      lr_decay=oo.lr_decay)
        elif options.optimizer == "Adam":
            optimizer = optim.Adam(self.parameters(), lr=initial_rate, weight_decay=oo.weight_decay,
                                   betas=oo.betas, eps=oo.eps)
        elif options.optimizer == "Adamax":
            optimizer = optim.Adamax(self.parameters(), lr=initial_rate, weight_decay=oo.weight_decay,
                                     betas=oo.betas, eps=oo.eps)
        elif options.optimizer == "ASGD":
            optimizer = optim.ASGD(self.parameters(), lr=initial_rate, weight_decay=oo.weight_decay,
                                   lambd=oo.lambd, alpha=oo.alpha, t0=oo.t0)
        elif options.optimizer == "RMSprop":
            optimizer = optim.RMSprop(self.parameters(), lr=initial_rate, weight_decay=oo.weight_decay,
                                      eps=oo.eps, alpha=oo.alpha, momentum=oo.momentum, centered=oo.centered)
        elif options.optimizer == "Rprop":
            optimizer = optim.Rprop(self.parameters(), lr=initial_rate, etas=oo.etas,
                                    step_sizes=oo.step_sizes)
        elif options.optimizer == "SGD":
            optimizer = optim.SGD(self.parameters(), lr=initial_rate, weight_decay=oo.weight_decay,
                                  momentum=oo.momentum, dampening=oo.dampening)

        print(optimizer)
        num_epochs = options.max_epochs
        batch_size = options.batch_size
        learning_rate = options.learning_rate
        lr_min = options.lr_min
        lr_peaks = options.lr_peaks
        ticks = training_data.num_rows / batch_size  # iterations per epoch

        # Calculation of total iterations in non-rolling vs rolling training
        # ticks = num_rows/batch_size (total number of iterations per epoch)
        # Non-Rolling Training:
        # Total Iteration = num_epochs * ticks
        # Rolling Training:
        # irl = Initial_rolling_length (We are using 2)
        # If num_epochs <=  max_rolling_length:
        # Total Iterations = sum(range(irl, irl + num_epochs))
        # If num_epochs > max_rolling_length:
        # Total Iterations = sum(range(irl, irl + max_rolling_length)) + (num_epochs - max_rolling_length)*ticks
        if options.rolling:
            rolling_length = 2
            max_rolling_length = int(ticks)
            if max_rolling_length > options.max_rolling_length + rolling_length:
                max_rolling_length = options.max_rolling_length + rolling_length
            bag_count = 100
            hidden_bag_size = batch_size * bag_count
            if num_epochs + rolling_length < max_rolling_length:
                max_rolling_length = num_epochs + rolling_length
            total_iterations = sum(range(rolling_length, max_rolling_length))
            if num_epochs + rolling_length > max_rolling_length:
                epochs_remaining = num_epochs + rolling_length - max_rolling_length
                total_iterations += epochs_remaining * training_data.num_rows / batch_size
            ticks = total_iterations / num_epochs
        else:
            total_iterations = ticks * num_epochs
        gamma = options.lr_gamma

        if not lr_min:
            lr_min = learning_rate
        scheduler = None
        if lr_scheduler == "TriangleLR":
            steps = lr_peaks * 2 + 1
            stepsize = num_epochs / steps
            scheduler = TriangularLR(optimizer, stepsize * ticks, lr_min, learning_rate, gamma)
        elif lr_scheduler == "CosineAnnealingLR":
            # divide by odd number to finish on the minimum learning rate
            cycles = lr_peaks * 2 + 1
            scheduler = optim.lr_scheduler.CosineAnnealingLR(optimizer, T_max=total_iterations / cycles,
                                                             eta_min=lr_min)
        elif lr_scheduler == "ExponentialLR":
            scheduler = optim.lr_scheduler.ExponentialLR(optimizer, gamma)
        elif lr_scheduler == "StepLR":
            scheduler = optim.lr_scheduler.StepLR(optimizer, step_size=options.lr_step_size, gamma=gamma)
        elif lr_scheduler == "ExponentialResettingLR":
            reset = (num_epochs * ticks) / 3  # reset at the 1/3 mark.
            scheduler = ExponentialResettingLR(optimizer, gamma, reset)

        # optimizer = optim.Adam(model.parameters(), lr=0.0001)
        log = []
        for epoch in range(num_epochs):
            self.train()
            if options.rolling:
                rolling_length += 1
                if rolling_length <= max_rolling_length:
                    hidden1_bag = torch.from_numpy(np.zeros([1, hidden_bag_size, model.hidden_units],
                                                            dtype=np.float32)).to(device)
                    if model.architecture == 'LSTM':
                        cell1_bag = torch.from_numpy(np.zeros([1, hidden_bag_size, model.hidden_units],
                                                              dtype=np.float32)).to(device)
                    if model.num_layers >= 2:
                        hidden2_bag = torch.from_numpy(np.zeros([1, hidden_bag_size, model.hidden_units],
                                                                dtype=np.float32)).to(device)
                        if model.architecture == 'LSTM':
                            cell2_bag = torch.from_numpy(np.zeros([1, hidden_bag_size, model.hidden_units],
                                                                  dtype=np.float32)).to(device)
                    if model.num_layers == 3:
                        hidden3_bag = torch.from_numpy(np.zeros([1, hidden_bag_size, training_data.num_keywords],
                                                                dtype=np.float32)).to(device)
                        if model.architecture == 'LSTM':
                            cell3_bag = torch.from_numpy(np.zeros([1, hidden_bag_size, training_data.num_keywords],
                                                                  dtype=np.float32)).to(device)
            for i_batch, (audio, labels) in enumerate(training_data.get_data_loader(batch_size)):
                if not self.batch_first:
                    audio = audio.transpose(1, 0)  # GRU wants seq,batch,feature

                if device:
                    audio = audio.to(device)
                    labels = labels.to(device)

                # Also, we need to clear out the hidden state,
                # detaching it from its history on the last instance.
                if options.rolling:
                    if rolling_length <= max_rolling_length:
                        if (i_batch + 1) % rolling_length == 0:
                            self.init_hidden()
                            break
                    shuffled_indices = list(range(hidden_bag_size))
                    np.random.shuffle(shuffled_indices)
                    temp_indices = shuffled_indices[:batch_size]
                    if model.architecture == 'LSTM':
                        if self.hidden1 is not None:
                            hidden1_bag[:, temp_indices, :], cell1_bag[:, temp_indices, :] = self.hidden1
                            self.hidden1 = (hidden1_bag[:, 0:batch_size, :], cell1_bag[:, 0:batch_size, :])
                            if model.num_layers >= 2:
                                hidden2_bag[:, temp_indices, :], cell2_bag[:, temp_indices, :] = self.hidden2
                                self.hidden2 = (hidden2_bag[:, 0:batch_size, :], cell2_bag[:, 0:batch_size, :])
                            if model.num_layers == 3:
                                hidden3_bag[:, temp_indices, :], cell3_bag[:, temp_indices, :] = self.hidden3
                                self.hidden3 = (hidden3_bag[:, 0:batch_size, :], cell3_bag[:, 0:batch_size, :])
                    else:
                        if self.hidden1 is not None:
                            hidden1_bag[:, temp_indices, :] = self.hidden1
                            self.hidden1 = hidden1_bag[:, 0:batch_size, :]
                            if model.num_layers >= 2:
                                hidden2_bag[:, temp_indices, :] = self.hidden2
                                self.hidden2 = hidden2_bag[:, 0:batch_size, :]
                            if model.num_layers == 3:
                                hidden3_bag[:, temp_indices, :] = self.hidden3
                                self.hidden3 = hidden3_bag[:, 0:batch_size, :]
                else:
                    self.init_hidden()

                # Before the backward pass, use the optimizer object to zero all of the
                # gradients for the variables it will update (which are the learnable
                # weights of the model). This is because by default, gradients are
                # accumulated in buffers( i.e, not overwritten) whenever .backward()
                # is called. Checkout docs of torch.autograd.backward for more details.
                optimizer.zero_grad()

                # optionally normalize the audio
                if self.mean is not None:
                    audio = (audio - self.mean) / self.std

                # Run our forward pass.
                keyword_scores = self(audio)

                # Compute the loss, gradients
                loss = loss_function(keyword_scores, labels)

                # Backward pass: compute gradient of the loss with respect to all the learnable
                # parameters of the model. Internally, the parameters of each Module are stored
                # in Tensors with requires_grad=True, so this call will compute gradients for
                # all learnable parameters in the model.
                loss.backward()
                # move to next learning rate
                if scheduler:
                    scheduler.step()

                # Calling the step function on an Optimizer makes an update to its parameters
                # applying the gradients we computed during back propagation
                optimizer.step()

                learning_rate = optimizer.param_groups[0]['lr']
                if detail:
                    learning_rate = optimizer.param_groups[0]['lr']
                    log += [{'iteration': iteration, 'loss': loss.item(), 'learning_rate': learning_rate}]
            # Find the best prediction in each sequence and return it's accuracy
            passed, total, rate = self.evaluate(validation_data, batch_size, device)
            learning_rate = optimizer.param_groups[0]['lr']
            current_loss = float(loss.item())
            print("Epoch {}, Loss {:.3f}, Validation Accuracy {:.3f}, Learning Rate {}".format(
                  epoch, current_loss, rate * 100, learning_rate))
            log += [{'epoch': epoch, 'loss': current_loss, 'accuracy': rate, 'learning_rate': learning_rate}]
            if run is not None:
                run.log('progress', epoch / num_epochs)
                run.log('epoch', epoch)
                run.log('accuracy', rate)
                run.log('loss', current_loss)
                run.log('learning_rate', learning_rate)

        end = time.time()
        self.training = False
        print("Trained in {:.2f} seconds".format(end - start))
        return log

    def evaluate(self, test_data, batch_size, device=None, outfile=None):
        """
        Evaluate the given test data and print the pass rate
        """
        self.eval()
        passed = 0
        total = 0

        if test_data.mean is not None:
            mean = torch.from_numpy(np.array([[test_data.mean]])).to(device)
            std = torch.from_numpy(np.array([[test_data.std]])).to(device)
        else:
            mean = None
            std = None

        self.zero_grad()
        results = []
        with torch.no_grad():
            for i_batch, (audio, labels) in enumerate(test_data.get_data_loader(batch_size)):
                batch_size = audio.shape[0]
                audio = audio.transpose(1, 0)  # GRU wants seq,batch,feature
                if device:
                    audio = audio.to(device)
                    labels = labels.to(device)
                if mean is not None:
                    audio = (audio - mean) / std
                total += batch_size
                self.init_hidden()
                keyword_scores = self(audio)
                last_accuracy, ok, actual = self.batch_accuracy(keyword_scores, labels)
                results += actual
                passed += ok

        if outfile:
            print("Saving evaluation results in '{}'".format(outfile))
            with open(outfile, "w") as f:
                json.dump(results, f)

        return (passed, total, passed / total)


class GRUKeywordSpotter(KeywordSpotter):
    """This class is a PyTorch Module that implements a 1, 2 or 3 layer GRU based audio classifier"""

    def __init__(self, input_dim, num_keywords, model):
        """
        Initialize the KeywordSpotter with the following parameters:
        input_dim - the size of the input audio frame in # samples.
        hidden_units - the size of the hidden state of the GRU nodes
        num_keywords - the number of predictions to come out of the model.
        num_layers - the number of GRU layers to use (1, 2 or 3)
        """
        self.hidden_units = model.hidden_units
        self.num_layers = model.num_layers
        super(GRUKeywordSpotter, self).__init__(input_dim, num_keywords)

        # The GRU takes audio sequences as input, and outputs hidden states
        # with dimensionality hidden_units.
        self.gru1 = nn.GRU(input_dim, self.hidden_units)
        self.gru2 = None
        if self.num_layers > 1:
            self.gru2 = nn.GRU(self.hidden_units, self.hidden_units)
        self.gru3 = None
        last_output_size = self.hidden_units
        if self.num_layers > 2:
            self.gru3 = nn.GRU(self.hidden_units, num_keywords)
            last_output_size = num_keywords

        # The linear layer is a fully connected layer that maps from hidden state space
        # to number of expected keywords
        self.hidden2keyword = nn.Linear(last_output_size, num_keywords)
        self.init_hidden()

    def name(self):
        return "{} layer GRU {}".format(self.num_layers, self.hidden_units)

    def init_hidden(self):
        """ Clear the hidden state for the GRU nodes """
        self.hidden1 = None
        self.hidden2 = None
        self.hidden3 = None

    def forward(self, input):
        """ Perform the forward processing of the given input and return the prediction """
        # input is shape: [seq,batch,feature]
        if self.tracking:
            if self.mean is not None:
                input = (input - self.mean) / self.std
        gru_out, self.hidden1 = self.gru1(input, self.hidden1)
        # we have to detach the hidden states because we may keep them longer than 1 iteration.
        self.hidden1 = self.hidden1.detach()
        if self.gru2 is not None:
            gru_out, self.hidden2 = self.gru2(gru_out, self.hidden2)
            self.hidden2 = self.hidden2.detach()
        if self.gru3 is not None:
            gru_out, self.hidden3 = self.gru3(gru_out, self.hidden3)
            self.hidden3 = self.hidden3.detach()

        # If we train taking the mean of all the prediction then it has higher chance to get triggered when a
        # similar word is spoken.  So by taking only the prediction at the end of the word is better way to
        # perform keyword spotting. So that rnn will get chance to run on complete keyword and make prediction.
        keyword_space = self.hidden2keyword(gru_out[-1, :, :])
        result = F.log_softmax(keyword_space, dim=1)
        return result


class FastGRNNKeywordSpotter(KeywordSpotter):
    """This class is a PyTorch Module that implements a 1, 2 or 3 layer GRU based audio classifier"""

    def __init__(self, input_dim, num_keywords, model):
        """
        Initialize the KeywordSpotter with the following parameters:
        input_dim - the size of the input audio frame in # samples.
        hidden_units - the size of the hidden state of the FastGrnn nodes
        num_keywords - the number of predictions to come out of the model.
        num_layers - the number of FastGrnn layers to use (1, 2 or 3)
        """
        self.hidden_units = model.hidden_units
        self.num_layers = model.num_layers
        self.num_keywords = num_keywords
        self.wRank = model.wRank
        self.uRank = model.uRank
        self.gate_nonlinearity = model.gate_nonlinearity
        self.update_nonlinearity = model.update_nonlinearity

        super(FastGRNNKeywordSpotter, self).__init__(input_dim, num_keywords)

        # The FastGRNN takes audio sequences as input, and outputs hidden states
        # with dimensionality hidden_units.
        self.fastgrnn1 = FastGRNN(input_dim, self.hidden_units,
                                  gate_nonlinearity=self.gate_nonlinearity,
                                  update_nonlinearity=self.update_nonlinearity,
                                  wRank=self.wRank, uRank=self.uRank)
        self.fastgrnn2 = None
        if self.num_layers > 1:
            self.fastgrnn2 = FastGRNN(self.hidden_units, self.hidden_units,
                                      gate_nonlinearity=self.gate_nonlinearity,
                                      update_nonlinearity=self.update_nonlinearity,
                                      wRank=self.wRank, uRank=self.uRank)
        self.fastgrnn3 = None
        last_output_size = self.hidden_units
        if self.num_layers > 2:
            self.fastgrnn3 = FastGRNN(self.hidden_units, num_keywords,
                                      gate_nonlinearity=self.gate_nonlinearity,
                                      update_nonlinearity=self.update_nonlinearity,
                                      wRank=self.wRank, uRank=self.uRank)
            last_output_size = num_keywords

        # The linear layer is a fully connected layer that maps from hidden state space
        # to number of expected keywords
        self.hidden2keyword = nn.Linear(last_output_size, num_keywords)
        self.init_hidden()

    def name(self):
        return "{} layer FastGRNN {}".format(self.num_layers, self.hidden_units)

    def init_hidden(self):
        """ Clear the hidden state for the GRU nodes """
        self.hidden1 = None
        self.hidden2 = None
        self.hidden3 = None

    def forward(self, input):
        """ Perform the forward processing of the given input and return the prediction """
        # input is shape: [seq,batch,feature]
        if self.tracking:
            if self.mean is not None:
                input = (input - self.mean) / self.std
        fastgrnn_out1 = self.fastgrnn1(input, hiddenState=self.hidden1, batch_first=False)
        fastgrnn_output = fastgrnn_out1
        # we have to detach the hidden states because we may keep them longer than 1 iteration.
        self.hidden1 = fastgrnn_out1.detach()[-1, :, :]
        if self.tracking:
            if self.wRank is None:
                W, U, bias_gate, bias_update, zeta, nu = self.fastgrnn1.getWeights()
                fastgrnn_out1 = onnx_exportable_fastgrnn(input, W, U, bias_gate, bias_update, zeta, nu,
                                                         output=fastgrnn_out1, hidden_size=self.hidden_units,
                                                         wRank=self.wRank, uRank=self.uRank,
                                                         gate_nonlinearity=self.gate_nonlinearity,
                                                         update_nonlinearity=self.update_nonlinearity)
            else:
                W1, W2, U1, U2, bias_gate, bias_update, zeta, nu = self.fastgrnn1.getWeights()
                fastgrnn_out1 = onnx_exportable_fastgrnn(input, W1, W2, U1, U2, bias_gate, bias_update, zeta, nu,
                                                         output=fastgrnn_out1, hidden_size=self.hidden_units,
                                                         wRank=self.wRank, uRank=self.uRank,
                                                         gate_nonlinearity=self.gate_nonlinearity,
                                                         update_nonlinearity=self.update_nonlinearity)
            fastgrnn_output = fastgrnn_out1
        if self.fastgrnn2 is not None:
            fastgrnn_out2 = self.fastgrnn2(fastgrnn_out1, hiddenState=self.hidden2, batch_first=False)
            self.hidden2 = fastgrnn_out2.detach()[-1, :, :]
            if self.tracking:
                if self.wRank is None:
                    W, U, bias_gate, bias_update, zeta, nu = self.fastgrnn2.getWeights()
                    fastgrnn_out2 = onnx_exportable_fastgrnn(fastgrnn_out1, W, U, bias_gate, bias_update, zeta, nu,
                                                             output=fastgrnn_out2, hidden_size=self.hidden_units,
                                                             wRank=self.wRank, uRank=self.uRank,
                                                             gate_nonlinearity=self.gate_nonlinearity,
                                                             update_nonlinearity=self.update_nonlinearity)
                else:
                    W1, W2, U1, U2, bias_gate, bias_update, zeta, nu = self.fastgrnn2.getWeights()
                    fastgrnn_out2 = onnx_exportable_fastgrnn(fastgrnn_out1, W1, W2, U1, U2, bias_gate,
                                                             bias_update, zeta, nu,
                                                             output=fastgrnn_out2, hidden_size=self.hidden_units,
                                                             wRank=self.wRank, uRank=self.uRank,
                                                             gate_nonlinearity=self.gate_nonlinearity,
                                                             update_nonlinearity=self.update_nonlinearity)
            fastgrnn_output = fastgrnn_out2
        if self.fastgrnn3 is not None:
            fastgrnn_out3 = self.fastgrnn3(fastgrnn_out2, hiddenState=self.hidden3, batch_first=False)
            self.hidden3 = fastgrnn_out3.detach()[-1, :, :]
            if self.tracking:
                if self.wRank is None:
                    W, U, bias_gate, bias_update, zeta, nu = self.fastgrnn3.getWeights()
                    fastgrnn_out3 = onnx_exportable_fastgrnn(fastgrnn_out2, W, U, bias_gate, bias_update, zeta, nu,
                                                             output=fastgrnn_out3, hidden_size=self.num_keywords,
                                                             wRank=self.wRank, uRank=self.uRank,
                                                             gate_nonlinearity=self.gate_nonlinearity,
                                                             update_nonlinearity=self.update_nonlinearity)
                else:
                    W1, W2, U1, U2, bias_gate, bias_update, zeta, nu = self.fastgrnn3.getWeights()
                    fastgrnn_out3 = onnx_exportable_fastgrnn(fastgrnn_out2, W1, W2, U1, U2, bias_gate,
                                                             bias_update, zeta, nu,
                                                             output=fastgrnn_out3, hidden_size=self.num_keywords,
                                                             wRank=self.wRank, uRank=self.uRank,
                                                             gate_nonlinearity=self.gate_nonlinearity,
                                                             update_nonlinearity=self.update_nonlinearity)
            fastgrnn_output = fastgrnn_out3
        keyword_space = self.hidden2keyword(fastgrnn_output[-1, :, :])

        result = F.log_softmax(keyword_space, dim=1)
        return result


class LSTMKeywordSpotter(KeywordSpotter):
    """This class is a PyTorch Module that implements a 1, 2 or 3 layer LSTM based audio classifier"""

    def __init__(self, input_dim, num_keywords, model):
        """
        Initialize the KeywordSpotter with the following parameters:
        input_dim - the size of the input audio frame in # samples.
        hidden_units - the size of the hidden state of the LSTM nodes
        num_keywords - the number of predictions to come out of the model.
        num_layers - the number of LSTM layers to use (1, 2 or 3)
        """
        self.hidden_units = model.hidden_units
        self.num_layers = model.num_layers
        self.input_dim = input_dim
        super(LSTMKeywordSpotter, self).__init__(input_dim, num_keywords)

        # The LSTM takes audio sequences as input, and outputs hidden states
        # with dimensionality hidden_units.
        self.lstm1 = nn.LSTM(input_dim, self.hidden_units)
        self.lstm2 = None
        if self.num_layers > 1:
            self.lstm2 = nn.LSTM(self.hidden_units, self.hidden_units)
        self.lstm3 = None
        last_output_size = self.hidden_units
        if self.num_layers > 2:
            # layer 3 can reduce output to num_keywords, this makes for a smaller
            # layer and a much smaller Linear layer below so we get some of the
            # size back.
            self.lstm3 = nn.LSTM(self.hidden_units, num_keywords)
            last_output_size = num_keywords

        # The linear layer is a fully connected layer that maps from hidden state space
        # to number of expected keywords
        self.hidden2keyword = nn.Linear(last_output_size, num_keywords)
        self.init_hidden()

    def name(self):
        return "{} layer LSTM {}".format(self.num_layers, self.hidden_units)

    def init_hidden(self):
        """ Clear the hidden state for the LSTM nodes """
        self.hidden1 = None
        self.hidden2 = None
        self.hidden3 = None

    def forward(self, input):
        """ Perform the forward processing of the given input and return the prediction """
        # input is shape: [seq,batch,feature]
        if self.tracking:
            if self.mean is not None:
                input = (input - self.mean) / self.std
        lstm_out, self.hidden1 = self.lstm1(input, self.hidden1)
        hidden, cell = self.hidden1
        # we have to detach the hidden states because we may keep them longer than 1 iteration.
        self.hidden1 = (hidden.detach(), cell.detach())
        if self.lstm2 is not None:
            lstm_out, self.hidden2 = self.lstm2(lstm_out, self.hidden2)
            hidden, cell = self.hidden2
            self.hidden2 = (hidden.detach(), cell.detach())
        if self.lstm3 is not None:
            lstm_out, self.hidden3 = self.lstm3(lstm_out, self.hidden3)
            hidden, cell = self.hidden3
            self.hidden3 = (hidden.detach(), cell.detach())
        keyword_space = self.hidden2keyword(lstm_out[-1, :, :])
        result = F.log_softmax(keyword_space, dim=1)
        return result


class AudioDataset(Dataset):
    """
    Featurized Audio in PyTorch Dataset so we can get a DataLoader that is needed for
    mini-batch training.
    """

    def __init__(self, filename, config, keywords):
        """ Initialize the AudioDataset from the given *.npz file """
        self.dataset = np.load(filename)

        # get parameters saved by make_dataset.py
        parameters = self.dataset["parameters"]
        self.sample_rate = int(parameters[0])
        self.audio_size = int(parameters[1])
        self.input_size = int(parameters[2])
        self.window_size = int(parameters[3])
        self.shift = int(parameters[4])
        self.features = self.dataset["features"].astype(np.float32)
        self.num_rows = len(self.features)
        self.features = self.features.reshape((self.num_rows, self.window_size, self.input_size))

        if config.normalize:
            mean = self.features.mean(axis=0)
            std = self.features.std(axis=0)
            self.mean = mean.mean(axis=0).astype(np.float32)
            std = std.mean(axis=0)
            # self.std is a divisor, so make sure it contains no zeros
            self.std = np.array(np.where(std == 0, 1, std)).astype(np.float32)
        else:
            self.mean = None
            self.std = None
        self.label_names = self.dataset["labels"]
        self.keywords = keywords
        self.num_keywords = len(self.keywords)
        self.labels = self.to_long_vector()
        msg = "Loaded dataset {} and found sample rate {}, audio_size {}, input_size {}, window_size {} and shift {}"
        print(msg.format(os.path.basename(filename), self.sample_rate, self.audio_size, self.input_size,
                         self.window_size, self.shift))

    def get_data_loader(self, batch_size):
        """ Get a DataLoader that can enumerate shuffled batches of data in this dataset """
        return DataLoader(self, batch_size=batch_size, shuffle=True, drop_last=True)

    def to_long_vector(self):
        """ convert the expected labels to a list of integer indexes into the array of keywords """
        indexer = [(0 if x == "<null>" else self.keywords.index(x)) for x in self.label_names]
        return np.array(indexer, dtype=np.longlong)

    def __len__(self):
        """ Return the number of rows in this Dataset """
        return self.num_rows

    def __getitem__(self, idx):
        """ Return a single labelled sample here as a tuple """
        audio = self.features[idx]  # batch index is second dimension
        label = self.labels[idx]
        sample = (audio, label)
        return sample


def create_model(model, input_size, num_keywords):
    if model.architecture == "GRU":
        return GRUKeywordSpotter(input_size, num_keywords, model)
    elif model.architecture == "FastGRNN":
        return FastGRNNKeywordSpotter(input_size, num_keywords, model)
    elif model.architecture == "LSTM":
        return LSTMKeywordSpotter(input_size, num_keywords, model)
    else:
        raise Exception("Model architecture '{}' not supported".format(arch))


def save_json(obj, filename):
    with open(filename, "w") as f:
        json.dump(obj, f, indent=2)


def train(config, evaluate_only=False, outdir=".", detail=False, azureml=False):

    filename = config.model.filename
    categories_file = config.dataset.categories
    wav_directory = config.dataset.path
    batch_size = config.training.batch_size
    hidden_units = config.model.hidden_units
    architecture = config.model.architecture
    num_layers = config.model.num_layers
    use_gpu = config.training.use_gpu

    run = None

    if azureml:
        from azureml.core.run import Run
        run = Run.get_context()
        if run is None:
            print("### Run.get_context() returned None")
        else:
            print("### Running in Azure Context")

    valid_layers = [1, 2, 3]
    if num_layers not in valid_layers:
        raise Exception("--num_layers can only be one of these values {}".format(valid_layers))

    if not os.path.isdir(outdir):
        os.makedirs(outdir)

    if not filename:
        filename = "{}{}KeywordSpotter.pt".format(architecture, hidden_units)
        config.model.filename = filename

    # load the featurized data
    if not os.path.isdir(wav_directory):
        print("### Error: please specify valid --dataset folder location: {}".format(wav_directory))
        sys.exit(1)

    if not categories_file:
        categories_file = os.path.join(wav_directory, "categories.txt")

    with open(categories_file, "r") as f:
        keywords = [x.strip() for x in f.readlines()]

    training_file = os.path.join(wav_directory, "training_list.npz")
    testing_file = os.path.join(wav_directory, "testing_list.npz")
    validation_file = os.path.join(wav_directory, "validation_list.npz")

    if not os.path.isfile(training_file):
        print("Missing file {}".format(training_file))
        print("Please run make_datasets.py")
        sys.exit(1)
    if not os.path.isfile(validation_file):
        print("Missing file {}".format(validation_file))
        print("Please run make_datasets.py")
        sys.exit(1)
    if not os.path.isfile(testing_file):
        print("Missing file {}".format(testing_file))
        print("Please run make_datasets.py")
        sys.exit(1)

    model = None

    device = torch.device("cpu")
    if use_gpu:
        if torch.cuda.is_available():
            device = torch.device("cuda")
        else:
            print("### CUDA not available!!")

    print("Loading {}...".format(testing_file))
    test_data = AudioDataset(testing_file, config.dataset, keywords)

    log = None
    if not evaluate_only:
        print("Loading {}...".format(training_file))
        training_data = AudioDataset(training_file, config.dataset, keywords)

        print("Loading {}...".format(validation_file))
        validation_data = AudioDataset(validation_file, config.dataset, keywords)

        if training_data.mean is not None:
            fname = os.path.join(outdir, "mean.npy")
            print("Saving {}".format(fname))
            np.save(fname, training_data.mean)
            fname = os.path.join(outdir, "std.npy")
            print("Saving {}".format(fname))
            np.save(fname, training_data.std)

            # use the training_data mean and std variation
            test_data.mean = training_data.mean
            test_data.std = training_data.std
            validation_data.mean = training_data.mean
            validation_data.std = training_data.std

        print("Training model {}".format(filename))
        model = create_model(config.model, training_data.input_size, training_data.num_keywords)
        if device.type == 'cuda':
            model.cuda()  # move the processing to GPU

        start = time.time()
        log = model.fit(training_data, validation_data, config.training, config.model, device, detail, run)
        end = time.time()

        passed, total, rate = model.evaluate(training_data, batch_size, device)
        print("Training accuracy = {:.3f} %".format(rate * 100))

        torch.save(model.state_dict(), os.path.join(outdir, filename))

    print("Evaluating {} keyword spotter using {} rows of featurized test audio...".format(
          architecture, test_data.num_rows))
    if model is None:
        msg = "Loading trained model with input size {}, hidden units {} and num keywords {}"
        print(msg.format(test_data.input_size, hidden_units, test_data.num_keywords))
        model = create_model(config.model, test_data.input_size, test_data.num_keywords)
        model.load_dict(torch.load(filename))
        if model and device.type == 'cuda':
            model.cuda()  # move the processing to GPU

    results_file = os.path.join(outdir, "results.txt")
    passed, total, rate = model.evaluate(test_data, batch_size, device, results_file)
    print("Testing accuracy = {:.3f} %".format(rate * 100))

    if not evaluate_only:
        name = os.path.splitext(filename)[0] + ".onnx"
        print("saving onnx file: {}".format(name))
        model.export(os.path.join(outdir, name), device)

        config.dataset.sample_rate = test_data.sample_rate
        config.dataset.input_size = test_data.audio_size
        config.dataset.num_filters = test_data.input_size
        config.dataset.window_size = test_data.window_size
        config.dataset.shift = test_data.shift

        logdata = {
            "accuracy_val": rate,
            "training_time": end - start,
            "log": log
        }
        d = TrainingConfig.to_dict(config)
        logdata.update(d)

        logname = os.path.join(outdir, "train_results.json")
        save_json(logdata, logname)

    return rate, log


def str2bool(v):
    if v is None:
        return False
    lower = v.lower()
    return lower in ["t", "1", "true", "yes"]


if __name__ == '__main__':
    config = TrainingConfig()
    parser = argparse.ArgumentParser("train a GRU based neural network for keyword spotting")

    # all the training parameters
    parser.add_argument("--epochs", help="Number of epochs to train", type=int)
    parser.add_argument("--lr_scheduler", help="Type of learning rate scheduler (None, TriangleLR, CosineAnnealingLR,"
                                               " ExponentialLR, ExponentialResettingLR)")
    parser.add_argument("--learning_rate", help="Default learning rate, and maximum for schedulers", type=float)
    parser.add_argument("--lr_min", help="Minimum learning rate for the schedulers", type=float)
    parser.add_argument("--lr_peaks", help="Number of peaks for triangle and cosine schedules", type=float)
    parser.add_argument("--batch_size", "-bs", help="Batch size of training", type=int)
    parser.add_argument("--architecture", help="Specify model architecture (GRU, LSTM, FastGRNN)")
    parser.add_argument("--num_layers", type=int, help="Number of RNN layers (1, 2 or 3)")
    parser.add_argument("--hidden_units", "-hu", type=int, help="Number of hidden units in the GRU layers")
    parser.add_argument("--use_gpu", help="Whether to use GPU for training", action="store_true")
    parser.add_argument("--normalize", help="Whether to normalize audio dataset", action="store_true")
    parser.add_argument("--rolling", help="Whether to train model in rolling fashion or not", action="store_true")
    parser.add_argument("--max_rolling_length", help="Max number of epochs you want to roll the rolling training"
                        " default is 100", type=int)

    # arguments for fastgrnn
    parser.add_argument("--wRank", "-wr", help="Rank of W in FastGRNN default is None", type=int)
    parser.add_argument("--uRank", "-ur", help="Rank of U in FastGRNN default is None", type=int)
    parser.add_argument("--gate_nonlinearity", "-gnl", help="Gate Non-Linearity in FastGRNN default is sigmoid"
                        " use between [sigmoid, quantSigmoid, tanh, quantTanh]")
    parser.add_argument("--update_nonlinearity", "-unl", help="Update Non-Linearity in FastGRNN default is Tanh"
                        " use between [sigmoid, quantSigmoid, tanh, quantTanh]")

    # or you can just specify an options file.
    parser.add_argument("--config", help="Use json file containing all these options (as per 'training_config.py')")

    # and some additional stuff ...
    parser.add_argument("--azureml", help="Tells script we are running in Azure ML context")
    parser.add_argument("--eval", "-e", help="No training, just evaluate existing model", action='store_true')
    parser.add_argument("--filename", "-o", help="Name of model file to generate")
    parser.add_argument("--categories", "-c", help="Name of file containing keywords")
    parser.add_argument("--dataset", "-a", help="Path to the audio folder containing 'training.npz' file")
    parser.add_argument("--outdir", help="Folder in which to store output file and log files")
    parser.add_argument("--detail", "-d", help="Save loss info for every iteration not just every epoch",
                        action="store_true")
    args = parser.parse_args()

    if args.config:
        config.load(args.config)

    azureml = str2bool(args.azureml)

    # then any user defined options overrides these defaults
    if args.epochs:
        config.training.max_epochs = args.epochs
    if args.learning_rate:
        config.training.learning_rate = args.learning_rate
    if args.lr_min:
        config.training.lr_min = args.lr_min
    if args.lr_peaks:
        config.training.lr_peaks = args.lr_peaks
    if args.lr_scheduler:
        config.training.lr_scheduler = args.lr_scheduler
    if args.batch_size:
        config.training.batch_size = args.batch_size
    if args.rolling:
        config.training.rolling = args.rolling
    if args.max_rolling_length:
        config.training.max_rolling_length = args.max_rolling_length
    if args.architecture:
        config.model.architecture = args.architecture
    if args.num_layers:
        config.model.num_layers = args.num_layers
    if args.hidden_units:
        config.model.hidden_units = args.hidden_units
    if args.filename:
        config.model.filename = args.filename
    if args.use_gpu:
        config.training.use_gpu = args.use_gpu
    if args.normalize:
        config.dataset.normalize = args.normalize
    if args.categories:
        config.dataset.categories = args.categories
    if args.dataset:
        config.dataset.path = args.dataset
    if args.wRank:
        config.model.wRank = args.wRank
    if args.uRank:
        config.model.uRank = args.wRank
    if args.gate_nonlinearity:
        config.model.gate_nonlinearity = args.gate_nonlinearity
    if args.update_nonlinearity:
        config.model.update_nonlinearity = args.update_nonlinearity

    if not os.path.isfile("config.json"):
        config.save("config.json")

    train(config, args.eval, args.outdir, args.detail, azureml)
