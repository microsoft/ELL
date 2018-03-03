####################################################################################################
##
##  Project:  Embedded Learning Library (ELL)
##  File:     demoHelper.py
##  Authors:  Chris Lovett
##            Byron Changuion
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
import math

script_path = os.path.dirname(os.path.abspath(__file__))


# Helper class that interfaces with ELL models to get predictions and provides handy conversion from opencv to ELL buffers and 
# rendering utilities
class DemoHelper:
    def __init__(self, threshold=0.15):
        """ Helper class to store information about the model we want to use.
        threshold   - specifies a prediction threshold
        """

        self.threshold = threshold
        self.start = time.time()
        self.frame_count = 0
        self.fps = 0
        self.camera = 0
        self.image_filename = None
        self.image_folder = None
        self.images = None
        self.image_pos = 0
        self.capture_device = None
        self.frame = None
        self.save_images = None
        self.image_index = 0
        self.model_file = None
        self.model = None
        self.model_name = "model"
        self.compiled_model = None
        self.compiled_module = None
        self.compiled_func = None
        self.labels_file = None
        self.model_file = None
        self.iterations = None  # limit number of iterations through the loop.
        self.current = None
        self.total_time = 0
        self.time_count = 0
        self.warm_up = True
        self.input_shape = None
        self.output_shape = None
        self.output_size = 0
        self.bgr = False
        self.results = None
        self.nogui = False
    
    def add_arguments(self, arg_parser):
        """
        Adds common commandline arguments for ELL tutorials and demos and returns an object with the relevant values set from those arguments.
        Note: This method is designed for subclasses, so they can can add MORE arguments before calling parse_args.
        """

        # required arguments
        arg_parser.add_argument("labels", help="path to the labels file for evaluating the model, or comma separated list if using more than one model")

        # options
        arg_parser.add_argument("--save", help="save images captured by the camera", action='store_true')
        arg_parser.add_argument("--threshold", type=float, help="threshold for the minimum prediction score. A lower threshold will show more prediction labels, but they have a higher chance of being completely wrong.", default=self.threshold)
        arg_parser.add_argument("--bgr", help="specify True if input data should be in BGR format (default False)", default = self.bgr)
        arg_parser.add_argument("--nogui", help="disable GUI to enable automated testing of a batch of images", action='store_true')
        arg_parser.add_argument("--iterations", type=int, help="when used with --nogui this tests multiple iterations of each image to get better timing information")

        # mutually exclusive options
        group = arg_parser.add_mutually_exclusive_group()
        group.add_argument("--camera", type=int, help="the camera id of the webcam", default=0)
        group.add_argument("--image", help="path to an image file. If set, evaluates the model using the image, instead of a webcam")
        group.add_argument("--folder", help="path to an image folder. If set, evaluates the model using the images found there")

        group2 = arg_parser.add_mutually_exclusive_group()
        group2.add_argument("--model", help="path to a model file")
        group2.add_argument("--compiledModel", help="path to the compiled model's Python module")
        group2.add_argument("--models", help="list of comma separated paths to model files")
        group2.add_argument("--compiledModels", help="list of comma separated paths to the compiled models' Python modules")

    def parse_arguments(self, argv, helpString):
        arg_parser = argparse.ArgumentParser(helpString)
        self.add_arguments(arg_parser)        
        args = arg_parser.parse_args(argv)
        self.initialize(args)

    def value_from_arg(self, argValue, defaultValue):
        if (argValue is not None):
            return argValue
        return defaultValue

    def initialize(self, args):
        # called after parse_args to extract args from the arg_parser.
        # process required arguments
        self.labels_file = args.labels

        # process options
        self.save_images = self.value_from_arg(args.save, None)
        self.threshold = self.value_from_arg(args.threshold, None)
        self.iterations = self.value_from_arg(args.iterations, None)
        self.current = self.iterations
        self.camera = self.value_from_arg(args.iterations, 0)
        self.image_filename = self.value_from_arg(args.image, None)
        self.image_folder = self.value_from_arg(args.folder, None)
        self.bgr = args.bgr
        self.nogui = args.nogui
        if self.nogui and self.iterations == None:
            self.iterations = 1

        # process image source options
        if (args.camera):
            self.image_filename = None
            self.image_folder = None
        elif (args.image):
            self.camera = None
            self.image_folder = None
        elif (args.folder):
            self.camera = None

        # load the labels
        self.labels = self.load_labels(self.labels_file)
        
        # process model options and load the model
        self.model_file = args.model
        self.compiled_model = args.compiledModel
        if (self.model_file == None):
            # this is the compiled model route, so load the wrapped module
            self.model_name = os.path.split(self.compiled_model)[1]
            self.import_compiled_model(self.compiled_model, self.model_name)
        else:
            # this is the "interpreted" model route, so we need the ELL runtime.
            self.model_name = os.path.splitext(os.path.basename(self.model_file))[0]
            self.import_ell_map()
            
        self.input_size = (self.input_shape.rows, self.input_shape.columns)
        
        print("Found input_shape [%d,%d,%d]" % (self.input_shape.rows, self.input_shape.columns, self.input_shape.channels))
        return True

    def load_ell(self):
        print("### Loading ELL modules...")
        import find_ell
        import ell
        return ell


    def import_ell_map(self):
        ell = self.load_ell()
        sys.path.append(script_path)
        sys.path.append(os.getcwd())
        print("loading model: " + self.model_file)
        self.model = ell.model.Map(self.model_file)
        self.input_shape = self.model.GetInputShape()
        self.output_shape = self.model.GetOutputShape()
        self.output_size = int(self.output_shape.rows * self.output_shape.columns * self.output_shape.channels)

    def import_compiled_model(self, compiledModulePath, name):
        moduleDirectory = os.path.dirname(compiledModulePath)
        print('Looking for: ' + name + ' in ' + moduleDirectory)
        if (not os.path.isdir('build')) and (not os.path.isdir(moduleDirectory + '/build')):
            raise Exception("you don't have a 'build' directory in '" + compiledModulePath + "', have you compiled this project yet?")

        func_name = 'predict'
        if func_name == "":
            raise Exception("Could not construct func name. Is the --compiledModel argument correct?")
        
        # Import the compiled model wrapper. Add the possible build directories.
        sys.path.append(script_path)
        sys.path.append(moduleDirectory)
        sys.path.append(os.path.join(moduleDirectory, 'build'))
        sys.path.append(os.path.join(moduleDirectory, 'build/Release'))
        sys.path.append(os.path.join(script_path, 'build'))
        sys.path.append(os.path.join(script_path, 'build/Release'))
        sys.path.append(os.path.join(os.getcwd(), 'build'))
        sys.path.append(os.path.join(os.getcwd(), 'build/Release'))
        try:
            self.compiled_module = __import__(name)
            
            inputShapeGetter = getattr(self.compiled_module, "get_default_input_shape")
            outputShapeGetter = getattr(self.compiled_module, "get_default_output_shape")
            self.input_shape = inputShapeGetter()
            self.output_shape = outputShapeGetter()

            self.output_size = int(self.output_shape.rows * self.output_shape.columns * self.output_shape.channels)
            try:
                self.compiled_func = getattr(self.compiled_module, func_name)
            except AttributeError:
                raise Exception(func_name + " function not found in compiled module")
        except:    
            errorType, value, traceback = sys.exc_info()
            print("### Exception: " + str(errorType) + ": " + str(value))
            print("====================================================================")
            print("Compiled ELL python module is not loading")
            print("It is possible that you need to add LibOpenBLAS to your system path (See Install-*.md) from root of this repo")
            raise Exception("Compiled model failed to load")

    def show_image(self, frameToShow, save):
        try:
            cv2.imshow('frame', frameToShow)
        except cv2.error as e:
            # OpenCV may not have been built with GTK or Carbon support
            pass
            
        if save and self.save_images:
            name = 'frame' + str(self.image_index) + ".png"
            cv2.imwrite(name, frameToShow)
            self.image_index = self.image_index + 1

    def load_labels(self, fileName):
        labels = []
        with open(fileName) as f:
            labels = f.read().splitlines()
        return labels

    def predict(self, data):
        if self.current != None:
            self.current = self.current - 1
        start = time.time()
        if self.model == None:
            self.results = self.compiled_func(data)
        else:
            self.results = self.model.Compute(data, dtype=np.float32)
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

    def report_times(self, node_level=True):
        """Prints the average prediction time and additional profiling info, if available."""
        average_time = self.get_times()
        if average_time is not None:
            print("Average prediction time: " + str(average_time))

        # if the model is compiled with profiling enabled, report the additional info
        if hasattr(self.compiled_module, self.model_name + "_PrintModelProfilingInfo"):
            getattr(self.compiled_module, self.model_name + "_PrintModelProfilingInfo")()

        if node_level:
            if hasattr(self.compiled_module, self.model_name + "_PrintNodeProfilingInfo"):
                getattr(self.compiled_module, self.model_name + "_PrintNodeProfilingInfo")()

    def get_top_n_predictions(self, predictions, N = 5):
        """Return at most the top N predictions as a list of tuples that meet the threshold.
        The first of element of each tuple represents the index or class of the prediction and the second 
        element represents that probability or confidence value.
        """
        map = [(i,predictions[i]) for i in range(len(predictions)) if predictions[i] >= self.threshold]
        map.sort(key=lambda tup: tup[1], reverse=True)
        result = map[:N]
        return result

    def get_label(self, i):
        if (i < len(self.labels)):
            return self.labels[i]
        return ""

    def get_predictor_map(self, predictor, intervalMs=0):
        ell = self.load_ell()
        """Creates an ELL map from an ELL predictor"""
        return ell.neural.utilities.ell_map_from_float_predictor(predictor)

    def compile(self, predictor, platform, path):
        path += '/model'
        prediction_function = self.get_predictor_map(predictor)
        prediction_function.Compile(platform, 'model', 'step', path, dtype=np.float32)
        from ..util.commands import run_llc, run_swig
        run_swig(path + '.i')
        run_llc(path + '.ll')

    def save_ell_predictor_to_file(self, predictor, filePath, intervalMs=0):
        """Saves an ELL predictor to file so that it can be compiled to run on a device, with an optional stepInterval in milliseconds"""
        ell_map = self.get_predictor_map(predictor, intervalMs)
        ell_map.Save(filePath)

    def init_image_source(self):
        # Start video capture device or load static image
        if self.camera is not None:
            self.capture_device = cv2.VideoCapture(self.camera)
        elif self.image_filename:
            self.frame = cv2.imread(self.image_filename)
            if (type(self.frame) == type(None)):
                raise Exception('image from %s failed to load' % (self.image_filename))
        elif self.image_folder:
            self.frame = self.load_next_image()
    
    def load_next_image(self):
        if self.image_folder is None:
            return self.frame
        # find images in the self.image_folder and cycle through them.
        if self.images == None:
            self.images = os.listdir(self.image_folder)
        frame = None
        while frame is None and self.image_pos < len(self.images):
            filename = os.path.join(self.image_folder, self.images[self.image_pos])
            frame = cv2.imread(filename)
            self.image_pos += 1
        if not frame is None:
            return frame
        return self.frame

    def get_next_frame(self):
        if self.capture_device is not None:
            # if predictor is too slow frames get buffered, this is designed to flush that buffer
            for i in range(self.get_wait()):
                ret, self.frame = self.capture_device.read()
            if (not ret):
                raise Exception('your capture device is not returning images')
            return self.frame
        else:
            return np.copy(self.frame)

    def resize_image(self, image, newSize):
        # Shape: [rows, cols, channels]
        """Crops, resizes image to outputshape. Returns image as numpy array in in RGB order."""
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
        """Crops, resizes image to outputshape. Returns image as numpy array in in RGB order."""        
        resized = self.resize_image(image, self.input_size)
        if not self.bgr:
            resized = cv2.cvtColor(resized, cv2.COLOR_BGR2RGB)
        resized = resized.astype(np.float).ravel()
        return resized

    def draw_label(self, image, label):
        """Helper to draw text label onto an image"""
        self.draw_header(image, label)
        return

    def draw_header(self, image, text):
        """Helper to draw header text block onto an image"""
        self.draw_text_block(image, text, (0, 0), (50, 200, 50))
        return

    def draw_footer(self, image, text):
        """Helper to draw footer text block onto an image"""
        self.draw_text_block(image, text, (0, image.shape[0] - 40), (200, 100, 100))
        return

    def draw_text_block(self, image, text, blockTopLeft=(0,0), blockColor=(50, 200, 50), blockHeight=40, fontScale=0.7):
        """Helper to draw a filled rectangle with text onto an image"""
        cv2.rectangle(
            image, blockTopLeft, (image.shape[1], blockTopLeft[1] + blockHeight), blockColor, cv2.FILLED)
        cv2.putText(image, text, (blockTopLeft[0] + int(blockHeight / 4), blockTopLeft[1] + int(blockHeight * 0.667)),
                     cv2.FONT_HERSHEY_COMPLEX_SMALL, fontScale, (0, 0, 0), 1, cv2.LINE_AA)

    def draw_fps(self, image):
        """Helper to draw frame per second onto image"""
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
        if self.current is not None and self.current > 0:
            return False
            
        # on slow devices this helps let the images to show up on screen
        result = False
        try:
            if self.nogui:
                if self.images is not None and self.image_pos < len(self.images):
                    self.frame = self.load_next_image()
                    self.current = self.iterations
                    return False
                return True
                
            for i in range(self.get_wait()):
                key = cv2.waitKey(1) & 0xFF
                if key == 27:
                    result = True
                    break
                if key == ord(' '):
                    self.frame = self.load_next_image()
        except cv2.error as e:
            # OpenCV may not have been built with GTK or Carbon support
            pass

        return result
