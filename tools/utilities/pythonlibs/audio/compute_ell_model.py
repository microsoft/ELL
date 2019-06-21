###################################################################################################
#
#  Project:  Embedded Learning Library (ELL)
#  File:     compute_ell_model.py
#  Authors:  Chris Lovett
#
#  Requires: Python 3.x
#
###################################################################################################
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
    sys.path += [os.path.join(ell_root, "build", "interfaces", "python", "package")]
    import ell


class ComputeModel:
    """ This class wraps a .ell model, exposing the model compute function as a
    transform method """
    def __init__(self, model_path):

        self.model_path = model_path
        self.map = ell.model.Map(model_path)
        self.input_shape = self.map.GetInputShape()
        self.output_shape = self.map.GetOutputShape()
        self.state_size = 0
        if self.map.NumInputs() == 2 and self.map.NumOutputs() == 2:
            # then perhaps we have a FastGRNN model with external state.
            self.input_size = self.input_shape.Size()
            self.output_size = self.output_shape.Size()
            self.state_size = self.map.GetInputShape(1).Size()
            self.output_buffer = ell.math.FloatVector(self.output_size)
            self.hidden_state = ell.math.FloatVector(self.state_size)
            self.new_state = ell.math.FloatVector(self.state_size)

    def predict(self, x):
        return self.transform(x)

    def transform(self, x):
        """ call the ell model with input array 'x' and return the output as numpy array """
        # Send the input to the predict function and return the prediction result
        if self.state_size:
            i = ell.math.FloatVector(x)
            self.map.ComputeMultiple([i, self.hidden_state], [self.output_buffer, self.new_state])
            self.hidden_state.copy_from(self.new_state)
            out_vec = self.output_buffer
        else:
            out_vec = self.map.Compute(x)
        return np.array(out_vec)

    def reset(self):
        """ reset all model state """
        self.map.Reset()
        if self.state_size:
            self.hidden_state = ell.math.FloatVector(self.state_size)

    def get_metadata(self, name):
        model = self.map.GetModel()
        value = self.map.GetMetadataValue(name)
        if value:
            return value
        value = model.GetMetadataValue(name)
        if value:
            return value
        nodes = model.GetNodes()
        while nodes.IsValid():
            node = nodes.Get()
            value = node.GetMetadataValue(name)
            if value:
                return value
            nodes.Next()
        return None
