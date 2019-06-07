#!/usr/bin/env python3
###################################################################################################
#
#  Project:  Embedded Learning Library (ELL)
#  File:     make_vad.py
#  Authors:  Chris Lovett
#
#  Requires: Python 3.x
#
###################################################################################################

"""
Utility for creating ELL featurizer models
"""
import argparse
import os
import json

import find_ell_root  # noqa: F401
import ell


VAD_DEFAULTS = {
    "tau_up": 1.54,
    "tau_down": 0.074326,
    "threshold_up": 3.552713,
    "threshold_down": 0.931252,
    "large_input": 2.400160,
    "gain_att": 0.002885,
    "level_threshold": 0.007885
}


def load_vad_options(filename):
    data = {}
    if filename:
        if os.path.isfile(filename):
            with open(filename, "r") as f:
                data = json.load(f)
    for key in VAD_DEFAULTS:
        if key not in data:
            data[key] = VAD_DEFAULTS[key]
    return data


def find_metadata(map, name):
    model = map.GetModel()
    iter = model.GetNodes()
    while iter.IsValid():
        node = iter.Get()
        value = node.GetMetadataValue(name)
        if value is not None and value != '':
            return value
        iter.Next()
    return None


def make_vad(output_filename, sample_rate, input_size, output_size, vad_options):
    """
    Create a new VAD ELL model:
    output_filename     - the output ELL model file name
    sample_rate         - the sample rate of the audio
    input_size          - the number of audio samples input to featurizer
    output_size         - the featurizer output size
    vad_options         - JSON file containing VAD parameters
    """

    frame_duration = sample_rate / input_size

    # Create output directory if necessary
    output_directory = os.path.dirname(output_filename)
    if output_directory and not os.path.isdir(output_directory):
        os.makedirs(output_directory)

    if isinstance(vad_options, str):
        options = load_vad_options(vad_options)
    else:
        options = vad_options

    model = ell.model.Model()
    input = model.AddInput(ell.model.PortMemoryLayout([output_size]), ell.nodes.PortType.smallReal)
    vad_node = model.AddVoiceActivityDetector(
        input, sample_rate, frame_duration,
        float(options["tau_up"]), float(options["tau_down"]), float(options["large_input"]),
        float(options["gain_att"]), float(options["threshold_up"]), float(options["threshold_down"]),
        float(options["level_threshold"]))
    cast_node = model.AddTypeCast(vad_node, ell.nodes.PortType.smallReal)
    output = model.AddOutput(ell.model.PortMemoryLayout([1]), cast_node)

    map = ell.model.Map(model, input, output)
    map.Save(output_filename)
    print("Saved {}".format(output_filename))
    return map


if __name__ == "__main__":
    arg_parser = argparse.ArgumentParser(description="Create ELL Voice Activity Detection model")

    # options
    arg_parser.add_argument("--output_filename", "-o", help="Output model filename (default 'vad.ell')",
                            default="vad.ell")
    arg_parser.add_argument("--featurizer", "-rf", help="Featurizer ELL model to be used with this")
    arg_parser.add_argument("--options", help="JSON file containing VAD parameters")

    args = arg_parser.parse_args()

    if not os.path.isfile(args.featurizer):
        print("Featurizer '{}' not found".format(args.featurizer))

    fmap = ell.model.Map(args.featurizer)
    sample_rate = float(find_metadata(fmap, "sample_rate"))
    input_size = fmap.GetInputShape(0).Size()
    output_size = fmap.GetOutputShape(0).Size()

    make_vad(args.output_filename, sample_rate, input_size, output_size, args.options)
