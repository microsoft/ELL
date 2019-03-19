###################################################################################################
#
#  Project:  Embedded Learning Library (ELL)
#  File:     dataset.py
#  Authors:  Chris Lovett, Chuck Jacobs
#
#  Requires: Python 3.x
#
###################################################################################################

import numpy as np


class Dataset(object):
    """
    Dataset class provides help with managing generic labelled datasets that are saved in numpy .npz format.
    A labelled dataset contains a "truth" label for each row of data that can be used in a supervised
    training process.
    """
    def __init__(self, features, label_names, valid_classes, parameters):
        """
        Create new Dataset from the given features and label names and parameters.  The parameters
        can be any tuple that is application specific.  For example, in the case of audio one might
        want to store the sample rate, window size and other audio specific parameters associated
        with the featurized data.  The valid_classes list all possible labels and label_names might only
        mention a subset of these.
        """

        # Keep rows with finite entries (there are lots of NaNs initially)
        good_rows = np.isfinite(features).all(axis=1)
        features = features[good_rows]
        label_names = label_names[good_rows]

        self.file_name = None
        self.features = features
        self.labels = None  # computable labels ([0,1] for binary classification, 1-hot vector for multiclass)
        self.raw_labels = None  # numeric labels ([0, num_classes])
        self.label_names = label_names  # textual labels
        self.categories = None  # map from textual -> numeric label
        self.category_names = None  # map from numeric -> textual label
        self.class_weights = None
        self.class_distribution = None
        self.parameters = parameters
        self.valid_classes = valid_classes

        self._init()

    def _init(self):

        # Process features
        self.mean = np.mean(self.features, axis=0)

        std = np.std(self.features, axis=0)
        std[np.equal(std, 0)] = 1e-5
        self.std = std

        self._process_labels()
        self._compute_class_weights()
        self._compute_sample_weights()

    @staticmethod
    def load(filename):
        """ Load a dataset from a numpy file.  """
        with np.load(filename) as data:
            features = data["features"]
            label_names = data["labels"]
            parameters = data["parameters"]
            valid_classes = data["valid_classes"]

        result = Dataset(features, label_names, valid_classes, parameters)
        result.file_name = filename
        return result

    def save(self, filename):
        """ Save the dataset fo a numpy .npz file """
        self.file_name = filename
        np.savez(filename, features=self.features, labels=self.label_names, valid_classes=self.valid_classes,
                 parameters=self.parameters)

    def _process_labels(self):
        # Generate map from name -> label value, starting with index 1 (reserving 0 for 'null')
        self.categories = {item[0]: item[1] for item in zip(self.valid_classes, range(len(self.valid_classes)))}
        self.category_names = {d[1]: d[0] for d in self.categories.items()}  # map from label value -> name
        self.num_classes = len(self.categories)

        self.raw_labels = np.array([self.categories[n] for n in self.label_names])
        self.labels = self.raw_labels

        # Deal with multiclass datasets, create a vector [0,1,0,0,0,...] for each expected label row
        # where the 1 in this vector is the location of the expected label.
        self.multiclass = self.num_classes > 2
        if self.multiclass:
            self.labels = self.to_categorical(self.raw_labels, self.num_classes)

    def _compute_class_weights(self):
        total_entries = len(self.raw_labels)
        unique_labels = set(self.raw_labels)
        self.class_distribution = {l: 0 for l in set(unique_labels)}  # init to zero
        for label in self.raw_labels:
            self.class_distribution[label] += 1
        self.class_weights = {label: (total_entries / self.class_distribution[label]) for label in unique_labels}

    def _compute_sample_weights(self):
        self.sample_weights = np.zeros(len(self.raw_labels))
        for index in range(len(self.raw_labels)):
            self.sample_weights[index] = self.class_weights[self.raw_labels[index]]

    def normalize(self):
        """ Normalize the dataset by subtracting the mean and dividing by the standard deviation """
        self.features = (self.features - self.mean) / self.std

    def to_categorical(self, labels, num_categories):
        """
        Convert the labels to vector format useful for training, for example the label 3 out of 10
        possible categories would become this vector: "0 0 0 1 0 0 0 0 0 0 0".
        """
        labels_arr = np.array(labels, dtype="int").ravel()
        num_rows = len(labels_arr)
        result = np.zeros((num_rows, num_categories))
        result[np.arange(num_rows), labels_arr] = 1
        return result
