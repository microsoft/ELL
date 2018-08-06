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
import cv2
import numpy as np
import time
import math

script_path = os.path.dirname(os.path.abspath(__file__))

sys.path += [ os.path.join(script_path, "..") ]

class EllModel:
    """ this is the base class for interacting with ELL models """
    def __init__(self):
        self.input_shape = None
        self.output_shape = None
        self.output_size = 0

    def load(self):
        pass

    def predict(self, data):
        return None

    def print_profile_info(self, node_level):
        pass


class CompiledModel(EllModel):
    """ this class encapsulates a compiled ELL model """
    def __init__(self, path):
        super(CompiledModel, self).__init__()
        self.compiled_module_path = path
        self.compiled_model = None
        self.compiled_module = None
        self.compiled_func = None
        self.func_name = 'predict'
        
        self.compiled_module_path = path
        self.model_name = os.path.split(path)[1]

    def load(self):
        module_directory = os.path.dirname(self.compiled_module_path)
        if not module_directory:
            module_directory = "."

        model_path, module_directory = os.path.split(module_directory)            
        if os.path.isdir(model_path):
            sys.path.append(model_path)

        if not os.path.isdir(os.path.join(module_directory, 'build')):
            raise Exception("you don't have a 'build' directory in '" + module_directory + "', have you compiled this project yet?")
                
        try:
            import importlib
            if module_directory == ".":
                # we are inside the directory that contains __init__.py, which means we can't execute __init__.py.
                sys.path += [ os.path.join(os.getcwd(), "build") ]
                sys.path += [ os.path.join(os.getcwd(), "build", "release") ]
                self.compiled_module = importlib.import_module(self.model_name)
            else:
                sys.path += [ os.getcwd() ]
                self.compiled_module = getattr(importlib.import_module(module_directory), self.model_name)
            
            inputShapeGetter = getattr(self.compiled_module, "get_default_input_shape")
            outputShapeGetter = getattr(self.compiled_module, "get_default_output_shape")
            self.input_shape = inputShapeGetter()
            self.output_shape = outputShapeGetter()
            self.output_size = int(self.output_shape.rows * self.output_shape.columns * self.output_shape.channels)
            try:
                self.compiled_func = getattr(self.compiled_module, self.func_name)
            except AttributeError:
                raise Exception(self.func_name + " function not found in compiled module")
        except:    
            errorType, value, traceback = sys.exc_info()
            print("### Exception: " + str(errorType) + ": " + str(value))
            print("====================================================================")
            print("Compiled ELL python module is not loading")
            print("It is possible that you need to add LibOpenBLAS to your system path (See Install-*.md) from root of this repo")
            raise Exception("Compiled model failed to load")

    def predict(self, data):
        return self.compiled_func(data)

    def print_profile_info(self, node_level):        
        # if the model is compiled with profiling enabled, report the additional info
        if hasattr(self.compiled_module, self.model_name + "_PrintModelProfilingInfo"):
            getattr(self.compiled_module, self.model_name + "_PrintModelProfilingInfo")()

        if node_level:
            if hasattr(self.compiled_module, self.model_name + "_PrintNodeProfilingInfo"):
                getattr(self.compiled_module, self.model_name + "_PrintNodeProfilingInfo")()


class ReferenceModel(EllModel):
    """ this class encapsulates a reference ELL model """
    def __init__(self, path):
        super(ReferenceModel, self).__init__()
        self.model_file = path
        self.model_name = os.path.splitext(os.path.basename(self.model_file))[0]

    def load(self):
        ell = self.load_ell()
        sys.path.append(script_path)
        sys.path.append(os.getcwd())
        self.model = ell.model.Map(self.model_file)
        self.input_shape = self.model.GetInputShape()
        self.output_shape = self.model.GetOutputShape()
        self.output_size = int(self.output_shape.rows * self.output_shape.columns * self.output_shape.channels)

    def load_ell(self):
        # this is the "interpreted" model route, so we need the ELL python module.
        import find_ell
        import ell
        return ell

    def predict(self, data):
        return self.model.Compute(data, dtype=np.float32)

class ImageStream:
    def __init__(self):
        self.new_frame = False
        self.frame = None
    
    def load_next_image(self):
        """ advance to next image in the stream """
        return None

    def get_next_frame(self):
        """ return the current image """
        return np.copy(self.frame)

class VideoStream(ImageStream):
    def __init__(self, camera):
        super(VideoStream, self).__init__()
        self.camera = camera
        self.capture_device = cv2.VideoCapture(self.camera)
        self.load_next_image()
    
    def get_next_frame(self):
        ret, self.frame = self.capture_device.read()
        if (not ret):
            raise Exception('your capture device is not returning images')
        return self.frame

    def load_next_image(self):
        # the video stream is live, no need to "advance" to the next frame
        pass

class FolderStream(ImageStream):
    def __init__(self, image_folder):
        super(FolderStream, self).__init__()
        self.image_folder = image_folder
        self.images = os.listdir(self.image_folder)
        self.image_pos = 0
        self.image_filename = None
        self.load_next_image()
        
    def load_next_image(self):
        frame = None
        while frame is None and self.image_pos < len(self.images):
            filename = os.path.join(self.image_folder, self.images[self.image_pos])
            self.image_filename = filename
            frame = cv2.imread(filename)
            if frame is None:
                print("Error loading image: {}".format(filename))
            else:                
                self.new_frame = True
                self.frame = frame
            self.image_pos += 1
        return frame

class StaticImage(ImageStream):
    def __init__(self, image_filename):
        super(StaticImage, self).__init__()
        self.image_filename = image_filename
        self.frame = cv2.imread(self.image_filename)
        if self.frame is None:
            raise Exception('image from %s failed to load' % (self.image_filename))
        else:                
            self.new_frame = True
    
    def load_next_image(self):
        return None


# Helper class that interfaces with ELL models to get predictions and provides handy conversion from opencv to ELL buffers and 
# rendering utilities
class DemoHelper:
    def __init__(self, threshold=0.15):
        """ Helper class to help load and run predictions over images using a compiled
        or reference ELL model.
        threshold   - specifies a prediction threshold
        """
        self.threshold = threshold
        self.start = time.time()
        self.frame_count = 0
        self.fps = 0
        self.save_images = None
        self.image_index = 0
        self.model = None
        self.labels_file = None
        self.iterations = None  # limit number of iterations through the loop.
        self.current = None
        self.total_time = 0
        self.time_count = 0
        self.warm_up = True
        self.bgr = False
        self.results = None
        self.nogui = False
        self.print_labels = False
        self.new_frame = False
        self.window_shown = False
    
    def value_from_arg(self, argValue, defaultValue):
        if (argValue is not None):
            return argValue
        return defaultValue

    def set_input(self, camera_id, image_folder = None, image_filename = None):        
        self.camera = self.value_from_arg(camera_id, 0)
        self.image_filename = self.value_from_arg(image_folder, None)
        self.image_folder = self.value_from_arg(image_filename, None)
        
        # process image source options
        if image_filename:
            self.source = StaticImage(image_filename)
        elif image_folder:
            self.source = FolderStream(image_folder)
        else:
            self.source = VideoStream(camera_id)

    def load_model(self, labels_file, model_file = None, compiled_model = None):
        # called after parse_args to extract args from the arg_parser.
        # process required arguments
        
        # process options 
        if self.nogui and self.iterations is None:
            self.iterations = 1

        self.current = self.iterations
        
        # load the labels
        self.labels_file = labels_file
        self.labels = self.load_labels(self.labels_file)
        
        # process model options and load the model
        if model_file:
            self.model = ReferenceModel(model_file)
        else:
            self.model = CompiledModel(compiled_model)

        # now load the model
        self.model.load()
            
        self.input_size = (self.model.input_shape.rows, self.model.input_shape.columns)        

    def show_image(self, frameToShow, save):
        try:
            self.window_shown = True
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
        if self.current != None and self.current > -1:
            self.current = self.current - 1
        start = time.time()
        self.results = self.model.predict(data)
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

        self.model.print_profile_info(node_level)

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
    
    def get_next_frame(self):
        return self.source.get_next_frame()

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

    def draw_header(self, image, text):
        """Helper to draw header text block onto an image"""        
        self.draw_text_block(image, text, (0, 0), (50, 200, 50))

    def draw_footer(self, image, text):
        """Helper to draw footer text block onto an image"""
        self.draw_text_block(image, text, (0, image.shape[0] - 40), (200, 100, 100))

    def draw_text_block(self, image, text, blockTopLeft=(0,0), blockColor=(50, 200, 50), blockHeight=40, fontScale=0.7):
        """Helper to draw a filled rectangle with text onto an image"""
        cv2.rectangle(
            image, blockTopLeft, (image.shape[1], blockTopLeft[1] + blockHeight), blockColor, cv2.FILLED)
        cv2.putText(image, text, (blockTopLeft[0] + int(blockHeight / 4), blockTopLeft[1] + int(blockHeight * 0.667)),
                    cv2.FONT_HERSHEY_COMPLEX_SMALL, fontScale, (0, 0, 0), 1, cv2.LINE_AA)

    def draw_fps(self, image):
        """Helper to draw frame per second onto image"""
        if self.print_labels:
            return
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
            # here we are measuring fps on the same image for --iterations iterations, so 
            # we do not advance the frame here until the iterations is reached.
            return False

        # on slow devices this helps let the images to show up on screen
        result = False
        try:
            if self.nogui:
                if self.source.load_next_image() is not None:
                    self.current = self.iterations
                    return False
                return True
                
            for i in range(self.get_wait()):
                key = cv2.waitKey(1) & 0xFF
                if key == 27:
                    result = True
                    break
                if key == ord(' '):
                    # for folder input this advances the image to next frame.
                    self.source.load_next_image()
        except cv2.error as e:
            # OpenCV may not have been built with GTK or Carbon support
            pass

        return result
