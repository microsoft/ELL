# This is a wrapper for the compiled ELL FastGRNN model which has multiple inputs and outputs
import importlib
import os
import sys

import numpy as np


class CompiledModel:
    """
    This is a wrapper on a compiled ELL model that can also handle the FastGRNN models.
    The predict function on FastGRNN models takes 2 inputs and produces 2 outputs since the hidden
    state is exposed. This wrapper hides this hidden state as a member, and copies the new_state to
    the hidden state after each call to predict.  It also provides a reset() function which can
    clean the hidden state.
    """
    def __init__(self, model_path):
        self.module = None
        parent_dir = os.path.dirname(model_path)
        if not os.path.isdir(parent_dir):
            raise Exception("### {} is missing, please run compile.cmd".format(parent_dir))

        sys.path += [parent_dir]
        sys.path += [os.path.join(parent_dir, "build")]
        sys.path += [os.path.join(parent_dir, "build", "release")]
        sys.path += [os.path.join(parent_dir, "build", "debug")]

        model_name = os.path.basename(model_path)
        self.module = importlib.import_module(model_name)

        wrapper_name = str.capitalize(model_name) + "Wrapper"
        if not hasattr(self.module, wrapper_name):
            raise Exception("### {} is missing from the compiled module".format(wrapper_name))

        self.wrapper = getattr(self.module, wrapper_name)()
        self.input_size = self.wrapper.GetInputSize(0)
        self.input_shape = self.wrapper.GetInputShape(0)
        self.output_size = self.wrapper.GetOutputSize(0)
        self.output_shape = self.wrapper.GetOutputShape(0)
        self.output = self.module.FloatVector(self.output_size)
        self.state_size = self.wrapper.GetInputSize(1)
        if self.state_size:
            self.hidden_state = self.module.FloatVector(self.state_size)
            self.new_state = self.module.FloatVector(self.state_size)

    def __del__(self):
        del self.module

    def get_vector(self, x):
        vec = np.array(x).astype(np.float).ravel()
        if np.any(np.isnan(vec)):
            np.nan_to_num(vec, copy=False)
        return self.module.FloatVector(vec)

    def predict(self, input):
        return self.transform(input)

    def transform(self, input):
        if self.state_size:
            self.output = self.wrapper.Predict(self.get_vector(input), self.hidden_state, self.new_state)
            self.hidden_state = self.module.FloatVector(self.new_state)  # copy new state back to hidden state
        else:
            self.output = self.wrapper.Predict(self.get_vector(input))

        return np.array(self.output)

    def reset(self):
        self.hidden_state = self.module.FloatVector(self.state_size)
        self.wrapper.Reset()
