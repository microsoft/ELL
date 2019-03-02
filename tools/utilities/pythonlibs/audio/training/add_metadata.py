#!/usr/bin/env python3
###################################################################################################
#
#  Project:  Embedded Learning Library (ELL)
#  File:     add_metadata.py
#  Authors:  Chris Lovett
#
#  Requires: Python 3.x
#
###################################################################################################

import argparse

import model_editor


if __name__ == "__main__":
    parser = argparse.ArgumentParser("Add a metadata key/value pair to the input node of the given model")
    parser.add_argument("model", help="The *.ell model to edit)")
    parser.add_argument("--name", "-n", help="The metadata key", default=None)
    parser.add_argument("--value", "-v", help="The metadata value", default=None)

    args = parser.parse_args()
    filename = args.model
    print("Adding metadata {}={}".format(args.name, args.value))
    editor = model_editor.ModelEditor(filename)
    node = editor.get_input_node()
    if node is not None:
        node.SetMetadataValue(args.name, args.value)
        editor.save(filename)
    else:
        print("No InputNode found in model")
