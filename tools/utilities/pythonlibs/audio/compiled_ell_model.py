###################################################################################################
##
##  Project:  Embedded Learning Library (ELL)
##  File:     compiled_ell_model.py
##  Authors:  Chris Lovett
##
##  Requires: Python 3.x
##
###################################################################################################
import importlib
import os
import sys

import numpy as np

class CompiledModel:
    """ This class wraps a compiled ELL module, exposing the compiled predict function as a 
    transform method """
    def __init__(self, model_path):
        self.path = model_path
        model_dir, model_name = os.path.split(model_path)
        full_path = os.path.abspath(model_dir)
        sys.path += [full_path]
        sys.path += [os.path.join(full_path, "build")]
        sys.path += [os.path.join(full_path, "build", "Release")]
        self.module = importlib.import_module(model_name)
        self.input_shape = self.module.get_default_input_shape()
        self.output_shape = self.module.get_default_output_shape()

    def transform(self, x):
        # Turn the input into something the model can read
        in_vec = np.array(x).astype(np.float).ravel()

        if np.any(np.isnan(in_vec)):
            np.nan_to_num(in_vec, copy = False)

        # Send the input to the predict function and return the prediction result
        return self.module.predict(in_vec)

    def reload(self):
        """ reload the module, hence resetting all model state """
        self.module = importlib.reload(self.module)