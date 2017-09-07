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

class GenerateMarkdown:
    def __init__(self):
        self.arg_parser = argparse.ArgumentParser(
            "This script takes a model directory and generates a basic markdown file that describes the model\n")
        self.modeldir = None
        self.model = None
        self.outfile = None
        self.template = None

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
            raise Exception("Template file not found: " + args.template)
        with open(args.template, 'r') as f:
            self.template = f.read()

        permalink = os.path.splitext(basename(self.outfile))[0]
        self.set_value("@PERMALINK@", permalink)

    def get_model_file(self, name, is_suffix=True):
        """Returns the path to a file from the model directory, given the filename or a suffix"""
        if is_suffix:
            filename = os.path.join(self.modeldir, self.model + name)
        else:
            filename = os.path.join(self.modeldir, name)
            
        if (not os.path.isfile(filename)):
            raise Exception("File not found: " + filename)
        return filename

    def format_float(self, value):
        return "{0:.2f}".format(value)

    def set_value(self, key, value):
        self.template = self.template.replace(key, str(value))

    def write_properties(self):
        """Writes the basic properties of the model"""
        filename = self.get_model_file("_modelargs.json")
        with open(filename, 'r') as f:
            results = json.loads(f.read())
            self.set_value("@IMAGE_SIZE@", results['image_size'])
            self.set_value("@NUM_CLASSES@", results['num_classes'])
            self.set_value("@MODEL_NAME@", results['name'])

        filename = self.get_model_file(".ell.zip")
        unzip = ziptools.Extractor(filename)
        success, temp = unzip.extract_file(".ell")
        if (success):
            print("extracted: " + temp)
            size_mb = round(os.path.getsize(temp) / (1000 * 1000))
            self.set_value("@MODEL_SIZE_MB@", size_mb)
            os.remove(temp)
        else:
            # not a zip archive
            print("Error, not a valid zip archive: " + filename)

    def write_performance(self, platforms):
        """Writes the metrics for the list of platforms"""
        for platform in platforms:
            filename = self.get_model_file("_validation_" + platform + ".json")

            with open(filename, 'r') as f:
                results = json.loads(f.read())
                average_time = reduce(lambda x, y: x + float(y['avg_time']), results, 0) / len(results)
                self.set_value("@" + platform.upper() + "_SECONDS_PER_FRAME@", self.format_float(average_time))

    def sanitize_layer_string(self, layer_str):
        # these are special characters in markdown files
        s = re.sub(r'\|', r'', layer_str)
        return s

    def write_architecture(self):
        """Writes the model architecture in the desired format"""
        filename = self.get_model_file(".cntk.zip")
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
        filename = self.get_model_file("test_eval.json", is_suffix=False)

        with open(filename, 'r') as f:
            results = json.loads(f.read())
            self.set_value("@TOP_1_ACCURACY@", self.format_float(100 * (1.0 - float(results['average_top1_error']))))
            self.set_value("@TOP_5_ACCURACY@", self.format_float(100 * (1.0 - float(results['average_top5_error']))))
            self.set_value("@TOP_1_ERROR@", self.format_float(100 * float(results['average_top1_error'])))
            self.set_value("@TOP_5_ERROR@", self.format_float(100 * float(results['average_top5_error'])))

    def save(self):
        with open(self.outfile, 'w', encoding='utf-8') as of:
            of.write(self.template)
        print("Saved to: " + self.outfile)

    def run(self):
        self.write_properties()
        self.write_architecture()
        self.write_accuracy()
        self.write_performance(["pi3"])
        self.save()

if __name__ == "__main__":
    program = GenerateMarkdown()

    argv = sys.argv
    argv.pop(0) # when passed directly into parse_args, the first argument (program name) is not skipped
    program.parse_command_line(argv)

    program.run()