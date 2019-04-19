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
    parser.add_argument("--names", "-n", help="One or more metadata keys", nargs="+")
    parser.add_argument("--values", "-v", help="The same number of metadata values", nargs="+")

    args = parser.parse_args()
    filename = args.model
    editor = model_editor.ModelEditor(filename)
    node = editor.get_input_node()
    if node is not None:
        for i in range(len(args.names)):
            print("Adding metadata {}={}".format(args.names[i], args.values[i]))
            node.SetMetadataValue(args.names[i], args.values[i])
        editor.save(filename)
    else:
        print("No InputNode found in model")
