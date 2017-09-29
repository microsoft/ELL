####################################################################################################
#
# Project:  Embedded Learning Library (ELL)
# File:     cntk_full_model_test.py (importers)
# Authors:  Chris Lovett
#
# Requires: Python 3.x, cntk-2.0-cp35
#
####################################################################################################
import cntk
import sys
import os
script_path = os.path.dirname(os.path.abspath(__file__))
sys.path.append(os.path.join(script_path, ".."))
import cntk_to_ell
import ELL
import ell_utilities
import lib.cntk_layers as cntk_layers
import lib.cntk_utilities as cntk_utilities
import numpy as np
import cv2
import math

class FullModelTest:
    def __init__(self, labels_file):
        self.cntk_model = None
        self.data = None
        self.method_index = 0
        self.input_shape = None
        self.image = None
        self.ell_data = None
        self.compiled_data = None
        self.labels = self.load_labels(labels_file)
        self.report = None
        self.layer_index = 1

    def compare_arrays(self, a, b, msg, precision=0):
        a = a.astype(dtype=np.float32).ravel()
        b = b.astype(dtype=np.float32).ravel()
        min_diff = abs(min(a) - min(b))
        max_diff = abs(max(a) - max(b))
        largest_diff = max([abs(x) for x in (a - b)])
        std_dev_diff = abs(np.std(a) - np.std(b))
        mean_diff = abs(np.mean(a) - np.mean(b))

        if min_diff > precision or max_diff > precision or std_dev_diff > precision or mean_diff > precision or largest_diff > precision:
            print("  " + msg)
        
        if min_diff > precision:
            print("    min %f versus %f, diff is %.10f" % (min(a), min(b), min_diff))
        
        if max_diff > precision:
            print("    max %f versus %f, diff is %.10f" % (max(a), max(b), max_diff))
            
        if mean_diff > precision:
            print("    mean %f versus %f, diff is %.10f" % (np.mean(a), np.mean(b), mean_diff))
            
        if std_dev_diff > precision:
            print("    stddev %f versus %f, diff is %.10f" % (np.std(a), np.std(b), std_dev_diff))
            
        if largest_diff > precision:
            print("    largest individual diff=%.10f" % (largest_diff))

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
            print("cntk picks: %s" % (self.get_label(np.argmax(self.data))))
        if type(self.ell_data) != type(None):
            print("ell picks: %s" % (self.get_label(np.argmax(self.ell_data))))
        if type(self.compiled_data) != type(None):
            print("ell compiled picks: %s" % (self.get_label(np.argmax(self.compiled_data))))
    
    def run(self, filename, image_file):
        self.report = open("report.md", "w")
        self.report.write("# Comparison Results\n")
        self.report.write("**model**: %s\n\n" %(filename))
        if image_file != None:
            self.image = self.load_image(image_file)
            self.report.write("**image**: %s\n\n" %(image_file))

        self.cntk_model = cntk.load_model(filename)
        modelLayers = cntk_utilities.get_model_layers(self.cntk_model)
        # Get the relevant CNTK layers that we will convert to ELL
        layersToConvert = cntk_layers.get_filtered_layers_list(modelLayers)
        print("----------------------------------------------------------------------------------")
        for layer in layersToConvert:
            self.compare(layer)

        self.print_top_result()
        self.report.close()

    def normalize(self, a):
        max = np.max(a)
        return a * (255 / max)

    def save_layer_outputs(self, layer):
        name = layer.layer.op_name + "(" + str(self.layer_index) + ")"        
        self.layer_index += 1
        self.report.write("## %s\n" % (name))
        self.report.write("````\n")
        shape = self.data.shape
        self.report.write("Output size: %d x %d x %d, stride: %d x %d x %d, offset: %d x %d x %d\n" % (shape[1],shape[2],shape[0],shape[1],shape[2],shape[0],0,0,0))

        self.report.write("````\n")

        with open("Compare_" + name + ".csv", "w") as f:
            f.write("cntk,ell,compiled\n")
            a = self.data.ravel()
            b = self.ell_data.ravel()
            c = self.compiled_data.ravel()
            pos = 0
            while pos < len(a) and pos < len(b) and pos < len(c):
                f.write("%f,%f,%f\n" % (a[pos], b[pos], b[pos]))
                pos += 1
            f.close()


    def compare(self, layer):  
        print("Comparing layer " + str(layer))
        
        if self.input_shape is None:
            for node_input in layer.layer.inputs:
                if node_input.is_input and isinstance(node_input, cntk.variables.Variable):
                    # great, this tells us the input size.
                    self.input_shape = node_input.shape
                    break

        if (type(self.data) == type(None)):  
            if (type(self.image) != type(None)):  
                self.data = self.prepare_image_for_predictor(self.image).astype(dtype=np.float32).reshape((self.input_shape[1],self.input_shape[2],self.input_shape[0]))
                self.data = np.transpose(self.data, (2, 0, 1)) # to match CNTK (channel, rows, coumns) order
            else:
                # then just use a range of numbers as input.
                self.data = range(self.input_shape[0] * self.input_shape[1] * self.input_shape[2])
                self.data = np.reshape(self.data, self.input_shape).astype(dtype=np.float32)
                self.data = self.normalize(self.data)
                
        # execute cntk model and save the output for comparison with ELL.
        feature = cntk.input_variable(self.data.shape)
        clone = layer.clone_cntk_layer(feature)
        output = clone(self.data)[0]
        self.verify_ell(layer, self.data, output)
        self.data = output # make this the input to the next layer.
        self.save_layer_outputs(layer)
    
    def verify_ell(self, layer, data, expected):
        # now compare this with the equivalent ELL layer, both reference and compiled.
        ell_layers = []
        # remove output_padding from because CNTK doesn't have output padding.
        layer.layer.ell_outputPaddingParameters = ELL.PaddingParameters(ELL.PaddingScheme.zeros, 0)
        layer.layer.ell_outputShape = cntk_utilities.get_adjusted_shape(
                layer.layer.output.shape, layer.layer.ell_outputPaddingParameters)
        layer.process(ell_layers)

        # Create an ELL neural network predictor from the relevant CNTK layers
        predictor = ELL.FloatNeuralNetworkPredictor(ell_layers)
        ellTestInput =  np.transpose(data, (1, 2, 0)).ravel().astype(dtype=np.float)
        ellResults = predictor.Predict(ellTestInput)
        ellArray = np.array(ellResults)
        self.ell_data = ellArray
        output_shape = predictor.GetOutputShape()
        out2 = np.reshape(ellArray, (output_shape.rows, output_shape.columns, output_shape.channels))
        out2 = np.transpose(out2, (2, 0, 1)) # to match CNTK output
        # now compare these results.
        self.compare_arrays(expected, out2.astype(dtype=np.float32), 'Results for %s layer do not match!' % (layer.op_name))
        
        # and verify compiled is also the same
        self.verify_compiled(predictor, ellTestInput, ellArray, layer.op_name)

    def verify_compiled(self, predictor, input, expectedOutput, module_name, precision=0):
        # now run same over ELL compiled model
        map = ell_utilities.ell_map_from_float_predictor(predictor)
        compiled = map.Compile("host", module_name, "test" + str(self.method_index))
        self.method_index += 1
        compiledResults = compiled.ComputeFloat(input)
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

if __name__ == '__main__':
    args = sys.argv
    args.pop(0)
    imagefile = None
    if len(args) == 3:
        imagefile = args[2]
    if len(args) >= 2:
        test = FullModelTest(args[0])
        test.run(args[1], imagefile)
    else:
        print("Usage: Python cntk_full_model_test.py labels_file cntk_model_file [test_image]")