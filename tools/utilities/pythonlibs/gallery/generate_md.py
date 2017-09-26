####################################################################################################
##
##  Project:  Embedded Learning Library (ELL)
##  File:     generate_md.py (gallery)
##  Authors:  Lisa Ong
##
##  Requires: Python 3.x, cntk-2.0-cp35
##
####################################################################################################
import os
import sys
import argparse
import json
import re
from functools import reduce
from os.path import basename

# ELL utilities
sys.path.append(os.path.join(os.path.dirname(os.path.abspath(__file__)), '../'))
import find_ell
import ELL
import ziptools

# CNTK
from cntk import load_model
from cntk.layers.typing import *
sys.path.append(os.path.join(os.path.dirname(os.path.abspath(__file__)), '../../../importers/CNTK'))
import lib.cntk_layers as cntk_layers
import lib.cntk_utilities as cntk_utilities

# local helpers
import model_data_retriever

class GenerateMarkdown:
    def __init__(self):
        self.arg_parser = argparse.ArgumentParser(
            "This script takes a model directory and generates a basic markdown file that describes the model\n")
        self.modeldir = None
        self.model = None
        self.outfile = None
        self.template = None
        self.data_retriever = None

    def parse_command_line(self, argv):
        # required arguments
        self.arg_parser.add_argument("modeldir", help="the directory containing the model files")
        self.arg_parser.add_argument("outfile", help="path to the output filename")

        # optional arguments
        self.arg_parser.add_argument("--template", help="path to the input markdown template file", default="vision_model.md.in")
        args = self.arg_parser.parse_args(argv)

        self.modeldir = args.modeldir
        self.model = basename(self.modeldir)
        self.outfile = args.outfile

        if (not os.path.isfile(args.template)):
            raise FileNotFoundError("Template file not found: " + args.template)
        with open(args.template, 'r') as f:
            self.template = f.read()

        permalink = os.path.splitext(basename(self.outfile))[0]
        self.set_value("@PERMALINK@", permalink)

        self.data_retriever = model_data_retriever.ModelDataRetriever(self.modeldir, self.model)

    def set_value(self, key, value):
        self.template = self.template.replace(key, str(value))

    def write_properties(self):
        """Writes the basic properties of the model"""
        properties = self.data_retriever.get_model_properties()

        self.set_value("@IMAGE_SIZE@", properties['image_size'])
        self.set_value("@NUM_CLASSES@", properties['num_classes'])
        self.set_value("@MODEL_NAME@", self.model)
        self.set_value("@MODEL_SIZE_MB@", properties['size_mb'])

    def write_performance(self, platforms):
        """Writes the metrics for the list of platforms"""
        speed = self.data_retriever.get_model_seconds_per_frame(platforms)

        for platform in platforms:
            try:
                average_time = speed[platform]
                self.set_value("@" + platform + "_SECONDS_PER_FRAME@", average_time)
            except:
                # leave entries empty if file isn't found, orinvalid json (validation not complete on the target)
                self.set_value("@" + platform + "_SECONDS_PER_FRAME@", "")
                pass

    def sanitize_layer_string(self, layer_str):
        # these are special characters in markdown files
        s = re.sub(r'\|', r'', layer_str)
        return s

    def write_architecture(self):
        """Writes the model architecture in the desired format"""
        filename = os.path.join(self.modeldir, self.model + ".cntk.zip")
        if (not os.path.isfile(filename)):
            raise FileNotFoundError("File not found: " + filename)

        unzip = ziptools.Extractor(filename)
        success, temp = unzip.extract_file(".cntk")
        if (success):
            print("extracted: " + temp)
            model_root = load_model(temp)
            model_layers = cntk_utilities.get_model_layers(model_root)
            layers = cntk_layers.get_filtered_layers_list(model_layers)

            result = reduce(lambda x, y: x + self.sanitize_layer_string(y.__str__()) + "<br>", layers, "")
            self.set_value("@MODEL_ARCH@", result)
            os.remove(temp)
        else:
            # not a zip archive
            print("Error, not a valid zip archive: " + filename)

    def write_accuracy(self):
        """Writes the accuracy from the model test result"""
        accuracy = self.data_retriever.get_model_topN_accuracies()

        self.set_value("@TOP_1_ACCURACY@", accuracy['top1'])
        self.set_value("@TOP_5_ACCURACY@", accuracy['top5'])

    def save(self):
        with open(self.outfile, 'w', encoding='utf-8') as of:
            of.write(self.template)
        print("Saved to: " + self.outfile)

    def run(self):
        self.write_properties()
        self.write_architecture()
        self.write_accuracy()
        self.write_performance(["pi3", "pi3_64", "aarch64"])
        self.save()

if __name__ == "__main__":
    program = GenerateMarkdown()

    argv = sys.argv
    argv.pop(0) # when passed directly into parse_args, the first argument (program name) is not skipped
    program.parse_command_line(argv)

    program.run()