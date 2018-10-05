#!/usr/bin/env python3

###################################################################################################
##
##  Project:  Embedded Learning Library (ELL)
##  File:     add_vad.py
##  Authors:  Chris Lovett
##
##  Requires: Python 3.x
##
###################################################################################################

import argparse
import os
import sys
import time

import numpy as np
import find_ell_root
import ell

class ModelEditor:
    """ Helper class that can modify an ELL model by inserting AddVoiceActivityDetectorNodes at the right place """
    
    def __init__(self, sample_rate, window_size):
        """
        Create new ModelEditor class with predefined VoiceActivityDetectorNode configuration and 
        a given audio sample rate and window size.
        """
        # Precomputed VAD constants that work well on most audio input.
        self.tau_up = 1.54
        self.tau_down = 0.074326
        self.large_input = 2.400160
        self.gain_att = 0.002885
        self.threshold_up = 3.552713
        self.threshold_down = 0.931252
        self.level_threshold = 0.007885
        self.sample_rate = sample_rate
        self.window_size = window_size
        self.vad_node = None
        self.model = None
        self.builder = None
        self.frame_duration = float(window_size) / float(sample_rate)

    def add_vad(self, lstm):
        """
        Add a VoiceActivityDetectorNode as the "resetTrigger" input to the given LSTM or GRU node.
        """
        reset_port = lstm.GetInputPort("resetTrigger")
        name = reset_port.GetParentNodes().Get().GetRuntimeTypeName()
        if not "VoiceActivityDetector" in name:
            # replace dummy trigger with VAD node
            if not self.vad_node:
                input_port = lstm.GetInputPort("input")
                input_node = input_port.GetParentNodes().Get()
                self.vad_node = self.builder.AddVoiceActivityDetectorNode(self.model,                 
                                ell.nodes.PortElements(input_node.GetOutputPort("output")),
                                self.sample_rate, self.frame_duration, self.tau_up, self.tau_down, 
                                self.large_input, self.gain_att, self.threshold_up, 
                                self.threshold_down, self.level_threshold)
            # make the vad node the "resetTrigger" input of this lstm node
            reset_output = ell.nodes.PortElements(self.vad_node.GetOutputPort("output"))
            self.builder.ResetInput(lstm, reset_output, "resetTrigger")
            return True
        return False

    def process_model(self, filename):
        """
        Process the given ELL model and insert VoiceActivityDetectorNode as the reset trigger for
        any LSTM or GRU nodes found in the model.
        """
        # load the ELL model.
        map = ell.model.Map(filename)
        self.model = map.GetModel()
        self.builder = ell.model.ModelBuilder()
        iter = self.model.GetNodes()
        changed = False
        found = False
        while iter.IsValid():
            node = iter.Get()
            name = node.GetRuntimeTypeName()
            if "GRU" in name or "LSTM" in name:
                found = True
                changed |= self.add_vad(node)
            iter.Next() 

        if changed:
            print("model updated")
            map.Save(filename)
        elif not found:            
            print("model does not contain GRU or LSTM nodes")
        else:
            print("model unchanged")



if __name__ == "__main__":
    parser = argparse.ArgumentParser("Fix the given ELL model by replacing dummy reset trigger " + 
        "nodes with VoiceActivityDetectorNodes in an ELL Audio Classifier model")
    parser.add_argument("model", help="The *.ell model to edit)")
    parser.add_argument("--sample_rate", "-sr", type=int, help="The audio sample rate in Hz (default 16000)", default=16000)
    parser.add_argument("--window_size", "-ws", type=int, help="The audio input window size (default 512)", default=512)
    
    args = parser.parse_args()
    filename = args.model
    print("Adding VAD node with sample rate {} and window size {} to {}".format(args.sample_rate, args.window_size, args.model))
    editor = ModelEditor(args.sample_rate, args.window_size)
    editor.process_model(args.model)
