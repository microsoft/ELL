# Copyright (c) Microsoft Corporation. All rights reserved.
# Licensed under the MIT license.
# !/usr/bin/env python3
###################################################################################################
#
#  Project:  EdgeML (https://github.com/microsoft/EdgeML/blob/pytorch/pytorch/pytorch_edgeml/graph/rnn.py)
#  File:     rnn.py
#  Requires: Python 3.x
#
###################################################################################################

import torch
import torch.nn as nn


def gen_nonlinearity(A, nonlinearity):
    '''
    Returns required activation for a tensor based on the inputs

    nonlinearity is either a callable or a value in
        ['tanh', 'sigmoid', 'relu', 'quantTanh', 'quantSigm', 'quantSigm4']
    '''
    if nonlinearity == "tanh":
        return torch.tanh(A)
    elif nonlinearity == "sigmoid":
        return torch.sigmoid(A)
    elif nonlinearity == "relu":
        return torch.relu(A, 0.0)
    elif nonlinearity == "quantTanh":
        return torch.max(torch.min(A, torch.ones_like(A)), -1.0 * torch.ones_like(A))
    elif nonlinearity == "quantSigmoid":
        A = (A + 1.0) / 2.0
        return torch.max(torch.min(A, torch.ones_like(A)), torch.zeros_like(A))
    elif nonlinearity == "quantSigmoid4":
        A = (A + 2.0) / 4.0
        return torch.max(torch.min(A, torch.ones_like(A)), torch.zeros_like(A))
    else:
        # nonlinearity is a user specified function
        if not callable(nonlinearity):
            raise ValueError("nonlinearity must either be a callable or a value " +
                             + "['tanh', 'sigmoid', 'relu', 'quantTanh', " +
                             "'quantSigm'")
        return nonlinearity(A)


class BaseRNN(nn.Module):
    '''
    Generic equivalent of static_rnn in tf
    Used to unroll all the cell written in this file
    We assume data to be batch_first by default ie.,
    [batchSize, timeSteps, inputDims] else
    [timeSteps, batchSize, inputDims]
    '''

    def __init__(self, RNNCell):
        super(BaseRNN, self).__init__()
        self.RNNCell = RNNCell

    def getWeights(self):
        return self.RNNCell.getWeights()

    def forward(self, input, hiddenState=None,
                cellState=None, batch_first=True):
        if batch_first is True:
            self.device = input.device
            hiddenStates = torch.zeros(
                [input.shape[0], input.shape[1],
                 self.RNNCell.output_size]).to(self.device)
            if hiddenState is None:
                hiddenState = torch.zeros([input.shape[0],
                                           self.RNNCell.output_size]).to(self.device)
            if self.RNNCell.cellType == "LSTMLR":
                cellStates = torch.zeros(
                    [input.shape[0], input.shape[1],
                     self.RNNCell.output_size]).to(self.device)
                if cellState is None:
                    cellState = torch.zeros(
                        [input.shape[0], self.RNNCell.output_size]).to(self.device)
                for i in range(0, input.shape[1]):
                    hiddenState, cellState = self.RNNCell(
                        input[:, i, :], (hiddenState, cellState))
                    hiddenStates[:, i, :] = hiddenState
                    cellStates[:, i, :] = cellState
                return hiddenStates, cellStates
            else:
                for i in range(0, input.shape[1]):
                    hiddenState = self.RNNCell(input[:, i, :], hiddenState)
                    hiddenStates[:, i, :] = hiddenState
                return hiddenStates
        else:
            self.device = input.device
            hiddenStates = torch.zeros(
                [input.shape[0], input.shape[1],
                 self.RNNCell.output_size]).to(self.device)
            if hiddenState is None:
                hiddenState = torch.zeros([1, input.shape[1],
                                           self.RNNCell.output_size]).to(self.device)
            if self.RNNCell.cellType == "LSTMLR":
                cellStates = torch.zeros(
                    [input.shape[0], input.shape[1],
                     self.RNNCell.output_size]).to(self.device)
                if cellState is None:
                    cellState = torch.zeros(
                        [input.shape[1], self.RNNCell.output_size]).to(self.device)
                for i in range(0, input.shape[0]):
                    hiddenState, cellState = self.RNNCell(
                        input[i, :, :], (hiddenState, cellState))
                    hiddenStates[i, :, :] = hiddenState
                    cellStates[i, :, :] = cellState
                return hiddenStates, cellStates
            else:
                hiddenState = hiddenState[0]
                for i in range(0, input.shape[0]):
                    hiddenState = self.RNNCell(input[i, :, :], hiddenState)
                    hiddenStates[i, :, :] = hiddenState
                return hiddenStates


class FastGRNNCell(nn.Module):
    '''
    FastRNN Cell with Both Full Rank and Low Rank Formulations
    Has multiple activation functions for the gates
    hidden_size = # hidden units

    update_nonlinearity = nonlinearity for final rnn update
    can be chosen from [tanh, sigmoid, relu, quantTanh, quantSigm]

    wRank = rank of W matrix (creates two matrices if not None)
    uRank = rank of U matrix (creates two matrices if not None)
    alphaInit = init for alpha, the update scalar
    betaInit = init for beta, the weight for previous state

    FastRNN architecture and compression techniques are found in
    FastGRNN(LINK) paper

    Basic architecture is like:

    h_t^ = update_nl(Wx_t + Uh_{t-1} + B_h)
    h_t = sigmoid(beta)*h_{t-1} + sigmoid(alpha)*h_t^

    W and U can further parameterised into low rank version by
    W = matmul(W_1, W_2) and U = matmul(U_1, U_2)
    '''

    def __init__(self, input_size, hidden_size, gate_nonlinearity="sigmoid",
                 update_nonlinearity="tanh", wRank=None, uRank=None,
                 zetaInit=1.0, nuInit=-4.0, name="FastGRNN"):
        super(FastGRNNCell, self).__init__()

        self._input_size = input_size
        self._hidden_size = hidden_size
        self._gate_nonlinearity = gate_nonlinearity
        self._update_nonlinearity = update_nonlinearity
        self._num_weight_matrices = [1, 1]
        self._wRank = wRank
        self._uRank = uRank
        self._zetaInit = zetaInit
        self._nuInit = nuInit
        if wRank is not None:
            self._num_weight_matrices[0] += 1
        if uRank is not None:
            self._num_weight_matrices[1] += 1
        self._name = name

        if wRank is None:
            # self.W = nn.Parameter(0.1 * torch.randn([input_size, hidden_size]))
            self.W = nn.Parameter(0.1 * torch.randn([hidden_size, input_size]))
        else:
            self.W1 = nn.Parameter(0.1 * torch.randn([wRank, input_size]))
            self.W2 = nn.Parameter(0.1 * torch.randn([hidden_size, wRank]))

        if uRank is None:
            self.U = nn.Parameter(0.1 * torch.randn([hidden_size, hidden_size]))
        else:
            self.U1 = nn.Parameter(0.1 * torch.randn([uRank, hidden_size]))
            self.U2 = nn.Parameter(0.1 * torch.randn([hidden_size, uRank]))

        # self.bias_gate = nn.Parameter(torch.ones([1, hidden_size]))
        # self.bias_update = nn.Parameter(torch.ones([1, hidden_size]))
        self.bias_gate = nn.Parameter(torch.ones([hidden_size]))
        self.bias_update = nn.Parameter(torch.ones([hidden_size]))
        self.zeta = nn.Parameter(self._zetaInit * torch.ones([1, 1]))
        self.nu = nn.Parameter(self._nuInit * torch.ones([1, 1]))

    @property
    def state_size(self):
        return self._hidden_size

    @property
    def input_size(self):
        return self._input_size

    @property
    def output_size(self):
        return self._hidden_size

    @property
    def gate_nonlinearity(self):
        return self._gate_nonlinearity

    @property
    def update_nonlinearity(self):
        return self._update_nonlinearity

    @property
    def wRank(self):
        return self._wRank

    @property
    def uRank(self):
        return self._uRank

    @property
    def num_weight_matrices(self):
        return self._num_weight_matrices

    @property
    def name(self):
        return self._name

    @property
    def cellType(self):
        return "FastGRNN"

    def forward(self, input, state):
        if self._wRank is None:
            wComp = torch.matmul(input, torch.transpose(self.W, 0, 1))
        else:
            wComp = torch.matmul(
                torch.matmul(input, torch.transpose(self.W1, 0, 1)), torch.transpose(self.W2, 0, 1))

        if self._uRank is None:
            uComp = torch.matmul(state, torch.transpose(self.U, 0, 1))
        else:
            uComp = torch.matmul(
                torch.matmul(state, torch.transpose(self.U1, 0, 1)), torch.transpose(self.U2, 0, 1))

        pre_comp = wComp + uComp

        z = gen_nonlinearity(pre_comp + self.bias_gate,
                             self._gate_nonlinearity)
        c = gen_nonlinearity(pre_comp + self.bias_update,
                             self._update_nonlinearity)
        new_h = z * state + (torch.sigmoid(self.zeta) *
                             (1.0 - z) + torch.sigmoid(self.nu)) * c

        return new_h

    def getVars(self):
        Vars = []
        if self._num_weight_matrices[0] == 1:
            Vars.append(self.W)
        else:
            Vars.extend([self.W1, self.W2])

        if self._num_weight_matrices[1] == 1:
            Vars.append(self.U)
        else:
            Vars.extend([self.U1, self.U2])

        Vars.extend([self.bias_gate, self.bias_update])
        Vars.extend([self.zeta, self.nu])

        return Vars

    def getWeights(self):
        if self._num_weight_matrices[0] == 1:
            return self.W, self.U, self.bias_gate, self.bias_update, self.zeta, self.nu
        else:
            return self.W1, self.W2, self.U1, self.U2, self.bias_gate, self.bias_update, self.zeta, self.nu


class FastRNNCell(nn.Module):
    '''
    FastRNN Cell with Both Full Rank and Low Rank Formulations
    Has multiple activation functions for the gates
    hidden_size = # hidden units

    update_nonlinearity = nonlinearity for final rnn update
    can be chosen from [tanh, sigmoid, relu, quantTanh, quantSigm]

    wRank = rank of W matrix (creates two matrices if not None)
    uRank = rank of U matrix (creates two matrices if not None)
    alphaInit = init for alpha, the update scalar
    betaInit = init for beta, the weight for previous state

    FastRNN architecture and compression techniques are found in
    FastGRNN(LINK) paper

    Basic architecture is like:

    h_t^ = update_nl(Wx_t + Uh_{t-1} + B_h)
    h_t = sigmoid(beta)*h_{t-1} + sigmoid(alpha)*h_t^

    W and U can further parameterised into low rank version by
    W = matmul(W_1, W_2) and U = matmul(U_1, U_2)
    '''

    def __init__(self, input_size, hidden_size, gate_nonlinearity="sigmoid",
                 update_nonlinearity="tanh", wRank=None, uRank=None,
                 alphaInit=-3.0, betaInit=3.0, name="FastRNN"):
        super(FastRNNCell, self).__init__()

        self._input_size = input_size
        self._hidden_size = hidden_size
        self._gate_nonlinearity = gate_nonlinearity
        self._update_nonlinearity = update_nonlinearity
        self._num_weight_matrices = [1, 1]
        self._wRank = wRank
        self._uRank = uRank
        self._alphaInit = alphaInit
        self._betaInit = betaInit
        if wRank is not None:
            self._num_weight_matrices[0] += 1
        if uRank is not None:
            self._num_weight_matrices[1] += 1
        self._name = name

        if wRank is None:
            self.W = nn.Parameter(0.1 * torch.randn([input_size, hidden_size]))
        else:
            self.W1 = nn.Parameter(0.1 * torch.randn([input_size, wRank]))
            self.W2 = nn.Parameter(0.1 * torch.randn([wRank, hidden_size]))

        if uRank is None:
            self.U = nn.Parameter(
                0.1 * torch.randn([hidden_size, hidden_size]))
        else:
            self.U1 = nn.Parameter(0.1 * torch.randn([hidden_size, uRank]))
            self.U2 = nn.Parameter(0.1 * torch.randn([uRank, hidden_size]))

        self.bias_update = nn.Parameter(torch.ones([1, hidden_size]))
        self.alpha = nn.Parameter(self._alphaInit * torch.ones([1, 1]))
        self.beta = nn.Parameter(self._betaInit * torch.ones([1, 1]))

    @property
    def state_size(self):
        return self._hidden_size

    @property
    def input_size(self):
        return self._input_size

    @property
    def output_size(self):
        return self._hidden_size

    @property
    def gate_nonlinearity(self):
        return self._gate_nonlinearity

    @property
    def update_nonlinearity(self):
        return self._update_nonlinearity

    @property
    def wRank(self):
        return self._wRank

    @property
    def uRank(self):
        return self._uRank

    @property
    def num_weight_matrices(self):
        return self._num_weight_matrices

    @property
    def name(self):
        return self._name

    @property
    def cellType(self):
        return "FastRNN"

    def forward(self, input, state):
        if self._wRank is None:
            wComp = torch.matmul(input, self.W)
        else:
            wComp = torch.matmul(
                torch.matmul(input, self.W1), self.W2)

        if self._uRank is None:
            uComp = torch.matmul(state, self.U)
        else:
            uComp = torch.matmul(
                torch.matmul(state, self.U1), self.U2)

        pre_comp = wComp + uComp

        c = gen_nonlinearity(pre_comp + self.bias_update,
                             self._update_nonlinearity)
        new_h = torch.sigmoid(self.beta) * state + \
            torch.sigmoid(self.alpha) * c

        return new_h

    def getVars(self):
        Vars = []
        if self._num_weight_matrices[0] == 1:
            Vars.append(self.W)
        else:
            Vars.extend([self.W1, self.W2])

        if self._num_weight_matrices[1] == 1:
            Vars.append(self.U)
        else:
            Vars.extend([self.U1, self.U2])

        Vars.extend([self.bias_update])
        Vars.extend([self.alpha, self.beta])

        return Vars


class LSTMLRCell(nn.Module):
    '''
    LR - Low Rank
    LSTM LR Cell with Both Full Rank and Low Rank Formulations
    Has multiple activation functions for the gates
    hidden_size = # hidden units

    gate_nonlinearity = nonlinearity for the gate can be chosen from
    [tanh, sigmoid, relu, quantTanh, quantSigm]
    update_nonlinearity = nonlinearity for final rnn update
    can be chosen from [tanh, sigmoid, relu, quantTanh, quantSigm]

    wRank = rank of all W matrices
    (creates 5 matrices if not None else creates 4 matrices)
    uRank = rank of all U matrices
    (creates 5 matrices if not None else creates 4 matrices)

    LSTM architecture and compression techniques are found in
    LSTM paper

    Basic architecture is like:

    f_t = gate_nl(W1x_t + U1h_{t-1} + B_f)
    i_t = gate_nl(W2x_t + U2h_{t-1} + B_i)
    C_t^ = update_nl(W3x_t + U3h_{t-1} + B_c)
    o_t = gate_nl(W4x_t + U4h_{t-1} + B_o)
    C_t = f_t*C_{t-1} + i_t*C_t^
    h_t = o_t*update_nl(C_t)

    Wi and Ui can further parameterised into low rank version by
    Wi = matmul(W, W_i) and Ui = matmul(U, U_i)
    '''

    def __init__(self, input_size, hidden_size, gate_nonlinearity="sigmoid",
                 update_nonlinearity="tanh", wRank=None, uRank=None,
                 name="LSTMLR"):
        super(LSTMLRCell, self).__init__()

        self._input_size = input_size
        self._hidden_size = hidden_size
        self._gate_nonlinearity = gate_nonlinearity
        self._update_nonlinearity = update_nonlinearity
        self._num_weight_matrices = [4, 4]
        self._wRank = wRank
        self._uRank = uRank
        if wRank is not None:
            self._num_weight_matrices[0] += 1
        if uRank is not None:
            self._num_weight_matrices[1] += 1
        self._name = name

        if wRank is None:
            self.W1 = nn.Parameter(
                0.1 * torch.randn([input_size, hidden_size]))
            self.W2 = nn.Parameter(
                0.1 * torch.randn([input_size, hidden_size]))
            self.W3 = nn.Parameter(
                0.1 * torch.randn([input_size, hidden_size]))
            self.W4 = nn.Parameter(
                0.1 * torch.randn([input_size, hidden_size]))
        else:
            self.W = nn.Parameter(0.1 * torch.randn([input_size, wRank]))
            self.W1 = nn.Parameter(0.1 * torch.randn([wRank, hidden_size]))
            self.W2 = nn.Parameter(0.1 * torch.randn([wRank, hidden_size]))
            self.W3 = nn.Parameter(0.1 * torch.randn([wRank, hidden_size]))
            self.W4 = nn.Parameter(0.1 * torch.randn([wRank, hidden_size]))

        if uRank is None:
            self.U1 = nn.Parameter(
                0.1 * torch.randn([hidden_size, hidden_size]))
            self.U2 = nn.Parameter(
                0.1 * torch.randn([hidden_size, hidden_size]))
            self.U3 = nn.Parameter(
                0.1 * torch.randn([hidden_size, hidden_size]))
            self.U4 = nn.Parameter(
                0.1 * torch.randn([hidden_size, hidden_size]))
        else:
            self.U = nn.Parameter(0.1 * torch.randn([hidden_size, uRank]))
            self.U1 = nn.Parameter(0.1 * torch.randn([uRank, hidden_size]))
            self.U2 = nn.Parameter(0.1 * torch.randn([uRank, hidden_size]))
            self.U3 = nn.Parameter(0.1 * torch.randn([uRank, hidden_size]))
            self.U4 = nn.Parameter(0.1 * torch.randn([uRank, hidden_size]))

        self.bias_f = nn.Parameter(torch.ones([1, hidden_size]))
        self.bias_i = nn.Parameter(torch.ones([1, hidden_size]))
        self.bias_c = nn.Parameter(torch.ones([1, hidden_size]))
        self.bias_o = nn.Parameter(torch.ones([1, hidden_size]))

    @property
    def state_size(self):
        return 2 * self._hidden_size

    @property
    def input_size(self):
        return self._input_size

    @property
    def output_size(self):
        return self._hidden_size

    @property
    def gate_nonlinearity(self):
        return self._gate_nonlinearity

    @property
    def update_nonlinearity(self):
        return self._update_nonlinearity

    @property
    def wRank(self):
        return self._wRank

    @property
    def uRank(self):
        return self._uRank

    @property
    def num_weight_matrices(self):
        return self._num_weight_matrices

    @property
    def name(self):
        return self._name

    @property
    def cellType(self):
        return "LSTMLR"

    def forward(self, input, hiddenStates):
        (h, c) = hiddenStates

        if self._wRank is None:
            wComp1 = torch.matmul(input, self.W1)
            wComp2 = torch.matmul(input, self.W2)
            wComp3 = torch.matmul(input, self.W3)
            wComp4 = torch.matmul(input, self.W4)
        else:
            wComp1 = torch.matmul(
                torch.matmul(input, self.W), self.W1)
            wComp2 = torch.matmul(
                torch.matmul(input, self.W), self.W2)
            wComp3 = torch.matmul(
                torch.matmul(input, self.W), self.W3)
            wComp4 = torch.matmul(
                torch.matmul(input, self.W), self.W4)

        if self._uRank is None:
            uComp1 = torch.matmul(h, self.U1)
            uComp2 = torch.matmul(h, self.U2)
            uComp3 = torch.matmul(h, self.U3)
            uComp4 = torch.matmul(h, self.U4)
        else:
            uComp1 = torch.matmul(
                torch.matmul(h, self.U), self.U1)
            uComp2 = torch.matmul(
                torch.matmul(h, self.U), self.U2)
            uComp3 = torch.matmul(
                torch.matmul(h, self.U), self.U3)
            uComp4 = torch.matmul(
                torch.matmul(h, self.U), self.U4)
        pre_comp1 = wComp1 + uComp1
        pre_comp2 = wComp2 + uComp2
        pre_comp3 = wComp3 + uComp3
        pre_comp4 = wComp4 + uComp4

        i = gen_nonlinearity(pre_comp1 + self.bias_i,
                             self._gate_nonlinearity)
        f = gen_nonlinearity(pre_comp2 + self.bias_f,
                             self._gate_nonlinearity)
        o = gen_nonlinearity(pre_comp4 + self.bias_o,
                             self._gate_nonlinearity)

        c_ = gen_nonlinearity(pre_comp3 + self.bias_c,
                              self._update_nonlinearity)

        new_c = f * c + i * c_
        new_h = o * gen_nonlinearity(new_c, self._update_nonlinearity)
        return new_h, new_c

    def getVars(self):
        Vars = []
        if self._num_weight_matrices[0] == 4:
            Vars.extend([self.W1, self.W2, self.W3, self.W4])
        else:
            Vars.extend([self.W, self.W1, self.W2, self.W3, self.W4])

        if self._num_weight_matrices[1] == 4:
            Vars.extend([self.U1, self.U2, self.U3, self.U4])
        else:
            Vars.extend([self.U, self.U1, self.U2, self.U3, self.U4])

        Vars.extend([self.bias_f, self.bias_i, self.bias_c, self.bias_o])

        return Vars


class GRULRCell(nn.Module):
    '''
    GRU LR Cell with Both Full Rank and Low Rank Formulations
    Has multiple activation functions for the gates
    hidden_size = # hidden units

    gate_nonlinearity = nonlinearity for the gate can be chosen from
    [tanh, sigmoid, relu, quantTanh, quantSigm]
    update_nonlinearity = nonlinearity for final rnn update
    can be chosen from [tanh, sigmoid, relu, quantTanh, quantSigm]

    wRank = rank of W matrix
    (creates 4 matrices if not None else creates 3 matrices)
    uRank = rank of U matrix
    (creates 4 matrices if not None else creates 3 matrices)

    GRU architecture and compression techniques are found in
    GRU(LINK) paper

    Basic architecture is like:

    r_t = gate_nl(W1x_t + U1h_{t-1} + B_r)
    z_t = gate_nl(W2x_t + U2h_{t-1} + B_g)
    h_t^ = update_nl(W3x_t + r_t*U3(h_{t-1}) + B_h)
    h_t = z_t*h_{t-1} + (1-z_t)*h_t^

    Wi and Ui can further parameterised into low rank version by
    Wi = matmul(W, W_i) and Ui = matmul(U, U_i)
    '''

    def __init__(self, input_size, hidden_size, gate_nonlinearity="sigmoid",
                 update_nonlinearity="tanh", wRank=None, uRank=None,
                 name="GRULR"):
        super(GRULRCell, self).__init__()

        self._input_size = input_size
        self._hidden_size = hidden_size
        self._gate_nonlinearity = gate_nonlinearity
        self._update_nonlinearity = update_nonlinearity
        self._num_weight_matrices = [3, 3]
        self._wRank = wRank
        self._uRank = uRank
        if wRank is not None:
            self._num_weight_matrices[0] += 1
        if uRank is not None:
            self._num_weight_matrices[1] += 1
        self._name = name

        if wRank is None:
            self.W1 = nn.Parameter(
                0.1 * torch.randn([input_size, hidden_size]))
            self.W2 = nn.Parameter(
                0.1 * torch.randn([input_size, hidden_size]))
            self.W3 = nn.Parameter(
                0.1 * torch.randn([input_size, hidden_size]))
        else:
            self.W = nn.Parameter(0.1 * torch.randn([input_size, wRank]))
            self.W1 = nn.Parameter(0.1 * torch.randn([wRank, hidden_size]))
            self.W2 = nn.Parameter(0.1 * torch.randn([wRank, hidden_size]))
            self.W3 = nn.Parameter(0.1 * torch.randn([wRank, hidden_size]))

        if uRank is None:
            self.U1 = nn.Parameter(
                0.1 * torch.randn([hidden_size, hidden_size]))
            self.U2 = nn.Parameter(
                0.1 * torch.randn([hidden_size, hidden_size]))
            self.U3 = nn.Parameter(
                0.1 * torch.randn([hidden_size, hidden_size]))
        else:
            self.U = nn.Parameter(0.1 * torch.randn([hidden_size, uRank]))
            self.U1 = nn.Parameter(0.1 * torch.randn([uRank, hidden_size]))
            self.U2 = nn.Parameter(0.1 * torch.randn([uRank, hidden_size]))
            self.U3 = nn.Parameter(0.1 * torch.randn([uRank, hidden_size]))

        self.bias_r = nn.Parameter(torch.ones([1, hidden_size]))
        self.bias_gate = nn.Parameter(torch.ones([1, hidden_size]))
        self.bias_update = nn.Parameter(torch.ones([1, hidden_size]))
        self._device = self.bias_update.device

    @property
    def state_size(self):
        return self._hidden_size

    @property
    def input_size(self):
        return self._input_size

    @property
    def output_size(self):
        return self._hidden_size

    @property
    def gate_nonlinearity(self):
        return self._gate_nonlinearity

    @property
    def update_nonlinearity(self):
        return self._update_nonlinearity

    @property
    def wRank(self):
        return self._wRank

    @property
    def uRank(self):
        return self._uRank

    @property
    def num_weight_matrices(self):
        return self._num_weight_matrices

    @property
    def name(self):
        return self._name

    @property
    def cellType(self):
        return "GRULR"

    def forward(self, input, state):
        if self._wRank is None:
            wComp1 = torch.matmul(input, self.W1)
            wComp2 = torch.matmul(input, self.W2)
            wComp3 = torch.matmul(input, self.W3)
        else:
            wComp1 = torch.matmul(
                torch.matmul(input, self.W), self.W1)
            wComp2 = torch.matmul(
                torch.matmul(input, self.W), self.W2)
            wComp3 = torch.matmul(
                torch.matmul(input, self.W), self.W3)

        if self._uRank is None:
            uComp1 = torch.matmul(state, self.U1)
            uComp2 = torch.matmul(state, self.U2)
        else:
            uComp1 = torch.matmul(
                torch.matmul(state, self.U), self.U1)
            uComp2 = torch.matmul(
                torch.matmul(state, self.U), self.U2)

        pre_comp1 = wComp1 + uComp1
        pre_comp2 = wComp2 + uComp2

        r = gen_nonlinearity(pre_comp1 + self.bias_r,
                             self._gate_nonlinearity)
        z = gen_nonlinearity(pre_comp2 + self.bias_gate,
                             self._gate_nonlinearity)

        if self._uRank is None:
            pre_comp3 = wComp3 + torch.matmul(r * state, self.U3)
        else:
            pre_comp3 = wComp3 + \
                torch.matmul(torch.matmul(r * state, self.U), self.U3)

        c = gen_nonlinearity(pre_comp3 + self.bias_update,
                             self._update_nonlinearity)

        new_h = z * state + (1.0 - z) * c
        return new_h

    def getVars(self):
        Vars = []
        if self._num_weight_matrices[0] == 3:
            Vars.extend([self.W1, self.W2, self.W3])
        else:
            Vars.extend([self.W, self.W1, self.W2, self.W3])

        if self._num_weight_matrices[1] == 3:
            Vars.extend([self.U1, self.U2, self.U3])
        else:
            Vars.extend([self.U, self.U1, self.U2, self.U3])

        Vars.extend([self.bias_r, self.bias_gate, self.bias_update])

        return Vars


class UGRNNLRCell(nn.Module):
    '''
    UGRNN LR Cell with Both Full Rank and Low Rank Formulations
    Has multiple activation functions for the gates
    hidden_size = # hidden units

    gate_nonlinearity = nonlinearity for the gate can be chosen from
    [tanh, sigmoid, relu, quantTanh, quantSigm]
    update_nonlinearity = nonlinearity for final rnn update
    can be chosen from [tanh, sigmoid, relu, quantTanh, quantSigm]

    wRank = rank of W matrix
    (creates 3 matrices if not None else creates 2 matrices)
    uRank = rank of U matrix
    (creates 3 matrices if not None else creates 2 matrices)

    UGRNN architecture and compression techniques are found in
    UGRNN(LINK) paper

    Basic architecture is like:

    z_t = gate_nl(W1x_t + U1h_{t-1} + B_g)
    h_t^ = update_nl(W1x_t + U1h_{t-1} + B_h)
    h_t = z_t*h_{t-1} + (1-z_t)*h_t^

    Wi and Ui can further parameterised into low rank version by
    Wi = matmul(W, W_i) and Ui = matmul(U, U_i)
    '''

    def __init__(self, input_size, hidden_size, gate_nonlinearity="sigmoid",
                 update_nonlinearity="tanh", wRank=None, uRank=None,
                 name="UGRNNLR"):
        super(UGRNNLRCell, self).__init__()

        self._input_size = input_size
        self._hidden_size = hidden_size
        self._gate_nonlinearity = gate_nonlinearity
        self._update_nonlinearity = update_nonlinearity
        self._num_weight_matrices = [2, 2]
        self._wRank = wRank
        self._uRank = uRank
        if wRank is not None:
            self._num_weight_matrices[0] += 1
        if uRank is not None:
            self._num_weight_matrices[1] += 1
        self._name = name

        if wRank is None:
            self.W1 = nn.Parameter(
                0.1 * torch.randn([input_size, hidden_size]))
            self.W2 = nn.Parameter(
                0.1 * torch.randn([input_size, hidden_size]))
        else:
            self.W = nn.Parameter(0.1 * torch.randn([input_size, wRank]))
            self.W1 = nn.Parameter(0.1 * torch.randn([wRank, hidden_size]))
            self.W2 = nn.Parameter(0.1 * torch.randn([wRank, hidden_size]))

        if uRank is None:
            self.U1 = nn.Parameter(
                0.1 * torch.randn([hidden_size, hidden_size]))
            self.U2 = nn.Parameter(
                0.1 * torch.randn([hidden_size, hidden_size]))
        else:
            self.U = nn.Parameter(0.1 * torch.randn([hidden_size, uRank]))
            self.U1 = nn.Parameter(0.1 * torch.randn([uRank, hidden_size]))
            self.U2 = nn.Parameter(0.1 * torch.randn([uRank, hidden_size]))

        self.bias_gate = nn.Parameter(torch.ones([1, hidden_size]))
        self.bias_update = nn.Parameter(torch.ones([1, hidden_size]))
        self._device = self.bias_update.device

    @property
    def state_size(self):
        return self._hidden_size

    @property
    def input_size(self):
        return self._input_size

    @property
    def output_size(self):
        return self._hidden_size

    @property
    def gate_nonlinearity(self):
        return self._gate_nonlinearity

    @property
    def update_nonlinearity(self):
        return self._update_nonlinearity

    @property
    def wRank(self):
        return self._wRank

    @property
    def uRank(self):
        return self._uRank

    @property
    def num_weight_matrices(self):
        return self._num_weight_matrices

    @property
    def name(self):
        return self._name

    @property
    def cellType(self):
        return "UGRNNLR"

    def forward(self, input, state):
        if self._wRank is None:
            wComp1 = torch.matmul(input, self.W1)
            wComp2 = torch.matmul(input, self.W2)
        else:
            wComp1 = torch.matmul(
                torch.matmul(input, self.W), self.W1)
            wComp2 = torch.matmul(
                torch.matmul(input, self.W), self.W2)

        if self._uRank is None:
            uComp1 = torch.matmul(state, self.U1)
            uComp2 = torch.matmul(state, self.U2)
        else:
            uComp1 = torch.matmul(
                torch.matmul(state, self.U), self.U1)
            uComp2 = torch.matmul(
                torch.matmul(state, self.U), self.U2)

        pre_comp1 = wComp1 + uComp1
        pre_comp2 = wComp2 + uComp2

        z = gen_nonlinearity(pre_comp1 + self.bias_gate,
                             self._gate_nonlinearity)
        c = gen_nonlinearity(pre_comp2 + self.bias_update,
                             self._update_nonlinearity)

        new_h = z * state + (1.0 - z) * c
        return new_h

    def getVars(self):
        Vars = []
        if self._num_weight_matrices[0] == 2:
            Vars.extend([self.W1, self.W2])
        else:
            Vars.extend([self.W, self.W1, self.W2])

        if self._num_weight_matrices[1] == 2:
            Vars.extend([self.U1, self.U2])
        else:
            Vars.extend([self.U, self.U1, self.U2])

        Vars.extend([self.bias_gate, self.bias_update])

        return Vars


class LSTM(nn.Module):
    """Equivalent to nn.LSTM using LSTMLRCell"""

    def __init__(self, input_size, hidden_size, gate_nonlinearity="sigmoid",
                 update_nonlinearity="tanh", wRank=None, uRank=None):
        super(LSTM, self).__init__()
        self._input_size = input_size
        self._hidden_size = hidden_size
        self._gate_nonlinearity = gate_nonlinearity
        self._update_nonlinearity = update_nonlinearity
        self._wRank = wRank
        self._uRank = uRank

        self.cell = LSTMLRCell(input_size, hidden_size,
                               gate_nonlinearity=gate_nonlinearity,
                               update_nonlinearity=update_nonlinearity,
                               wRank=wRank, uRank=uRank)
        self.unrollRNN = BaseRNN(self.cell)

    def forward(self, input, hiddenState=None,
                cellState=None, batch_first=True):
        return self.unrollRNN(input, hiddenState, cellState, batch_first)


class GRU(nn.Module):
    """Equivalent to nn.GRU using GRULRCell"""

    def __init__(self, input_size, hidden_size, gate_nonlinearity="sigmoid",
                 update_nonlinearity="tanh", wRank=None, uRank=None):
        super(GRU, self).__init__()
        self._input_size = input_size
        self._hidden_size = hidden_size
        self._gate_nonlinearity = gate_nonlinearity
        self._update_nonlinearity = update_nonlinearity
        self._wRank = wRank
        self._uRank = uRank

        self.cell = GRULRCell(input_size, hidden_size,
                              gate_nonlinearity=gate_nonlinearity,
                              update_nonlinearity=update_nonlinearity,
                              wRank=wRank, uRank=uRank)
        self.unrollRNN = BaseRNN(self.cell)

    def forward(self, input, hiddenState=None,
                cellState=None, batch_first=True):
        return self.unrollRNN(input, hiddenState, cellState, batch_first)


class UGRNN(nn.Module):
    """Equivalent to nn.UGRNN using UGRNNLRCell"""

    def __init__(self, input_size, hidden_size, gate_nonlinearity="sigmoid",
                 update_nonlinearity="tanh", wRank=None, uRank=None):
        super(UGRNN, self).__init__()
        self._input_size = input_size
        self._hidden_size = hidden_size
        self._gate_nonlinearity = gate_nonlinearity
        self._update_nonlinearity = update_nonlinearity
        self._wRank = wRank
        self._uRank = uRank

        self.cell = UGRNNLRCell(input_size, hidden_size,
                                gate_nonlinearity=gate_nonlinearity,
                                update_nonlinearity=update_nonlinearity,
                                wRank=wRank, uRank=uRank)
        self.unrollRNN = BaseRNN(self.cell)

    def forward(self, input, hiddenState=None,
                cellState=None, batch_first=True):
        return self.unrollRNN(input, hiddenState, cellState, batch_first)


class FastRNN(nn.Module):
    """Equivalent to nn.FastRNN using FastRNNCell"""

    def __init__(self, input_size, hidden_size, gate_nonlinearity="sigmoid",
                 update_nonlinearity="tanh", wRank=None, uRank=None,
                 alphaInit=-3.0, betaInit=3.0):
        super(FastRNN, self).__init__()
        self._input_size = input_size
        self._hidden_size = hidden_size
        self._gate_nonlinearity = gate_nonlinearity
        self._update_nonlinearity = update_nonlinearity
        self._wRank = wRank
        self._uRank = uRank

        self.cell = FastRNNCell(input_size, hidden_size,
                                gate_nonlinearity=gate_nonlinearity,
                                update_nonlinearity=update_nonlinearity,
                                wRank=wRank, uRank=uRank,
                                alphaInit=alphaInit, betaInit=betaInit)
        self.unrollRNN = BaseRNN(self.cell)

    def forward(self, input, hiddenState=None,
                cellState=None, batch_first=True):
        return self.unrollRNN(input, hiddenState, cellState, batch_first)


class FastGRNN(nn.Module):
    """Equivalent to nn.FastGRNN using FastGRNNCell"""

    def __init__(self, input_size, hidden_size, gate_nonlinearity="sigmoid",
                 update_nonlinearity="tanh", wRank=None, uRank=None,
                 zetaInit=1.0, nuInit=-4.0):
        super(FastGRNN, self).__init__()
        self._input_size = input_size
        self._hidden_size = hidden_size
        self._gate_nonlinearity = gate_nonlinearity
        self._update_nonlinearity = update_nonlinearity
        self._wRank = wRank
        self._uRank = uRank

        self.cell = FastGRNNCell(input_size, hidden_size,
                                 gate_nonlinearity=gate_nonlinearity,
                                 update_nonlinearity=update_nonlinearity,
                                 wRank=wRank, uRank=uRank,
                                 zetaInit=zetaInit, nuInit=nuInit)
        self.unrollRNN = BaseRNN(self.cell)

    def getWeights(self):
        return self.unrollRNN.getWeights()

    def forward(self, input, hiddenState=None,
                cellState=None, batch_first=True):
        return self.unrollRNN(input, hiddenState, cellState, batch_first)
