####################################################################################################
##
##  Project:  Embedded Learning Library (ELL)
##  File:     generate_md.py
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
import zipfile
from cntk import load_model
from cntk.layers.typing import *
from functools import reduce
from os.path import basename

class GenerateMarkdown:
    def __init__(self):
        self.arg_parser = argparse.ArgumentParser(
            "This script takes a model directory and generates a basic markdown file that describes the model\n")
        self.modeldir = None
        self.model = None
        self.outfile = None

    def parse_command_line(self, argv):
        # required arguments
        self.arg_parser.add_argument("modeldir", help="the directory containing the model files")
        self.arg_parser.add_argument("outfile", help="path to the output filename")

        argv.pop(0) # when passed directly into parse_args, the first argument (program name) is not skipped
        args = self.arg_parser.parse_args(argv)

        self.modeldir = args.modeldir
        self.model = basename(self.modeldir)
        self.outfile = open(args.outfile, 'w', encoding='utf-8')

    def get_model_file(self, suffix):
        filename = os.path.join(self.modeldir, self.model + suffix)
        if (not os.path.isfile(filename)):
            raise Exception("File not found: " + filename)
        return filename

    def get_performance(self, platform):
        """Averages the performance metrics"""
        filename = self.get_model_file("_validation_" + platform + ".json")

        with open(filename, 'r') as f:
            results = json.loads(f.read())
            sum_average_times = reduce(lambda x, y: x + float(y['avg_time']), results, 0)
            average_time = sum_average_times / len(results)
            self.outfile.write("\n| Performance | " + platform + ": " + str(average_time) + "s/frame\n")

    def get_model_layers(self, root):
        """Returns a list of the high-level layers (i.e. function blocks) that make up the CNTK model """
        stack = [root.root_function]  # node
        layers = []         # final result, list of all relevant layers
        visited = set()
        #in_model = False

        while stack:
            node = stack.pop(0)
            from cntk import cntk_py
            try:
                # Function node
                stack = list(node.root_function.inputs) + stack
            except AttributeError:
                # OutputVariable node. We need process the owner node if this is an output.
                try:
                    if node.is_output:
                        stack.insert(0, node.owner)
                        continue
                except AttributeError:
                    pass
            # Add function nodes but skip Variable nodes
            if (not isinstance(node, Variable)) and (not node.uid in visited):
                #if (not in_model and node.name == "fc_output"):
                #    in_model = True
                #    layers.append(node)
                #elif in_model:
                #    if (node.name == "mean_removed_input"):
                #        layers.append(node)
                #        break
                layers.append(node)
                visited.add(node.uid)

        # CNTK layers are in opposite order to what ELL wants, so reverse the list
        layers.reverse()
        return layers

    def format_model_layer(self, cntk_layer):
        # channels, rows, columns => rows, columns, channels
        s = re.sub(r'Tensor\[(\d+),(\d+),(\d+)\]', r'Tensor[\2,\3,\1]', cntk_layer.__str__())

        # remove labels
        s = re.sub(r'^(.+): ', r'', s)

        return s

    def get_architecture(self):
        # TODO: we should have _config.json define the architecture sothat we don't need this
        filename = self.get_model_file(".cntk.zip")

        self.outfile.write("\n| Architecture | ");
        with zipfile.ZipFile(filename) as zf:
            for member in zf.infolist():
                _, e = os.path.splitext(member.filename)
                if (e == ".cntk"):
                    path = os.path.dirname(filename)
                    zf.extract(member, path)
                    model_root = load_model(os.path.join(path, member.filename))
                    layers = self.get_model_layers(model_root)
                    result = reduce(lambda x, y: x + self.format_model_layer(y) + "<br>", layers, "")
                    self.outfile.write(result)
                    break

    def run(self):
        self.get_performance("pi3")
        self.get_architecture()

if __name__ == "__main__":
    args = sys.argv
    program = GenerateMarkdown()
    program.parse_command_line(args)
    program.run()