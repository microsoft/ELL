#!/usr/bin/env python3
###################################################################################################
#
#  Project:  Embedded Learning Library (ELL)
#  File:     vad.py
#  Authors:  Chris Lovett, Ivan Tashev
#
#  Requires: Python 3.x
#
###################################################################################################
import os


class VoiceActivityDetector:
    def __init__(self, model):

        if isinstance(model, str):
            if os.path.splitext(model)[1] == ".ell":
                import compute_ell_model as ell
                self.model = ell.ComputeModel(model)
                self.using_map = True
            else:
                import compiled_ell_model as ell
                self.model = ell.CompiledModel(model)
        else:
            import compute_ell_model as ell
            self.model = ell.ComputeModel(model)
            self.using_map = True

    def predict(self, input):
        output = list(self.model.predict(input))
        return output[0]
