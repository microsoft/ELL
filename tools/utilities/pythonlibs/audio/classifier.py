###################################################################################################
#
#  Project:  Embedded Learning Library (ELL)
#  File:     classifier.py
#  Authors:  Chris Lovett
#
#  Requires: Python 3.x
#
###################################################################################################
import os
import time

import numpy as np


class AudioClassifier:
    """
    This class wraps an ELL audio classifier model and adds some nice features, like mapping the
    predictions to a string label, any category starting with "_" will be classified as null.  It also
    does some optional posterior smoothing on the predictions since audio model outputs
    tend to be rather noisy. It also supports a threshold value so any prediction less than this
    probability is ignored.
    """
    def __init__(self, model_path, categories_file, threshold=0, smoothing_window=0, ignore_list=[]):
        """
        Initialize the new AudioClassifier.
        model - the path to the ELL model module to load.
        categories_file - the path to a text file containing strings labels for each prediction
        threshold - threshold for predictions, (default 0).
        smoothing_window - controls the size of the smoothing window (defaults to 0).
        ignore_list - list of category labels to ignore (like 'background' or 'silence')
        """
        self.smoothing_window = None
        if smoothing_window is not None:
            self.smoothing_window = float(smoothing_window)

        self.threshold = threshold
        self.categories = None

        if isinstance(ignore_list, str):
            self.ignore_list = [x.trim() for x in ignore_list.split(',')]
        elif isinstance(ignore_list, list):
            self.ignore_list = ignore_list
        elif ignore_list is None:
            self.ignore_list = []
        else:
            raise Exception("Expecting ignore list to be a comma separated list or a python list of strings")

        if categories_file:
            with open(categories_file, "r") as fp:
                self.categories = [e.strip() for e in fp.readlines()]

        self.using_map = False
        if os.path.splitext(model_path)[1] == ".ell":
            import compute_ell_model as ell
            self.model = ell.ComputeModel(model_path)
            self.using_map = True
        else:
            import compiled_ell_model as ell
            self.model = ell.CompiledModel(model_path)

        self.logfile = None
        ts = self.model.input_shape
        self.input_shape = (ts.rows, ts.columns, ts.channels)
        ts = self.model.output_shape
        self.output_shape = (ts.rows, ts.columns, ts.channels)
        self.input_size = int(self.model.input_shape.Size())
        self.output_size = int(self.model.output_shape.Size())
        self.smoothing_items = []
        self.total_time = 0
        self.count = 0

    def get_metadata(self, name):
        return self.model.get_metadata(name)

    def set_log(self, logfile):
        """ provide optional log file to write the raw un-smoothed predictions to """
        self.logfile = logfile

    def predict(self, feature_data):
        """ process the given feature_data using the classifier model, and smooth
        the output.  It returns a tuple containing (prediction, probability, label) """

        start_time = time.time()
        output = self.model.transform(feature_data)
        now = time.time()
        diff = now - start_time
        self.total_time += diff
        self.count += 1

        if self.logfile:
            self.logfile.write("{}\n".format(",".join([str(x) for x in output])))

        if self.smoothing_window:
            output = self._smooth(output)

        prediction = self._get_prediction(output)
        if prediction is not None:
            label = ""
            if self.categories and prediction < len(self.categories):
                label = self.categories[prediction]
            if label not in self.ignore_list:
                return (prediction, output[prediction], label, output)

        return (None, None, None, None)

    def reset(self):
        self.model.reset()

    def clear_smoothing(self):
        self.smoothing_window = []

    def _get_prediction(self, output):
        """ handles scalar and vector predictions """
        if len(output) == 1:
            prediction = output[0]
        if np.isscalar(output):
            return 1 if output > self.threshold else None
        else:
            prediction = np.argmax(output)
            if prediction in self.ignore_list:
                return None

            if output[prediction] > self.threshold:
                return prediction
        return None

    def _smooth(self, predictions):
        """ smooth the predictions over a given window size """

        self.smoothing_items += [predictions]  # add our new item
        # trim to our smoothing window size
        if len(self.smoothing_items) > self.smoothing_window:
            del self.smoothing_items[0]

        # compute summed probabilities over this new sliding window
        return np.mean(self.smoothing_items, axis=0)

    def avg_time(self):
        """ get the average prediction time """
        if self.count == 0:
            self.count = 1
        return self.total_time / self.count
