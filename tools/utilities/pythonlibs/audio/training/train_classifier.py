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
from torch.autograd import Variable
import torch.onnx
from torch.utils.data import Dataset, DataLoader


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
        decayed_range = (self.lr_max - self.lr_min) * self.gamma ** it
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

    def fit(self, training_data, validation_data, options, device=None, detail=False):
        """
        Perform the training.  This is not called "train" because the base class already defines
        that method with a different meaning.  The base class "train" method puts the Module into
        "training mode".
        """
        print("Training {} using {} rows of featurized training input...".format(self.name(), training_data.num_rows))
        start = time.time()
        loss_function = nn.NLLLoss()
        initial_rate = options.learning_rate
        lr_scheduler = options.lr_scheduler
        optimizer = optim.RMSprop(self.parameters(), lr=initial_rate, weight_decay=options.weight_decay)

        num_epochs = options.epochs
        batch_size = options.batch_size
        learning_rate = options.learning_rate
        lr_min = options.lr_min
        lr_peaks = options.lr_peaks
        ticks = training_data.num_rows / batch_size  # iterations per epoch
        total_iterations = ticks * num_epochs
        gamma_iterations = total_iterations
        if lr_peaks > 1:
            gamma_iterations /= lr_peaks

        if not lr_min:
            lr_min = learning_rate
        # compute the gamma for those schedulers that take a gamma such that the decay results
        # in us reaching the desired minimum learning rate after the right number of iterations.
        gamma = math.exp(math.log(lr_min / learning_rate) / gamma_iterations)

        scheduler = None
        if lr_scheduler == "TriangleLR":
            steps = options.lr_peaks * 2 + 1
            stepsize = num_epochs / steps
            # we need a larger gamma so the decline is not so dramatic.
            gamma_iterations = total_iterations * 1.25
            gamma = math.exp(math.log(lr_min / learning_rate) / gamma_iterations)
            scheduler = TriangularLR(optimizer, stepsize * ticks, lr_min, learning_rate, gamma)
        elif lr_scheduler == "CosineAnnealingLR":
            # divide by odd number to finish on the minimum learning rate
            cycles = options.lr_peaks * 2 + 1
            scheduler = optim.lr_scheduler.CosineAnnealingLR(optimizer, T_max=total_iterations / cycles,
                                                             eta_min=lr_min)
        elif lr_scheduler == "ExponentialLR":
            scheduler = optim.lr_scheduler.ExponentialLR(optimizer, gamma)
        elif lr_scheduler == "ExponentialResettingLR":
            reset = (num_epochs * ticks) / 3  # reset at the 1/3 mark.
            scheduler = ExponentialResettingLR(optimizer, gamma, reset)

        # optimizer = optim.Adam(model.parameters(), lr=0.0001)
        log = []

        for epoch in range(num_epochs):
            self.train()
            iteration = 0
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
                iteration += 1

            # Find the best prediction in each sequence and return it's accuracy
            passed, total, rate = self.evaluate(validation_data, batch_size, device)
            learning_rate = optimizer.param_groups[0]['lr']
            print("Epoch {}, Loss {}, Validation Accuracy {:.3f}, Learning Rate {}".format(
                  epoch, loss.item(), rate * 100, learning_rate))
            log += [{'epoch': epoch, 'loss': loss.item(), 'accuracy': rate, 'learning_rate': learning_rate}]

        end = time.time()
        print("Trained in {:.2f} seconds".format(end - start))
        return log

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
    """This class is a PyTorch Module that implements a 1, 2 or 3 layer GRU based audio classifier"""

    def __init__(self, input_dim, num_keywords, hidden_dim, num_layers):
        """
        Initialize the KeywordSpotter with the following parameters:
        input_dim - the size of the input audio frame in # samples.
        hidden_dim - the size of the hidden state of the GRU nodes
        num_keywords - the number of predictions to come out of the model.
        num_layers - the number of GRU layers to use (1, 2 or 3)
        """
        self.hidden_dim = hidden_dim
        self.num_layers = num_layers
        super(GRUKeywordSpotter, self).__init__(input_dim, num_keywords)

        # The GRU takes audio sequences as input, and outputs hidden states
        # with dimensionality hidden_dim.
        self.gru1 = nn.GRU(input_dim, hidden_dim)
        self.gru2 = None
        if num_layers > 1:
            self.gru2 = nn.GRU(hidden_dim, hidden_dim)
        self.gru3 = None
        last_output_size = hidden_dim
        if num_layers > 2:
            self.gru3 = nn.GRU(hidden_dim, num_keywords)
            last_output_size = num_keywords

        # The linear layer is a fully connected layer that maps from hidden state space
        # to number of expected keywords
        self.hidden2keyword = nn.Linear(last_output_size, num_keywords)
        self.init_hidden()

    def name(self):
        return "{} layer GRU {}".format(self.num_layers, self.hidden_dim)

    def init_hidden(self):
        """ Clear the hidden state for the GRU nodes """
        self.hidden1 = None
        self.hidden2 = None
        self.hidden3 = None

    def forward(self, input):
        """ Perform the forward processing of the given input and return the prediction """
        # input is shape: [seq,batch,feature]
        gru_out, self.hidden1 = self.gru1(input, self.hidden1)
        if self.gru2 is not None:
            gru_out, self.hidden2 = self.gru2(gru_out, self.hidden2)
        if self.gru3 is not None:
            gru_out, self.hidden3 = self.gru3(gru_out, self.hidden3)

        keyword_space = self.hidden2keyword(gru_out)
        result = F.log_softmax(keyword_space, dim=2)
        # return the mean across the sequence length to produce the
        # best prediction of which word exists in that sequence.
        # we can do that because we know each window_size sequence in
        # the training dataset contains at most one word.
        result = result.mean(dim=0)
        return result


class LSTMKeywordSpotter(KeywordSpotter):
    """This class is a PyTorch Module that implements a 1, 2 or 3 layer LSTM based audio classifier"""

    def __init__(self, input_dim, num_keywords, hidden_dim, num_layers):
        """
        Initialize the KeywordSpotter with the following parameters:
        input_dim - the size of the input audio frame in # samples.
        hidden_dim - the size of the hidden state of the LSTM nodes
        num_keywords - the number of predictions to come out of the model.
        num_layers - the number of LSTM layers to use (1, 2 or 3)
        """
        self.hidden_dim = hidden_dim
        self.num_layers = num_layers
        self.input_dim = input_dim
        super(LSTMKeywordSpotter, self).__init__(input_dim, num_keywords)

        # The LSTM takes audio sequences as input, and outputs hidden states
        # with dimensionality hidden_dim.
        self.lstm1 = nn.LSTM(input_dim, hidden_dim)
        self.lstm2 = None
        if num_layers > 1:
            self.lstm2 = nn.LSTM(hidden_dim, hidden_dim)
        self.lstm3 = None
        last_output_size = hidden_dim
        if num_layers > 2:
            # layer 3 can reduce output to num_keywords, this makes for a smaller
            # layer and a much smaller Linear layer below so we get some of the
            # size back.
            self.lstm3 = nn.LSTM(hidden_dim, num_keywords)
            last_output_size = num_keywords

        # The linear layer is a fully connected layer that maps from hidden state space
        # to number of expected keywords
        self.hidden2keyword = nn.Linear(last_output_size, num_keywords)
        self.init_hidden()

    def name(self):
        return "{} layer LSTM {}".format(self.num_layers, self.hidden_dim)

    def init_hidden(self):
        """ Clear the hidden state for the LSTM nodes """
        self.hidden1 = None
        self.hidden2 = None
        self.hidden3 = None

    def forward(self, input):
        """ Perform the forward processing of the given input and return the prediction """
        # input is shape: [seq,batch,feature]
        lstm_out, self.hidden1 = self.lstm1(input, self.hidden1)
        if self.lstm2 is not None:
            lstm_out, self.hidden2 = self.lstm2(lstm_out, self.hidden2)
        if self.lstm3 is not None:
            lstm_out, self.hidden3 = self.lstm3(lstm_out, self.hidden3)

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


def create_model(arch, input_size, num_keywords, hidden_units, num_layers):
    if arch == "GRU":
        return GRUKeywordSpotter(input_size, num_keywords, hidden_units, num_layers)
    elif arch == "LSTM":
        return LSTMKeywordSpotter(input_size, num_keywords, hidden_units, num_layers)
    else:
        raise Exception("Model architecture '{}' not supported".format(arch))


class TrainingOptions:
    def __init__(self):
        self.epochs = 30
        self.hidden_units = 128
        self.lr_scheduler = None
        self.learning_rate = 1e-3
        self.lr_min = 1e-5
        self.lr_peaks = 0
        self.weight_decay = 1e-5
        self.batch_size = 128
        self.architecture = "GRU"
        self.num_layers = 2
        self.job_id = None

    def set(self, name, value):
        if name not in self.__dict__:
            return
        t = self.__dict__[name]
        if type(t) == int:
            self.__dict__[name] = int(value)
        if type(t) == float:
            self.__dict__[name] = float(value)
        if type(t) == str:
            self.__dict__[name] = str(value)
        else:
            self.__dict__[name] = value

    def load(self, filename):
        with open(filename, "r") as f:
            loaded_options = json.load(f)
            for k in self.__dict__:
                if k in loaded_options:
                    self.set(k, loaded_options[k])


def save_json(obj, filename):
    with open(filename, "w") as f:
        json.dump(obj, f, indent=2)


def train(options, filename=None, evaluate_only=False, categories_file=None, wav_directory=None, outdir=".",
          detail=False):

    epochs = options.epochs
    batch_size = options.batch_size
    hidden_units = options.hidden_units
    architecture = options.architecture
    num_layers = options.num_layers

    valid_layers = [1, 2, 3]
    if num_layers not in valid_layers:
        raise Exception("--num_layers can only be one of these values {}".format(valid_layers))

    if not os.path.isdir(outdir):
        os.makedirs(outdir)

    if filename is None:
        filename = os.path.join(outdir, "{}{}KeywordSpotter.pt".format(architecture, hidden_units))

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

    print("Loading {}...".format(testing_file))
    test_data = AudioDataset(testing_file, keywords)

    log = None
    if not evaluate_only:
        print("Loading {}...".format(training_file))
        training_data = AudioDataset(training_file, keywords)

        print("Loading {}...".format(validation_file))
        validation_data = AudioDataset(validation_file, keywords)

        model = create_model(architecture, training_data.input_size, training_data.num_keywords, hidden_units,
                             num_layers)
        if device:
            model.cuda()  # move the processing to GPU

        start = time.time()
        log = model.fit(training_data, validation_data, options, device, detail)
        end = time.time()

        passed, total, rate = model.evaluate(training_data, batch_size, device)
        print("Training accuracy = {:.3f} %".format(rate * 100))

        torch.save(model.state_dict(), filename)

    print("Evaluating {} keyword spotter using {} rows of featurized test audio...".format(
          architecture, test_data.num_rows))
    if model is None:
        msg = "Loading trained model with input size {}, hidden units {} and num keywords {}"
        print(msg.format(test_data.input_size, hidden_units, test_data.num_keywords))
        model = create_model(architecture, test_data.input_size, test_data.num_keywords, hidden_units, num_layers)
        model.load_state_dict(torch.load(filename))
        if model and device:
            model.cuda()  # move the processing to GPU

    passed, total, rate = model.evaluate(test_data, batch_size, device)
    print("Testing accuracy = {:.3f} %".format(rate * 100))

    name = os.path.splitext(filename)[0] + ".onnx"
    print("saving onnx file: {}".format(name))
    model.export(name, device)

    logdata = {
        "accuracy": rate,
        "architecture": architecture,
        "batch_size": batch_size,
        "epochs": epochs,
        "filename": os.path.basename(filename),
        "hidden_size": hidden_units,
        "lr_scheduler": options.lr_scheduler,
        "learning_rate": options.learning_rate,
        "weight_decay": options.weight_decay,
        "lr_min": options.lr_min,
        "lr_peaks": options.lr_peaks,
        "num_layers": num_layers,
        "training_time": end - start,
        "auto_scale": True,
        "sample_rate": test_data.sample_rate,
        "input_size": test_data.audio_size,
        "num_filters": test_data.input_size,  # input to classifier size
        "window_size": test_data.window_size,
        "shift": test_data.shift,
        "log": log
    }
    logname = os.path.join(os.path.dirname(filename), "train_results.json")
    save_json(logdata, logname)

    return rate, log


if __name__ == '__main__':
    options = TrainingOptions()
    parser = argparse.ArgumentParser("train a GRU based neural network for keyword spotting")

    # all the training parameters
    parser.add_argument("--epochs", help="Number of epochs to train", default=options.epochs, type=int)
    parser.add_argument("--hidden_units", "-hu", help="Number of hidden units in the GRU layers",
                        default=options.hidden_units, type=int)
    parser.add_argument("--lr_scheduler", help="Type of learning rate scheduler (None, TriangleLR, CosineAnnealingLR,"
                                               " ExponentialLR, ExponentialResettingLR)",
                        default=options.lr_scheduler)
    parser.add_argument("--learning_rate", help="Default learning rate, and maximum for schedulers",
                        default=options.learning_rate, type=float)
    parser.add_argument("--lr_min", help="Minimum learning rate for the schedulers",
                        default=options.lr_min, type=float)
    parser.add_argument("--lr_peaks", help="Number of peaks for triangle and cosine schedules",
                        default=options.lr_peaks, type=float)
    parser.add_argument("--batch_size", "-bs", help="Batch size of training",
                        default=options.batch_size, type=int)
    parser.add_argument("--architecture", help="Specify model architecture (GRU, LSTM)",
                        default=options.architecture)
    parser.add_argument("--num_layers", type=int, help="Number of RNN layers (1, 2 or 3)",
                        default=options.num_layers)

    # or you can just specify an options file.
    parser.add_argument("--options", help="Use json file containing all these options")

    # and some additional stuff ...
    parser.add_argument("--eval", "-e", help="No training, just evaluate existing model",
                        action='store_true')
    parser.add_argument("--model", "-o", help="Name of file to load/save model to/from", default=None)
    parser.add_argument("--categories", "-c", help="Name of file containing keywords", default=None)
    parser.add_argument("--audio", "-a", help="Path to the audio folder containing 'training.npz' file", default=None)
    parser.add_argument("--outdir", help="Folder in which to store output file and log files", default=".")
    parser.add_argument("--detail", "-d", help="Save loss info for every iteration not just every epoch",
                        action="store_true")
    args = parser.parse_args()

    options.epochs = args.epochs
    options.hidden_units = args.hidden_units
    options.learning_rate = args.learning_rate
    options.lr_min = args.lr_min
    options.lr_peaks = args.lr_peaks
    options.lr_scheduler = args.lr_scheduler
    options.batch_size = args.batch_size
    options.architecture = args.architecture
    options.num_layers = args.num_layers

    if args.options:
        options.load(args.options)

    train(options, args.model, args.eval, args.categories, args.audio, args.outdir, args.detail)
