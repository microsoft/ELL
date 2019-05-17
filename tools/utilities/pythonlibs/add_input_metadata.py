#!/usr/bin/env python3
###################################################################################################
#
#  Project:  Embedded Learning Library (ELL)
#  File:     add_input_metadata.py
#  Authors:  Mason Remy
#
#  Requires: Python 3.x
#
###################################################################################################

import sys
import os
import argparse
import find_ell

ell_root = find_ell.get_ell_root()
sys.path += [os.path.join(ell_root, "build", "interfaces", "python", "package")]

import ell


def get_input_node(model):
    """ return the model input node """
    iter = model.GetNodes()
    while iter.IsValid():
        node = iter.Get()
        if "InputNode" in node.GetRuntimeTypeName():
            return node
        iter.Next()
    return None


def add_input_metadata(model_filename, metadata_dict):
    model_map = ell.model.Map(model_filename)
    model = model_map.GetModel()
    input_node = get_input_node(model)
    if input_node:
        for key in metadata_dict:
            key = str(key)
            value = str(metadata_dict[key])
            print("Adding metadata {}={}".format(key, value))
            input_node.SetMetadataValue(key, value)
        model_map.Save(model_filename)
    else:
        print("No InputNode found in model")


if __name__ == "__main__":
    parser = argparse.ArgumentParser("Add a metadata key/value pair to the input node of the given model")
    parser.add_argument("model", help="The *.ell model to edit")
    parser.add_argument("--names", "-n", help="One or more metadata keys", nargs="+")
    parser.add_argument("--values", "-v", help="The same number of metadata values", nargs="+")

    args = parser.parse_args()
    metadata_key_value = zip(args.names, args.values)
    metadata_dict = {key: value for key, value in metadata_key_value}
    add_input_metadata(args.model, metadata_dict)
