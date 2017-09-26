####################################################################################################
##
##  Project:  Embedded Learning Library (ELL)
##  File:     model_data_retriever.py (gallery)
##  Authors:  Lisa Ong
##
##  Requires: Python 3.x
##
####################################################################################################

import os
import sys
import argparse
import json
import re

# ELL utilities
sys.path.append(os.path.join(os.path.dirname(os.path.abspath(__file__)), '../'))
import ziptools

def _format_float(value):
    return "{0:.2f}".format(value)

def _lf_to_crlf(file):
    """Utility function that converts LF delimiters to CRLF delimiters in a file"""
    lines = open(file, 'rU').readlines()
    with open(file, 'w', newline='\r\n') as f:
        f.writelines(lines)

"""Encapsulates retrieving information about a model from the ELL-models repository"""
class ModelDataRetriever:
    def __init__(self, modeldir, model):
        if not os.path.isdir(modeldir):
            raise FileNotFoundError("{} is not a folder".modeldir)

        self.modeldir = modeldir
        self.model = model

    def _get_data_filename(self, name, is_suffix=False):
        """Returns the path to a file from the model directory, given the filename or a suffix"""
        if is_suffix:
            filename = os.path.join(self.modeldir, self.model + name)
        else:
            filename = os.path.join(self.modeldir, name)
            
        if (not os.path.isfile(filename)):
            raise FileNotFoundError("File not found: " + filename)
        return filename

    def get_model_properties(self):
        """Retrieves the basic properties of the model"""
        properties = {}

        filename = self._get_data_filename("modelargs.json")
        with open(filename, 'r') as f:
            results = json.loads(f.read())
            properties['image_size'] = results['image_size']
            properties['num_classes'] = results['num_classes']

        filename = self._get_data_filename(".ell.zip", is_suffix=True)
        unzip = ziptools.Extractor(filename)
        success, temp = unzip.extract_file(".ell")
        if (success):
            print("extracted: " + temp)
            size_mb = round(os.path.getsize(temp) / (1000 * 1000))
            properties['size_mb'] = size_mb
            os.remove(temp)
        else:
            # not a zip archive
            raise(FileNotFoundError, "{} is not a valid zip archive".format(filename))

        return properties

    def get_model_seconds_per_frame(self, platforms):
        """Retrieves the seconds per frame metric for the list of platforms from the model validation results"""
        seconds_per_frame = {}

        for platform in platforms:
            try:
                filename = self._get_data_filename("validation_" + platform + ".out")
                _lf_to_crlf(filename)
                with open(filename, 'r') as f:
                    # find the last instance of "Total time: 48412.709961 ms\tcount: 150"
                    for line in reversed(list(f)):
                        matches = re.search(r'^Total time: (.+) ms\tcount: (.+)', line)
                        if matches and matches.group(1) and matches.group(2):
                            average_time = float(matches.group(1)) / int(matches.group(2)) / 1000
                            break
                    seconds_per_frame[platform] = _format_float(average_time)
            except:
                # leave entries missing if file isn't found, or invalid json (validation not complete on the target)
                pass

        return seconds_per_frame

    def get_model_topN_accuracies(self):
        """Retrieves the accuracy (top 1 and top 5 correct in percentages) from the model test results"""

        accuracy = {}
        filename = self._get_data_filename("test_eval.json")

        with open(filename, 'r') as f:
            results = json.loads(f.read())
            accuracy['top1'] = _format_float(100 * (1.0 - float(results['average_top1_error'])))
            accuracy['top5'] = _format_float(100 * (1.0 - float(results['average_top5_error'])))

        return accuracy