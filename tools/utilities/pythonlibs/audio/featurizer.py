###################################################################################################
##
##  Project:  Embedded Learning Library (ELL)
##  File:     featurizer.py
##  Authors:  Chris Lovett
##
##  Requires: Python 3.x
##
###################################################################################################
import os
import time

import numpy as np

class AudioTransform:
    """ The AudioTransform class encapsulates a compiled ELL model that is designed
    to preprocess audio data before passing to a Classifier """ 
    def __init__(self, model_path, output_window_size):
        """
        Construct a featurizer from the given ELL model.
        The featurizer will ensure it returns an even number of 
        frames to ensure it files the given output_window_size.

        model_path - the path to the ELL compiled model
        output_window_size - the classifier window size
        """
                
        self.using_map = False        
        if os.path.splitext(model_path)[1] == ".ell":
            import compute_ell_model as ell
            self.model = ell.ComputeModel(model_path)
            self.using_map = True
        else:
            import compiled_ell_model as ell
            self.model = ell.CompiledModel(model_path)
            
        self.logfile = None
        self.output_window_size = output_window_size
        self.input_size = int(self.model.input_shape.Size())
        self.output_size = int(self.model.output_shape.Size())
        self.frame_count = 0
        self.eof = True        
        self.total_time = 0

    def set_log(self, logfile):
        """ Provide optional log file for saving raw featurizer output """
        self.logfile = logfile

    def open(self, audio_source):
        """ Open the featurizer using given audio source """
        self.audio_source = audio_source
        self.frame_count = 0
        self.total_time = 0

    def read(self):
        """ Read the next output from the featurizer """
        data = self.audio_source.read()
        if data is None:
            self.eof = True
            if self.output_window_size != 0 and self.frame_count % self.output_window_size != 0:
                # keep returning zeros until we fill the window size
                self.frame_count += 1
                return np.zeros((self.output_size))
            return None

        if self.logfile:
            self.logfile.write("{}\n".format(",".join([str(x) for x in data])))
        self.frame_count += 1        
        
        start_time = time.time()
        result = self.model.transform(data)
        now = time.time()
        diff = now - start_time
        self.total_time += diff
        return result
    
    def close(self):
        self.audio_source.close()

    def avg_time(self):
        if self.frame_count == 0:
            self.frame_count = 1
        return self.total_time /  self.frame_count