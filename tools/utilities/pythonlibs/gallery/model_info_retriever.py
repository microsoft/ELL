####################################################################################################
##
##  Project:  Embedded Learning Library (ELL)
##  File:     model_info_retriever.py (gallery)
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
import subprocess

# ELL utilities
sys.path.append(os.path.join(os.path.dirname(os.path.abspath(__file__)), "../"))
import ziptools

def _format_float(value):
    return "{0:.2f}".format(value)

def _lf_to_crlf(file):
    "Utility function that converts LF delimiters to CRLF delimiters in a file"
    lines = open(file, "rU").readlines()
    with open(file, "w", newline="\r\n") as f:
        f.writelines(lines)

class BaseLayer:
    """Class that extracts information from any layer"""
    def __init__(self, name, parameters):
        self.name = re.sub(r"Layer", "", name, 1) # strip out the "Layer" part
        self.name = re.sub(r"<(.+)>", "", self.name, 1) # remove parameterized type

        matches = re.search(r"shape=\[(.+)\]\->\[(.+)\]", parameters)
        output_shape = matches.group(2)
        self.output_shape = re.sub(r",", "\u00d7", output_shape)
        self.parameters = {}

    def as_dict(self):
        return {
            "name" : self.name,
            "output_shape" : self.output_shape,
            "parameters" : self.parameters
        }

class ActivationLayer(BaseLayer):
    """Class that extracts information on an Activation layer
       e.g.: name: "ActivationLayer<float,ParametricReLUActivation>"
             parameters: "shape=[3,3,1024]->[5,5,1024], outputPadding=min,1"
    """
    def __init__(self, name, parameters):
        super().__init__(name, parameters)

        matches = re.search(r"<(.*),(\w+)Activation>", name)
        operation = matches.group(2).lower() # lowercase to match pooling operations

        # insert a space for the XXXReLU case, otherwise leave as is
        try:
            matches = re.search(r"(.+)relu", operation)
            operation = "{} relu".format(matches.group(1))
        except:
            pass

        self.parameters["operation"] = operation

class ConvolutionalLayer(BaseLayer):
    """Class that extracts information on a Convolutional layer
       e.g. name: "ConvolutionalLayer<float>"
            parameters: "shape=[82,82,16]->[80,80,64], inputPadding=zeros,1, stride=1, method=columnwise, receptiveField=3, numFilters=64"
    """
    def __init__(self, name, parameters):
        super().__init__(name, parameters)

        matches = re.search(r"receptiveField=(\d+)", parameters)
        size = matches.group(1)
        self.parameters["size"] = "{}\u00d7{}".format(size, size)

        matches = re.search(r"stride=(\d+)", parameters)
        self.parameters["stride"] = matches.group(1)

        if self.name.startswith("Binary"):
            data_type = "int64"
        else:
            # data_type may be specified as template parameter
            try:
                matches = re.search(r"<(.+)>", name)
                data_type = matches.group(1)
            except:
                data_type = "float32"

        if data_type == "float":
            data_type = "float32"

        self.parameters["type"] = data_type
        self.name = "Convolution"
        # activation will be inferred later by folding layers together

class PoolingLayer(BaseLayer):
    """Class that extracts information on a Pooling layer
       e.g.: name: "PoolingLayer<float,MaxPoolingFunction>"
             parameters: "shape=[160,160,16]->[82,82,16], outputPadding=zeros,1, function=maxpooling, stride=2, size=2"
    """
    def __init__(self, name, parameters):
        super().__init__(name, parameters)

        matches = re.search(r"size=(\d+)", parameters)
        size = matches.group(1)
        self.parameters["size"] = "{}\u00d7{}".format(size, size)

        matches = re.search(r"stride=(\d+)", parameters)
        self.parameters["stride"] = matches.group(1)

        matches = re.search(r"function=(\w+)", parameters)
        operation = re.sub(r"pooling", "", matches.group(1).lower()) # lowercase
        if operation == "mean":
            operation = "average"
        self.parameters["operation"] = operation

class LayerFactory:
    """Factory that produces classes that extract information on layers"""
    @staticmethod
    def create_layer(line):
        try:
            # split into name and parameters
            # sample line input: ConvolutionalLayer(shape=[162,162,3]->[160,160,16], inputPadding=zeros,1, stride=1, ...)'
            matches = re.search(r"^\s*(.+)\((.+)\)$", line)
            layer_name = matches.group(1)
            layer_parameters = matches.group(2)

            if layer_name.startswith("ActivationLayer"):
                return ActivationLayer(layer_name, layer_parameters)
            elif layer_name.startswith("ConvolutionalLayer") or layer_name.startswith("BinaryConvolutionalLayer"):
                return ConvolutionalLayer(layer_name, layer_parameters)
            elif layer_name.startswith("Pooling"):
                return PoolingLayer(layer_name, layer_parameters)
            elif layer_name.startswith("SoftmaxLayer"):
                return BaseLayer(layer_name, layer_parameters)
            else:
                return None
        except:
            print("Skipping line: {}".format(line))
            return None

class ModelInfoRetriever:
    """Encapsulates retrieving information about a model from the ELL-models repository"""

    def __init__(self, modeldir, model):
        if not os.path.isdir(modeldir):
            raise NotADirectoryError("{} is not a folder".format(modeldir))

        self.modeldir = modeldir
        self.model = model
        self.model_file = None
        self.cleanup_model_file = False

    def __enter__(self):
        """Called when this object is instantiated with 'with'"""
        return self

    def __exit__(self, exc_type, exc_value, traceback):
        """Called on cleanup of this object that was instantiated with 'with'"""
        if self.cleanup_model_file:
            os.unlink(self.model_file)

    def _get_data_filename(self, name, is_suffix=False):
        "Returns the path to a file from the model directory, given the filename or a suffix"
        if is_suffix:
            filename = os.path.join(self.modeldir, self.model + name)
        else:
            filename = os.path.join(self.modeldir, name)
            
        if (not os.path.isfile(filename)):
            raise FileNotFoundError("File not found: " + filename)
        return filename

    def _ensure_model_file(self):
        "Lazy-extracts the ELL model file from zip if it doesn't exist"
        try:
            filename = self._get_data_filename(".ell", is_suffix=True)
            self.model_file = filename
        except:
            filename = self._get_data_filename(".ell.zip", is_suffix=True)
            unzip = ziptools.Extractor(filename)
            success, temp = unzip.extract_file(".ell")
            if (success):
                print("extracted: " + temp)
                self.model_file = temp
                self.cleanup_model_file = True
            else:
                # not a zip archive
                raise(FileNotFoundError, "{} is not a valid zip archive".format(filename))

    def get_model_properties(self):
        """Retrieves the basic properties of the model"""
        properties = {}

        filename = self._get_data_filename("modelargs.json")
        with open(filename, "r") as f:
            results = json.loads(f.read())
            properties["image_size"] = results.get("image_size")
            properties["num_classes"] = results.get("num_classes")
            properties["model"] = results.get("model")
            properties["name"] = results.get("name")
            properties["filter_size"] = results.get("filter_size", 3)
            properties["increase_factor"] = results.get("increase_factor", 0)
            self.model = properties["name"] # regardless of the name of the folder, this will get the proper model name (i.e. <modelname>.cntk)

            # optional property
            properties["trainer"] = results.get("trainer", "CNTK 2.2")

        self._ensure_model_file()
        properties["size_mb"] = round(os.path.getsize(self.model_file) / (1000 * 1000))

        return properties

    def get_model_seconds_per_frame(self, platforms):
        """Retrieves the seconds per frame metric for the list of platforms from the model validation results"""
        seconds_per_frame = {}

        for platform in platforms:
            try:
                filename = self._get_data_filename("validation_" + platform + ".out")
                _lf_to_crlf(filename)
                with open(filename, "r") as f:
                    # find the last instance of "Total time: 48412.709961 ms\tcount: 150"
                    for line in reversed(list(f)):
                        matches = re.search(r"^Total time: (.+) ms\tcount: (.+)", line)
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

        with open(filename, "r") as f:
            results = json.loads(f.read())
            accuracy["top1"] = _format_float(100 * (1.0 - float(results["average_top1_error"])))
            accuracy["top5"] = _format_float(100 * (1.0 - float(results["average_top5_error"])))

        return accuracy

    def _run_print_tool(self, print_exe_path):
        "Runs the print tool and returns its text output as a list of lines"
        if not os.path.isfile(print_exe_path):
            raise FileNotFoundError("{} does not exist".format(print_exe_path))

        self._ensure_model_file()
        command = [print_exe_path, "-imap", self.model_file]

        proc = subprocess.Popen(command, stdout=subprocess.PIPE, stderr=subprocess.PIPE,
            bufsize=0, universal_newlines = True)

        result = [line.strip("\n") for line in proc.stdout]
        errors = [line for line in proc.stderr]
        proc.wait()

        if proc.returncode != 0:
            raise Exception("{} failed: {}".format(command[0], "".join(errors)))
        return result

    def _get_relevant_arch_layers(self, all_layers):
        "Filters out only relevant layers to display in the model architecture"
        result = [LayerFactory.create_layer(x) for x in all_layers]
        return [x.as_dict() for x in result if x is not None]

    def get_model_architecture(self, print_exe_path):
        """Retrieves the model architecture as a JSON formatted object"""
        raw_layers = self._run_print_tool(print_exe_path)
        arch_layers = self._get_relevant_arch_layers(raw_layers)

        # fold activation into convolution
        result = []
        for i in range(len(arch_layers)):
            candidate = arch_layers[i]
            if candidate["name"] == "Convolution" and (i+1 < len(arch_layers)) and arch_layers[i+1]["name"] == "Activation":
                candidate["parameters"]["activation"] = arch_layers[i+1]["parameters"]["operation"]

            # skip activation because it's been folded into convolution
            if candidate["name"] != "Activation":
                result.append(candidate)

        return result
