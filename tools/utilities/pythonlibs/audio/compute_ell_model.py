###################################################################################################
##
##  Project:  Embedded Learning Library (ELL)
##  File:     compute_ell_model.py
##  Authors:  Chris Lovett
##
##  Requires: Python 3.x
##
###################################################################################################
import importlib
import os
import sys

import numpy as np

try:
    # try and import ELL from site_packages
    import ell
except:
    # try and use ELL_ROOT environment variable to find ELL.
    ell_root = os.getenv("ELL_ROOT")
    if not ell_root:
        raise Exception("Please set your ELL_ROOT environment variable")
    sys.path += [ os.path.join(ell_root, "build", "interfaces", "python", "package")]
    import ell


class ComputeModel:
    """ This class wraps a .ell model, exposing the model compute function as a 
    transform method """
    def __init__(self, model_path):

        self.model_path = model_path
        self.map = ell.model.Map(model_path)
        self.input_shape = self.map.GetInputShape()
        self.output_shape = self.map.GetOutputShape()

    def transform(self, x):
        # Turn the input into something the model can read
        in_vec = np.array(x).astype(np.float32).ravel()

        # Send the input to the predict function and return the prediction result
        return np.array(self.map.Compute(in_vec, dtype=np.float32))

    def reload(self):
        """ reload the map, hence resetting all model state """
        self.map = ell.model.Map(model_path)