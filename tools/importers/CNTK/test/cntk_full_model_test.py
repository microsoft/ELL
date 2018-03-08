####################################################################################################
#
# Project:  Embedded Learning Library (ELL)
# File:     cntk_full_model_test.py (importers)
# Authors:  Chris Lovett
#
# Requires: Python 3.x, cntk-2.4
#
####################################################################################################
import sys
import logging
import os

import cntk
import numpy as np
import cv2
import math
import argparse

script_path = os.path.dirname(os.path.abspath(__file__))
sys.path.append(os.path.join(script_path, ".."))
import cntk_to_ell
from custom_functions import CustomSign, BinaryConvolution
import ell
import lib.cntk_converters as cntk_converters
import lib.cntk_layers as cntk_layers
import lib.cntk_utilities as cntk_utilities
from custom_functions import BinaryConvolution, CustomSign

logging.basicConfig(level=logging.INFO, format="%(message)s")
_logger = logging.getLogger(__name__)

class FullModelTest:
    def __init__(self, args):
        self.image_file = args.image
        self.model_file = args.model_file
        self.cntk_model = None
        self.data = None
        self.method_index = 0
        self.input_shape = None
        self.image = None
        self.ell_data = None
        self.compiled_data = None
        self.labels = self.load_labels(args.label_file)
        self.report = None
        self.layer_index = 1
        self.layers = args.layers # whether to test the whole model or layer by layer

    def compare_arrays(self, a, b, msg, precision=1e-4):
        a = a.astype(dtype=np.float32).ravel()
        b = b.astype(dtype=np.float32).ravel()

        if len(a) != len(b): raise Exception("Arrays are not the same size. Output dimensions are different!")

        min_diff = abs(min(a) - min(b))
        max_diff = abs(max(a) - max(b))
        largest_diff = max([abs(x) for x in (a - b)])
        std_dev_diff = abs(np.std(a) - np.std(b))
        mean_diff = abs(np.mean(a) - np.mean(b))

        if not np.allclose(a, b, atol=precision):
            _logger.info("  " + msg)

        if min_diff > precision:
            _logger.info("    min %f versus %f, diff is %.10f" % (min(a), min(b), min_diff))

        if max_diff > precision:
            _logger.info("    max %f versus %f, diff is %.10f" % (max(a), max(b), max_diff))

        if mean_diff > precision:
            _logger.info("    mean %f versus %f, diff is %.10f" % (np.mean(a), np.mean(b), mean_diff))

        if std_dev_diff > precision:
            _logger.info("    stddev %f versus %f, diff is %.10f" % (np.std(a), np.std(b), std_dev_diff))

        if largest_diff > precision:
            _logger.info("    largest individual diff=%.10f" % (largest_diff))

    def load_image(self, filename):
        image = cv2.imread(filename)
        if (type(image) == type(None)):
            raise Exception('image from %s failed to load' % (filename))
        return image


    def resize_image(self, image, newSize):
        # Shape: [rows, cols, channels]
        """Crops, resizes image to newSize."""
        if image.shape[0] > image.shape[1]:  # Tall (more rows than cols)
            rowStart = int((image.shape[0] - image.shape[1]) / 2)
            rowEnd = rowStart + image.shape[1]
            colStart = 0
            colEnd = image.shape[1]
        else:  # Wide (more cols than rows)
            rowStart = 0
            rowEnd = image.shape[0]
            colStart = int((image.shape[1] - image.shape[0]) / 2)
            colEnd = colStart + image.shape[0]

        cropped = image[rowStart:rowEnd, colStart:colEnd]
        resized = cv2.resize(cropped, newSize)
        return resized

    def prepare_image_for_predictor(self, image):
        """Crops, resizes image to outputshape. Returns image as numpy array in in BGR order."""
        input_size = (self.input_shape[1], self.input_shape[2]) # remember cntk shapes are (channel,rows,columns)
        resized = self.resize_image(image, input_size)
        resized = resized.astype(np.float).ravel()
        return resized

    def print_top_result(self):
        if type(self.data) != type(None):
            _logger.info("cntk picks: %s" % (self.get_label(np.argmax(self.data))))
        if type(self.ell_data) != type(None):
            _logger.info("ell picks: %s" % (self.get_label(np.argmax(self.ell_data))))
        if type(self.compiled_data) != type(None):
            _logger.info("ell compiled picks: %s" % (self.get_label(np.argmax(self.compiled_data))))

    def run(self):
        self.report = open("report.md", "w")
        self.report.write("# Comparison Results\n")
        self.report.write("**model**: %s\n\n" %(self.model_file))
        if self.image_file != None:
            self.image = self.load_image(self.image_file)
            self.report.write("**image**: %s\n\n" %(self.image_file))

        self.cntk_model = cntk.load_model(self.model_file)
        modelLayers = cntk_utilities.get_model_layers(self.cntk_model)
        # Get the relevant CNTK layers that we will convert to ELL
        layersToConvert = cntk_layers.get_filtered_layers_list(modelLayers)
        _logger.info("----------------------------------------------------------------------------------")
        if self.layers:
            for layer in layersToConvert:
                self.compare_layer(layer)
        else:
            self.compare_model(layersToConvert)

        self.print_top_result()
        self.report.close()

    def normalize(self, a):
        max = np.max(a)
        return a * (255 / max)

    def save_layer_outputs(self, op_name):
        name = op_name + "(" + str(self.layer_index) + ")"
        self.layer_index += 1
        self.report.write("## %s\n" % (name))
        self.report.write("````\n")
        shape = self.data.shape
        self.report.write("Output size: " + str(shape))

        self.report.write("````\n")

        with open("Compare_" + name + ".csv", "w") as f:
            f.write("cntk,ell,compiled\n")

            a = cntk_converters.get_float_vector_from_cntk_array(self.data)
            b = self.ell_data.ravel()
            c = self.compiled_data.ravel()
            pos = 0
            while pos < len(a) and pos < len(b) and pos < len(c):
                f.write("%f,%f,%f\n" % (a[pos], b[pos], b[pos]))
                pos += 1
            f.close()

    def compare_model(self, layers):
        ellLayers = cntk_layers.convert_cntk_layers_to_ell_layers(layers)
        # Create an ELL neural network predictor from the layers
        predictor = ell.neural.FloatNeuralNetworkPredictor(ellLayers)
        shape = predictor.GetInputShape()
        self.input_shape = (shape.channels,shape.rows,shape.columns) # to CNTK (channel, rows, coumns) order
        self.data = self.get_input_data()

        if (len(self.cntk_model.arguments) > 1):
            output = np.zeros(self.cntk_model.arguments[1].shape).astype(np.float32)
            predictions = self.cntk_model.eval({self.cntk_model.arguments[0]:[self.data],self.cntk_model .arguments[1]:output})
        else:
            predictions = self.cntk_model.eval({self.cntk_model.arguments[0]:[self.data]})

        size = 0
        output = None
        if isinstance(predictions,dict):
            for key in self.cntk_model.outputs:
                shape = key.shape
                if len(shape) > 0:
                    s = np.max(shape)
                    if (s > size):
                        size = s
                        output = predictions[key][0] / 100
        else:
            output = predictions[0]

        self.verify_ell("Softmax", predictor, self.data, output)
        self.data = output # make this the input to the next layer.
        self.save_layer_outputs("Softmax")

    def get_input_data(self):
        data = self.data # from previous layer
        if (type(data) == type(None)):
            if (type(self.image) != type(None)):
                data = self.prepare_image_for_predictor(self.image).astype(dtype=np.float32).reshape((self.input_shape[1],self.input_shape[2],self.input_shape[0]))
                data = np.transpose(data, (2, 0, 1)) # to match CNTK (channel, rows, coumns) order
            else:
                # then just use a range of numbers as input.
                data = ((np.random.rand(self.input_shape[0] * self.input_shape[1] * self.input_shape[2]) * 10) - 5).astype(np.float32) # Random input from -5 to 5
                data = np.reshape(data, self.input_shape).astype(dtype=np.float32)
                data = self.normalize(data)
        return data

    def compare_layer(self, layer):
        _logger.info("Comparing layer " + str(layer))

        if self.input_shape is None:
            for node_input in layer.layer.inputs:
                if node_input.is_input and isinstance(node_input, cntk.variables.Variable):
                    # great, this tells us the input size.
                    self.input_shape = node_input.shape
                    break

        self.data = self.get_input_data()

        # execute cntk model and save the output for comparison with ell.
        feature = cntk.input_variable(self.data.shape)
        clone = layer.clone_cntk_layer(feature)
        output = clone(self.data)[0]
        predictor = self.get_predictor(layer)
        self.verify_ell(layer.op_name, predictor,self.data, output)
        self.data = output # make this the input to the next layer.
        self.save_layer_outputs(layer.op_name)

    def get_predictor(self, layer):

        ell_layers = []
        # remove output_padding from because CNTK doesn't have output padding.
        layer.layer.ell_outputPaddingParameters = ell.neural.PaddingParameters(ell.neural.PaddingScheme.zeros, 0)
        layer.layer.ell_outputShape = cntk_utilities.get_adjusted_shape(
                layer.layer.output.shape, layer.layer.ell_outputPaddingParameters)
        layer.process(ell_layers)
        # Create an ELL neural network predictor from the relevant CNTK layers
        return ell.neural.FloatNeuralNetworkPredictor(ell_layers)


    def verify_ell(self, op_name, predictor, data, expected):
        # now compare this with the equivalent ELL layer, both reference and compiled.

        if len(data.shape) == 1:
            ellTestInput = data.ravel().astype(dtype=np.float)
        else:
            ellTestInput =  np.transpose(data, (1, 2, 0)).ravel().astype(dtype=np.float)
        ellResults = predictor.Predict(ellTestInput)
        ellArray = np.array(ellResults)
        self.ell_data = ellArray
        output_shape = predictor.GetOutputShape()
        out2 = np.reshape(ellArray, (output_shape.rows, output_shape.columns, output_shape.channels))
        if len(data.shape) == 3:
            out2 = np.transpose(out2, (2, 0, 1)) # to match CNTK output

        # now compare these results.
        self.compare_arrays(expected, out2.astype(dtype=np.float32), 'Results for %s layer do not match!' % (op_name))

        # and verify compiled is also the same
        self.verify_compiled(predictor, ellTestInput, ellArray, op_name)

    def verify_compiled(self, predictor, input, expectedOutput, module_name, precision=1e-4):
        map = ell.neural.utilities.ell_map_from_float_predictor(predictor)

        # Note: for testing purposes, callback functions assume the "model" namespace
        compiler_options = model.MapCompilerOptions()
        compiler_options.useBlas = False
        compiled = map.Compile("host", "model", "test" + str(self.method_index), compilerOptions=compiler_options, dtype=np.float32)

        self.method_index += 1
        compiledResults = compiled.Compute(input, dtype=np.float32)
        ca = np.array(compiledResults) # convert back to numpy
        self.compiled_data = ca
        expectedFloats = expectedOutput.astype(dtype=np.float32)
        # Compare compiled results
        self.compare_arrays(expectedFloats, ca, 'results for %s layer do not match ELL compiled output !' % (module_name), precision)

    def load_labels(self, fileName):
        labels = []
        with open(fileName) as f:
            labels = f.read().splitlines()
        return labels

    def get_label(self, i):
        if (i < len(self.labels)):
            return self.labels[i]
        return ""


def main(argv):
    arg_parser = argparse.ArgumentParser(
        "Tests a given cntk model and compares results from CNTK, ELL, and compiled ELL implementations of that model\n"
        "Example:\n"
        "    python cntk_full_model_test.py labels.txt model.cntk --image image.svg --layers True \n"
        "The --layers option causes it to do layer by layer comparison outputing Compare*.csv files for each layer'\n")

    arg_parser.add_argument("label_file", help="path to a labels file")
    arg_parser.add_argument("model_file", help="path to a CNTK model file, or a zip archive of a CNTK model file")
    arg_parser.add_argument("--image", help="path to a image file to use as input (default is random data)")
    arg_parser.add_argument("--layers", type=bool, help="turns on layer-by-layer testing", default=False)
    args = arg_parser.parse_args(argv)
    test = FullModelTest(args)
    test.run()

if __name__ == '__main__':
    args = sys.argv
    args.pop(0)
    main(args)