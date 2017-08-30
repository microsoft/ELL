####################################################################################################
##
##  Project:  Embedded Learning Library (ELL)
##  File:     demoHelper.py
##  Authors:  Chris Lovett
##
##  Requires: Python 3.x
##
####################################################################################################

import os
import sys
import argparse
import cv2
import numpy as np
import time
import json

script_path = os.path.dirname(os.path.abspath(__file__))
# Helper class that interfaces with opencv and provides handy conversion from opencv to ELL buffers and 
# rendering utilties

class DemoHelper:
    def __init__(self, threshold=0.25):
        """ Helper class to store information about the model we want to use.
        argv       - arguments passed in from the command line 
        threshold   - specifies a prediction threshold. We will ignore prediction values less than this
        """

        self.arg_parser = argparse.ArgumentParser(
            "Runs the given ELL model passing images from camera or static image file\n"
            "If no model file is given, it tries to load the compiled model defined in the config file\n"
            "Example:\n"
            "   python demo.py darknet_config.json darknetImageNetLabels.txt --model darknet.ellmodel\n"
            "This shows opencv window with image classified by the model using given labels")
        
        # each input pixel may need to be scaled. It is common for models to require an 8-bit pixel
        # to be represented as a value between 0.0 and 1.0, which is the same as multiplying it by 1/255.
        # This can be configured in the config file.
        self.scaleFactor = 1 / 255
        self.threshold = threshold
        self.start = time.time()
        self.frame_count = 0
        self.fps = 0
        self.camera = 0
        self.imageFilename = None
        self.captureDevice = None
        self.frame = None
        self.save_images = None
        self.model_file = None
        self.model = None
        self.compiled = None
        self.compiled_func = None
        self.config_file = None
        self.labels_file = None
        self.model_file = None
        self.iterations = None  # limit number of iterations through the loop.
        self.realLabels = None
        self.total_time = 0
        self.time_count = 0
        self.warm_up = True

    def parse_arguments(self, argv):
        # required arguments
        self.arg_parser.add_argument("configFile", help="path to the model configuration file")
        self.arg_parser.add_argument("labels", help="path to the labels file for evaluating the model")

        # options
        self.arg_parser.add_argument("--model", help="path to a model file")
        self.arg_parser.add_argument("--iterations", type=int, help="limits how many times the model will be evaluated, the default is to loop forever")
        self.arg_parser.add_argument("--save", help="save images captured by the camera", action='store_true')
        self.arg_parser.add_argument("--threshold", type=float, help="threshold for the minimum prediction score. A lower threshold will show more prediction labels, but they have a higher chance of being completely wrong.", default=self.threshold)
        self.arg_parser.add_argument("--realLabels", help="returns the numeric labels as predictions instead of the text labels", action='store_true')

        # mutually exclusive options
        group = self.arg_parser.add_mutually_exclusive_group()
        group.add_argument("--camera", type=int, help="the camera id of the webcam", default=self.camera)
        group.add_argument("--image", help="path to an image file. If set, evaluates the model using the image, instead of a webcam")

        argv.pop(0) # when an args list is passed to parse_args, the first argument (program name) needs to be dropped
        args = self.arg_parser.parse_args(argv)
        
        # process required arguments
        self.labels_file = args.labels
        self.config_file = args.configFile

        # process options
        self.model_file = args.model
        self.save_images = args.save
        self.threshold = args.threshold
        if (args.iterations):
            self.iterations = args.iterations
        self.realLabels = args.realLabels

        # process mutually exclusive options
        if (args.camera):
            self.camera = args.camera
            self.imageFilename = None
        elif (args.image):
            self.imageFilename = args.image
            self.camera = None

        # load the labels
        self.labels = self.load_labels(self.labels_file)
        
        with open(self.config_file) as f:
            self.config = json.loads(f.read())
            self.input_size = (self.config['input_rows'], self.config['input_columns'])
            print("using input size of ", self.input_size)
            if ("input_scale" in self.config):
                self.scaleFactor = float(self.config["input_scale"])

        if (self.model_file == None):
            # then assume we have a compiled model handy
            self.import_compiled_model()
        else:
            # this is the "interpreted" model route, so we need the ELL runtime.
            sys.path.append(script_path)
            print("### Loading ELL modules...")
            __import__("find_ell")
            ELL = __import__("ELL")            
            ell_utilities = __import__("ell_utilities")
            print("loading model: " + self.model_file)
            self.model = ELL.ELL_Map(self.model_file)

        return True

    def import_compiled_model(self):
        name = self.config['model']
        if name == "":
            raise Exception("config file is missing model name")
        
        func_name = self.config['func']
        if func_name == "":
            raise Exception("config file is missing func name")
        
        name = os.path.splitext(name)[0]
        if not os.path.isdir('build'):
            raise Exception("you don't have a 'build' directory, have you compiled this project yet?")

        # Import the compiled model wrapper
        sys.path.append(os.path.join(script_path, 'build'))
        sys.path.append(os.path.join(script_path, 'build/Release'))
        try:
            self.compiled = __import__(name)
            size = int(self.config['output_rows']) * self.config['output_columns'] * self.config['output_channels']
            self.results = self.compiled.FloatVector(size)
            try:
                self.compiled_func = getattr(self.compiled, func_name)
            except AttributeError:
                raise Exception(func_name + " function not found in compiled module")
        except:    
            errorType, value, traceback = sys.exc_info()
            print("### Exception: " + str(errorType))
            print("====================================================================")
            print("Compiled ELL python module is not loading")
            print("It is possible that you need to add LibOpenBLAS to your system path (See Install-*.md) from root of this repo")
            raise Exception("Compiled model failed to load")

    def show_image(self, frameToShow, save):
        cv2.imshow('frame', frameToShow)
        if save and self.save_images != None:
            name = 'frame' + str(self.save_images) + ".png"
            cv2.imwrite(name, frameToShow)
            self.save_images = self.save_images + 1

    def load_labels(self, fileName):
        labels = []
        with open(fileName) as f:
            labels = f.read().splitlines()
        return labels

    def predict(self, data):
        if self.iterations != None:
            self.iterations = self.iterations - 1
        start = time.time()
        if self.model == None:
            self.compiled_func(data, self.results)
        else:
            self.results = self.model.ComputeFloat(data)
        end = time.time()
        diff = end - start

        # if warm up is true then discard the first time
        if self.time_count == 1 and self.warm_up:
            self.warm_up = False
            self.total_time = 0
            self.time_count = 0

        self.total_time = self.total_time + diff
        self.time_count = self.time_count + 1
        return self.results

    def get_times(self):
        """Returns the average prediction time, if available."""
        average_time = None
        if self.time_count > 0:
            average_time = self.total_time/self.time_count
        return average_time

    def report_times(self):
        """Prints the average prediction time, if available."""
        average_time = self.get_times()
        if average_time is not None:
            print("Average prediction time: " + str(average_time))

    def get_top_n(self, predictions, N):
        """Return at most the top 5 predictions as a list of tuples that meet the threshold."""
        topN = np.zeros([N, 2])

        for p in range(len(predictions)):
            for t in range(len(topN)):
                if predictions[p] > topN[t][0]:
                    topN[t] = [predictions[p], p]
                    break
        result = []
        for element in topN:
            if element[0] > self.threshold:
                i = int(element[1])
                if (i < len(self.labels)):
                    if self.realLabels:
                        result.append((i, round(element[0], 2)))
                    else:
                        result.append((self.labels[i], round(element[0], 2)))
        return result

    def get_predictor_map(self, predictor, intervalMs):
        """Creates an ELL map from an ELL predictor"""
        import ell_utilities

        name = self.model_name
        if intervalMs > 0:
            ell_map = ell_utilities.ell_steppable_map_from_float_predictor(
                predictor, intervalMs, name + "InputCallback", name + "OutputCallback")
        else:
            ell_map = ell_utilities.ell_map_from_float_predictor(predictor)
        return ell_map

    def save_ell_predictor_to_file(self, predictor, filePath, intervalMs=0):
        """Saves an ELL predictor to file so that it can be compiled to run on a device, with an optional stepInterval in milliseconds"""
        ell_map = self.get_predictor_map(predictor, intervalMs)
        ell_map.Save(filePath)

    def init_image_source(self):
        # Start video capture device or load static image
        if self.camera is not None:
            self.captureDevice = cv2.VideoCapture(self.camera)
        elif self.imageFilename:
            self.frame = cv2.imread(self.imageFilename)
            if (type(self.frame) == type(None)):
                raise Exception('image from %s failed to load' %
                                (self.imageFilename))

    def get_next_frame(self):
        if self.captureDevice is not None:
            # if predictor is too slow frames get buffered, this is designed to flush that buffer
            for i in range(self.get_wait()):
                ret, self.frame = self.captureDevice.read()
            if (not ret):
                raise Exception('your captureDevice is not returning images')
            return self.frame
        else:
            return np.copy(self.frame)

    def resize_image(self, image, newSize):
        # Shape: [rows, cols, channels]
        """Crops, resizes image to outputshape. Returns image as numpy array in in RGB order, with each pixel multiplied by the configured scaleFactor."""
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
        """Crops, resizes image to outputshape. Returns image as numpy array in in RGB order, with each pixel multiplied by the configured scaleFactor."""        
        resized = self.resize_image(image, self.input_size)
        resized = cv2.cvtColor(resized, cv2.COLOR_BGR2RGB)
        resized = resized * self.scaleFactor
        resized = resized.astype(np.float).ravel()
        return resized

    def draw_label(self, image, label):
        """Helper to draw text onto an image"""
        cv2.rectangle(
            image, (0, 0), (image.shape[1], 40), (50, 200, 50), cv2.FILLED)
        cv2.putText(image, label, (10, 25),
                    cv2.FONT_HERSHEY_SIMPLEX, 0.8, (0, 0, 0), 2, cv2.LINE_AA)
        return

    def draw_fps(self, image):
        now = time.time()
        if self.frame_count > 0:
            diff = now - self.start
            if diff >= 1:
                self.fps = round(self.frame_count / diff, 1)
                self.frame_count = 0
                self.start = now

        label = "fps " + str(self.fps)
        labelSize, baseline = cv2.getTextSize(
            label, cv2.FONT_HERSHEY_SIMPLEX, 0.4, 1)
        width = image.shape[1]
        height = image.shape[0]
        pos = (width - labelSize[0] - 5, labelSize[1] + 5)
        cv2.putText(image, label, pos, cv2.FONT_HERSHEY_SIMPLEX,
                    0.4, (0, 0, 128), 1, cv2.LINE_AA)
        self.frame_count = self.frame_count + 1

    def get_wait(self):
        speed = self.fps
        if (speed == 0):
            speed = 1
        if (speed > 1):
            return 1
        return 3

    def done(self):
        if self.iterations is not None and self.iterations <= 0:
            return True
        # on slow devices this helps let the images to show up on screen
        result = False
        for i in range(self.get_wait()):
            if cv2.waitKey(1) & 0xFF == 27:
                result = True
                break
        return result
