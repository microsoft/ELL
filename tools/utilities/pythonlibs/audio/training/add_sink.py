#!/usr/bin/env python3

###################################################################################################
##
##  Project:  Embedded Learning Library (ELL)
##  File:     add_sink.py
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
import model_editor

if __name__ == "__main__":
    parser = argparse.ArgumentParser("Add a SinkNode to the given node in the model")
    parser.add_argument("model", help="The *.ell model to edit)")
    parser.add_argument("--node", "-n", help="A substring to match on the type of node to attach the SinkNode to", required=True)
    parser.add_argument("--function", "-f", help="The name of the callback function", required=True)
        
    args = parser.parse_args()
    filename = args.model
    print("Adding SinkNode callback '{}' to the first node type matching '{}'".format(args.function, args.node))
    editor = model_editor.ModelEditor(filename)
    
    if editor.attach_sink(args.node, args.function):
        editor.save(filename)
        print("model updated")
    else: 
        print("model unchanged")

