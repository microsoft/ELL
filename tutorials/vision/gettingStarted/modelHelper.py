import cv2
import numpy as np
import time

# Class to hold info about the model that the app needs to call the model and display result correctly

class ModelHelper:
    def __init__(self, argv, modelName, modelFiles, labelsFile, inputHeightAndWidth=(224, 224), scaleFactor=1 / 255, threshold=0.25):
        """ Helper class to store information about the model we want to use.
        argv       - arguments passed in from the command line
        modelName  - string name of the model
        modelFiles - list of strings containing darknet .cfg filename and darknet .weights filename, or CNTK model file name.
        labelsFile - string name of labels that correspond to the predictions output of the model
        inputHeightAndWidth - a list of two values giving the rows and columns of the input image for the model e.g. (224, 224)
        scaleFactor - each input pixel may need to be scaled. It is common for models to require an 8-bit pixel
                      to be represented as a value between 0.0 and 1.0, which is the same as multiplying it by 1/255.
        threshold   - specifies a prediction threshold. We will ignore prediction values less than this
        """
        self.model_name = modelName
        self.model_files = modelFiles
        self.labels_file = labelsFile
        self.inputHeightAndWidth = inputHeightAndWidth
        self.scaleFactor = scaleFactor
        self.threshold = threshold
        self.labels = self.load_labels(self.labels_file)
        self.start = time.time()
        self.frame_count = 0
        self.fps = 0
        self.camera = None
        self.imageFilename = None
        self.captureDevice = None
        self.frame = None
        self.has_temp_sensor = None
        self.save_images = None
        # now parse the arguments
        self.parse_arguments(argv)
    
    def parse_arguments(self, argv):
        # Parse arguments
        self.camera = 0
        self.imageFilename = None
        for i in range(1,len(argv)):            
            arg1 = argv[i]
            if (arg1 == "-save"):
                self.save_images = 1
            elif arg1.isdigit():
                self.camera = int(arg1) 
            else:
                self.imageFilename = arg1
                self.camera = None

    def show_image(self, frameToShow):          
        cv2.imshow('frame', frameToShow)
        if (not self.save_images is None):
            name = 'frame' + str(self.save_images) + ".png"
            cv2.imwrite(name, frameToShow)
            self.save_images = self.save_images + 1

    def load_labels(self, fileName):
        labels = []
        with open(fileName) as f:
            labels = f.read().splitlines()
        return labels

    def get_top_n(self, predictions, N):
        """Return at most the top 5 predictions as a list of tuples that meet the threashold."""
        topN = np.zeros([N, 2])
        for p in range(len(predictions)):
            for t in range(len(topN)):
                if predictions[p] > topN[t][0]:
                    topN[t] = [predictions[p], p]
                    break
        result = []
        for element in topN:
            if (element[0] > self.threshold):
                result.append(
                    (self.labels[int(element[1])], round(element[0], 2)))
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
                raise Exception('image from %s failed to load' % (self.imageFilename))

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
        if (image.shape[0] > image.shape[1]): # Tall (more rows than cols)
            rowStart = int((image.shape[0] - image.shape[1]) / 2)
            rowEnd = rowStart + image.shape[1]
            colStart = 0
            colEnd = image.shape[1]
        else: # Wide (more cols than rows)
            rowStart = 0
            rowEnd = image.shape[0]
            colStart = int((image.shape[1] - image.shape[0]) / 2)
            colEnd = colStart + image.shape[0]

        cropped = image[rowStart:rowEnd, colStart:colEnd]
        resized = cv2.resize(cropped, newSize)
        return resized
    
    def prepare_image_for_predictor(self, image):
        """Crops, resizes image to outputshape. Returns image as numpy array in in RGB order, with each pixel multiplied by the configured scaleFactor."""
        resized = self.resize_image(image, self.inputHeightAndWidth)
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
        pos = (width - labelSize[0] - 5, labelSize[1]+5)
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
        # on slow devices this helps let the images to show up on screen
        result = False
        for i in range(self.get_wait()):
            if cv2.waitKey(1) & 0xFF == 27:
                result = True
                break
        return result
