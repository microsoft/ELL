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
import os
import sys
import time
import numpy as np
import torch
import torch.nn as nn
import torch.nn.functional as F
import torch.optim as optim
from torch.autograd import Variable
import torch.onnx
from torch.utils.data import Dataset, DataLoader


class KeywordSpotter(nn.Module):
    """ This baseclass provides the PyTorch Module pattern for definining and training keyword spotters """

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

        self.init_hidden()

    def init_hidden(self):
        """ Clear any  hidden state """
        pass

    def forward(self, input):
        """ Perform the forward processing of the given input and return the prediction """
        raise Exception("need to implement the forward method")

    def export(self, name, device):
        """ Export the model to the ONNX file format """
        self.init_hidden()
        dummy_input = Variable(torch.randn(1, 1, self.input_dim))
        if device:
            dummy_input = dummy_input.to(device)
        torch.onnx.export(self, dummy_input, name, verbose=True)

    def batch_accuracy(self, scores, labels):
        """ Compute the training accuracy of the results of a single mini-batch """
        batch_size = scores.shape[0]
        passed = 0
        for i in range(batch_size):
            expected = labels[i]
            actual = scores[i].argmax()
            if expected == actual:
                passed += 1

        return (float(passed) * 100.0 / float(batch_size), passed)

    def fit(self, training_data, validation_data, batch_size=64, num_epochs=30, learning_rate=0.001, weight_decay=0,
            device=None):
        """
        Perform the training.  This is not called "train" because the base class already defines
        that method with a different meaning.  The base class "train" method puts the Module into
        "training mode".
        """
        print("Training keyword spotter using {} rows of featurized training input...".format(training_data.num_rows))
        start = time.time()
        loss_function = nn.NLLLoss()
        optimizer = optim.RMSprop(self.parameters(), lr=learning_rate, weight_decay=weight_decay)
        # optimizer = optim.Adam(model.parameters(), lr=0.0001)

        for epoch in range(num_epochs):
            self.train()
            for i_batch, (audio, labels) in enumerate(training_data.get_data_loader(batch_size)):

                if not self.batch_first:
                    audio = audio.transpose(1, 0)  # GRU wants seq,batch,feature

                if device:
                    audio = audio.to(device)
                    labels = labels.to(device)

                # Also, we need to clear out the hidden state,
                # detaching it from its history on the last instance.
                self.init_hidden()

                # Before the backward pass, use the optimizer object to zero all of the
                # gradients for the variables it will update (which are the learnable
                # weights of the model). This is because by default, gradients are
                # accumulated in buffers( i.e, not overwritten) whenever .backward()
                # is called. Checkout docs of torch.autograd.backward for more details.
                optimizer.zero_grad()

                # Run our forward pass.
                keyword_scores = self(audio)

                # Compute the loss, gradients
                loss = loss_function(keyword_scores, labels)

                # Backward pass: compute gradient of the loss with respect to all the learnable
                # parameters of the model. Internally, the parameters of each Module are stored
                # in Tensors with requires_grad=True, so this call will compute gradients for
                # all learnable parameters in the model.
                loss.backward()

                # Calling the step function on an Optimizer makes an update to its parameters
                # applying the gradients we computed during back propagation
                optimizer.step()

            # Find the best prediction in each sequence and return it's accuracy
            passed, total, rate = self.evaluate(validation_data, batch_size, device)
            print("Epoch {}, Loss {}, Validation Accuracy {:.3f}".format(epoch, loss.item(), rate * 100))

        end = time.time()
        print("Trained in {:.2f} seconds".format(end - start))

    def evaluate(self, test_data, batch_size, device=None):
        """
        Evaluate the given test data and print the pass rate
        """
        self.eval()
        passed = 0
        total = 0
        self.zero_grad()
        with torch.no_grad():
            for i_batch, (audio, labels) in enumerate(test_data.get_data_loader(batch_size)):
                batch_size = audio.shape[0]
                audio = audio.transpose(1, 0)  # GRU wants seq,batch,feature
                if device:
                    audio = audio.to(device)
                    labels = labels.to(device)
                total += batch_size
                self.init_hidden()
                keyword_scores = self(audio)
                last_accuracy, ok = self.batch_accuracy(keyword_scores, labels)
                passed += ok

        return (passed, total, passed / total)


class GRUKeywordSpotter(KeywordSpotter):
    """This class is a PyTorch Module that implements a 2 layer GRU based audio classifier"""

    def __init__(self, input_dim, num_keywords, hidden_dim):
        """
        Initialize the KeywordSpotter with the following parameters:
        input_dim - the size of the input audio frame in # samples.
        hidden_dim - the size of the hidden state of the GRU nodes
        num_keywords - the number of predictions to come out of the model.
        """
        super(GRUKeywordSpotter, self).__init__(input_dim, num_keywords)
        self.hidden_dim = hidden_dim

        # The GRU takes audio sequences as input, and outputs hidden states
        # with dimensionality hidden_dim.
        self.gru1 = nn.GRU(input_dim, hidden_dim)
        self.gru2 = nn.GRU(hidden_dim, hidden_dim)

        # The linear layer is a fully connected layer that maps from hidden state space
        # to number of expected keywords
        self.hidden2keyword = nn.Linear(hidden_dim, num_keywords)
        self.init_hidden()

    def init_hidden(self):
        """ Clear the hidden state for the GRU nodes """
        self.hidden1 = None
        self.hidden2 = None

    def forward(self, input):
        """ Perform the forward processing of the given input and return the prediction """
        # input is shape: [seq,batch,feature]
        gru_out, self.hidden1 = self.gru1(input, self.hidden1)
        gru_out, self.hidden2 = self.gru2(gru_out, self.hidden2)
        keyword_space = self.hidden2keyword(gru_out)
        result = F.log_softmax(keyword_space, dim=2)
        # return the mean across the sequence length to produce the
        # best prediction of which word exists in that sequence.
        # we can do that because we know each window_size sequence in
        # the training dataset contains at most one word.
        result = result.mean(dim=0)
        return result


class LSTMKeywordSpotter(KeywordSpotter):
    """This class is a PyTorch Module that implements a 2 layer LSTM based audio classifier"""

    def __init__(self, input_dim, num_keywords, hidden_dim):
        """
        Initialize the KeywordSpotter with the following parameters:
        input_dim - the size of the input audio frame in # samples.
        hidden_dim - the size of the hidden state of the LSTM nodes
        num_keywords - the number of predictions to come out of the model.
        """
        super(LSTMKeywordSpotter, self).__init__(input_dim, num_keywords)
        self.hidden_dim = hidden_dim
        self.input_dim = input_dim

        # The LSTM takes audio sequences as input, and outputs hidden states
        # with dimensionality hidden_dim.
        self.lstm1 = nn.LSTM(input_dim, hidden_dim)
        self.lstm2 = nn.LSTM(hidden_dim, hidden_dim)

        # The linear layer is a fully connected layer that maps from hidden state space
        # to number of expected keywords
        self.hidden2keyword = nn.Linear(hidden_dim, num_keywords)
        self.init_hidden()

    def init_hidden(self):
        """ Clear the hidden state for the LSTM nodes """
        self.hidden1 = None
        self.hidden2 = None

    def forward(self, input):
        """ Perform the forward processing of the given input and return the prediction """
        # input is shape: [seq,batch,feature]
        lstm_out, self.hidden1 = self.lstm1(input, self.hidden1)
        lstm_out, self.hidden2 = self.lstm2(lstm_out, self.hidden2)
        keyword_space = self.hidden2keyword(lstm_out)
        result = F.log_softmax(keyword_space, dim=2)
        # return the mean across the sequence length to produce the
        # best prediction of which word exists in that sequence.
        # we can do that because we know each window_size sequence in
        # the training dataset contains at most one word.
        result = result.mean(dim=0)
        return result


class AudioDataset(Dataset):
    """
    Featurized Audio in PyTorch Dataset so we can get a DataLoader that is needed for
    mini-batch training.
    """

    def __init__(self, filename, keywords):
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


def create_model(arch, input_size, num_keywords, hidden_units):
    if arch == "GRU":
        return GRUKeywordSpotter(input_size, num_keywords, hidden_units)
    elif arch == "LSTM":
        return LSTMKeywordSpotter(input_size, num_keywords, hidden_units)
    else:
        raise Exception("Model architecture '{}' not supported".format(arch))


def train(epochs=30, hidden_units=128, learning_rate=1e-3, weight_decay=1e-5, batch_size=128, filename=None,
          evaluate_only=False, categories_file=None, wav_directory=None, architecture="GRU"):

    if filename is None:
        filename = architecture + "KeywordSpotter.pt"

    # load the featurized data
    if not os.path.isdir(wav_directory):
        print("### Error: please specify valid --audio folder location")
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

    device = None
    if torch.cuda.is_available():
        device = torch.device("cuda")
        if model and device:
            model.cuda()  # move the processing to GPU
    else:
        print("### CUDA not available!!")

    if not evaluate_only:
        print("Loading {}...".format(training_file))
        training_data = AudioDataset(training_file, keywords)

        print("Loading {}...".format(validation_file))
        validation_data = AudioDataset(validation_file, keywords)

        model = create_model(architecture, training_data.input_size, training_data.num_keywords, hidden_units)
        if device:
            model.cuda()  # move the processing to GPU
        model.fit(training_data, validation_data, batch_size, epochs, learning_rate, weight_decay, device)

        passed, total, rate = model.evaluate(training_data, batch_size, device)
        print("Training accuracy = {:.3f} %".format(rate * 100))

        torch.save(model.state_dict(), filename)

    print("Loading {}...".format(testing_file))
    test_data = AudioDataset(testing_file, keywords)

    print("Evaluating GRU based keyword spotter using {} rows of featurized test audio...".format(test_data.num_rows))
    if model is None:
        msg = "Loading trained model with input size {}, hidden units {} and num keywords {}"
        print(msg.format(test_data.input_size, hidden_units, test_data.num_keywords))
        model = create_model(architecture, test_data.input_size, test_data.num_keywords, hidden_units)
        model.load_state_dict(torch.load(filename))
        if model and device:
            model.cuda()  # move the processing to GPU

    passed, total, rate = model.evaluate(test_data, batch_size, device)
    print("Testing accuracy = {:.3f} %".format(rate * 100))

    name = os.path.splitext(filename)[0] + ".onnx"
    print("saving onnx file: {}".format(name))
    model.export(name, device)
    return rate


if __name__ == '__main__':
    parser = argparse.ArgumentParser("train a GRU based neural network for keyword spotting")
    parser.add_argument("--epochs", help="Number of epochs to train (default 30)", default=30, type=int)
    parser.add_argument("--hidden_units", "-hu", help="Number of hidden units in the GRU layers (default 128)",
                        default=128, type=int)
    parser.add_argument("--learning_rate", "-lr", help="Learning rate for the RMSProp optimizer (default 1e-3)",
                        default=1e-3, type=float)
    parser.add_argument("--weight_decay", "-wd", help="Weight decay for the RMSProp optimizer (default 1e-5)",
                        default=1e-5, type=float)
    parser.add_argument("--batch_size", "-bs", help="Batch size of training (default 128)", default=128, type=int)
    parser.add_argument("--eval", "-e", help="No training, just evaluate existing model (default false)",
                        action='store_true')
    parser.add_argument("--model", "-o", help="Name of file to load/save model to/from", default=None)
    parser.add_argument("--categories", "-c", help="Name of file containing keywords", default=None)
    parser.add_argument("--audio", "-a", help="Path to the audio folder containing 'training.npz' file", default=None)
    parser.add_argument("--architecture", help="Specify model architecture (GRU, LSTM)", default="GRU")
    args = parser.parse_args()

    train(args.epochs, args.hidden_units, args.learning_rate, args.weight_decay, args.batch_size, args.model,
          args.eval, args.categories, args.audio, args.architecture)
