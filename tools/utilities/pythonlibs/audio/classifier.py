###################################################################################################
##
##  Project:  Embedded Learning Library (ELL)
##  File:     classifier.py
##  Authors:  Chris Lovett
##
##  Requires: Python 3.x
##
###################################################################################################
import os
import time

import numpy as np

class AudioClassifier:
    """
    This class wraps an ELL audio classifier model and adds some nice features, like mapping the
    predictions to a string label, adding an ignore_list which specifies any predictions that are
    meaningless and it does some posterior smoothing on the predictions since audio model outputs 
    tend to be rather noisy. It also supports a threshold value so any prediction less than this
    probability is ignored.
    """
    def __init__(self, model_path, categories_file, ignore_list=[], threshold=0, smoothing_delay=0.2):
        """
        Initialize the new AudioClassifier.
        model - the path to the ELL model module to load.
        categories_file - the path to a text file containing strings labels for each prediction
        ignore_list - a list of prediction indices to ignore
        threshold - threshold for predictions, (default 0).
        smoothing_delay - controls the size of this window (defaults to 0.2 seconds).
        """
        self.smoothing_delay = smoothing_delay
        self.ignore_list = ignore_list
        if not self.ignore_list:
            self.ignore_list = []
        self.threshold = threshold
        self.categories = None
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
        self.input_size = int(self.model.input_shape.Size())
        self.output_size = int(self.model.output_shape.Size())
        self.items = []
        self.start_time = None
        self.total_time = 0
        self.count = 0

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

        output = self._smooth(output)
        
        prediction = self._get_prediction(output)
        if prediction and prediction not in self.ignore_list:
            label = ""
            if self.categories and prediction < len(self.categories):
                label = self.categories[prediction]
            return (prediction, output[prediction], label)

        return (None, None, None)

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
        """ smooth the predictions over a time delay window """
        now = time.time()
        # if we get more than 1 second delay then reset our state
        if self.start_time is None or now > self.start_time + 1:
            self.start_time = now
            self.items = []
                
        # trim to our delay window
        new_items = [x for x in self.items if x[0] + self.smoothing_delay >= now ]
        new_items += [ (now, predictions) ] # add our new item
        self.items = new_items
        
        # compute summed probabilities over this new sliding window
        sum = np.sum([p[1] for p in new_items], axis=0)
        return sum / len(new_items)


    def avg_time(self):
        if self.count == 0:
            self.count = 1
        return self.total_time /  self.count
