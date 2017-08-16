import os
import sys
import cv2
import numpy as np
import time
import json

# Helper class that interfaces with opencv and provides handy conversion from opencv to ELL buffers and 
# rendering utilties

class DemoHelper:
    def __init__(self, scaleFactor=1 / 255, threshold=0.25):
        """ Helper class to store information about the model we want to use.
        argv       - arguments passed in from the command line 
        scaleFactor - each input pixel may need to be scaled. It is common for models to require an 8-bit pixel
                      to be represented as a value between 0.0 and 1.0, which is the same as multiplying it by 1/255.
        threshold   - specifies a prediction threshold. We will ignore prediction values less than this
        """
        self.scaleFactor = scaleFactor
        self.threshold = threshold
        self.start = time.time()
        self.frame_count = 0
        self.fps = 0
        self.camera = 0
        self.imageFilename = None
        self.captureDevice = None
        self.frame = None
        self.save_images = None
        self.command_name = None
        self.model_file = None
        self.model = None
        self.compiled = None
        self.compiled_func = None
        self.config_file = None
        self.labels_file = None
        self.model_file = None
        self.once = None  # whether to loop or just perform one prediction.

    def print_usage(self):
        print("Usage: " + self.command_name + " configFile labels [model|-save|cameraId|staticImage|-once]")
        print("Runs the given ELL model passing images from camera or static image file")
        print("If no model file is given, it tries to load the compiled model defined in the config file")
        print("Example:")
        print("    gettingStarted darknet_config.json darknetImageNetLabels.txt darknet.ellmodel")
        print("This shows opencv window with image classified by the model using given labels")
        return

    def parse_arguments(self, argv):
        # Parse arguments
        self.command_name = argv[0]

        if (len(argv) < 3):
            return False

        i = 1
        while (i < len(argv)):
            arg = argv[i]
            if (arg[0] == "-" or arg[0] == "/"):
                option = arg[1:]
                if (option.lower() == "save"):
                    self.save_images = 1
                elif (option.lower() == "once"):
                    self.once = 1
                else:
                    print("Unknown option: " + arg)
                    return False
            elif arg.isdigit():
                self.camera = int(arg)
            elif (self.config_file == None):
                self.config_file = arg
            elif (self.labels_file == None):
                self.labels_file = arg
            else:
                ext = os.path.splitext(arg)[1]
                if (ext.lower() == ".ellmodel"):
                    self.model_file = arg
                else:
                    self.imageFilename = arg
                    self.camera = None
            i = i + 1

        if (self.config_file == None):
            print("missing config file argument")
            return False  
        if (self.labels_file == None):
            print("missing label file argument")
            return False       

        # load the labels
        self.labels = self.load_labels(self.labels_file)
        
        with open(self.config_file) as f:
            self.config = json.loads(f.read())
            self.input_size = (self.config['input_rows'], self.config['input_columns'])
            print("using input size of ", self.input_size)

        if (self.model_file == None):
            # then assume we have a compiled model handy
            self.import_compiled_model()
        else:
            # this is the "interpreted" model route, so we need the ELL runtime.
            script_path = os.path.dirname(os.path.abspath(__file__))
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
        if (name == ""):
            raise Exception("config file is missing model name")
        
        func_name = self.config['func']
        if (func_name == ""):
            raise Exception("config file is missing func name")
        
        name = os.path.splitext(name)[0]
        if (not os.path.isdir('build')):
            raise Exception("you don't have a 'build' directory, have you compiled this project yet?")

        # Import the compiled model wrapper
        sys.path.append('build')
        sys.path.append('build/Release')
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
        if (save and not self.save_images is None):
            name = 'frame' + str(self.save_images) + ".png"
            cv2.imwrite(name, frameToShow)
            self.save_images = self.save_images + 1

    def load_labels(self, fileName):
        labels = []
        with open(fileName) as f:
            labels = f.read().splitlines()
        return labels

    def predict(self, data):
        if (self.once == 1):
            self.once = 2
        if (self.model == None):
            self.compiled_func(data, self.results)
            return self.results
        else:
            return self.model.ComputeFloat(data)

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
            if (element[0] > self.threshold):
                i = int(element[1])
                if (i < len(self.labels)):
                    result.append((self.labels[i], round(element[0], 2)))
        return result

    def get_predictor_map(self, predictor, intervalMs):
        """Creates an ELL map from an ELL predictor"""
        import ell_utilities

        name = self.model_name
        if (intervalMs > 0):
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
        if (image.shape[0] > image.shape[1]):  # Tall (more rows than cols)
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
        if (self.frame_count > 0):
            diff = now - self.start
            if (diff >= 1):
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
        if (self.once == 2):
            return True
        # on slow devices this helps let the images to show up on screen
        result = False
        for i in range(self.get_wait()):
            if cv2.waitKey(1) & 0xFF == 27:
                result = True
                break
        return result
