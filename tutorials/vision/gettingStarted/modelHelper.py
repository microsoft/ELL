import cv2
import numpy as np
import ell_utilities

# Class to hold info about the model that the app needs to call the model and display result correctly
class ModelHelper:
    def __init__(self, modelName, modelFiles, labelsFile, inputHeightAndWidth = (224, 224), scaleFactor = 1/255, threshold = 0.25):
        """ Helper class to store information about the model we want to use.
        modelName - string name of the model
        modelFiles - list of strings containing darknet .cfg filename and darknet .weights filename, or CNTK model file name.
        labelsFile - string name of labels that correspond to the predictions output of the model
        inputHeightAndWidth - a list of two values giving the rows and columns of the input image for the model e.g. (224, 224)
        scaleFactor - each input pixel may need to be scaled. It is common for models to require an 8-bit pixel
                      to be represented as a value between 0.0 and 1.0, which is the same as multiplying it by 1/255.
        threshold - specifies a prediction threshold. We will ignore prediction values less than this
        """
        self.model_name = modelName
        self.model_files = modelFiles
        self.labels_file = labelsFile
        self.inputHeightAndWidth = inputHeightAndWidth
        self.scaleFactor = scaleFactor
        self.threshold = threshold
        self.labels = self.load_labels(self.labels_file)

    def load_labels(self, fileName):
        labels = []
        with open(fileName) as f:
            labels = f.read().splitlines()
        return labels

    def get_top_n(self, predictions, N):
        """Return at most the top 5 predictions as a list of tuples that meet the threashold."""
        topN = np.zeros([N,2])
        for p in range(len(predictions)):
            for t in range(len(topN)):
                if predictions[p] > topN[t][0]:
                    topN[t] = [predictions[p], p]
                    break    
        result = []
        for element in topN:
            if (element[0] > self.threshold):
                result.append((self.labels[int(element[1])], round(element[0],2)))
        return result

    def resize_image(self, image, newSize):
        """Crops, resizes image to outputshape. Returns image as numpy array in in RGB order, with each pixel multiplied by the configured scaleFactor."""
        if (image.shape[0] > image.shape[1]):
            rowStart = 0
            rowEnd = image.shape[1]
            colStart = int((image.shape[0] - image.shape[1]) / 2)
            colEnd = image.shape[1]
        else:
            rowStart = int( (image.shape[1] - image.shape[0]) / 2)
            rowEnd = image.shape[0]
            colStart = 0
            colEnd = image.shape[0]

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
        cv2.rectangle(image, (0, 0), (image.shape[1], 40), (50, 200, 50), cv2.FILLED)
        cv2.putText(image, label, (10,25), cv2.FONT_HERSHEY_SIMPLEX, 0.9, (0, 0, 0), 2, 8)
        return
    
    def save_ell_predictor_to_file(self, predictor, filePath, intervalMs = 0):
        """Saves an ELL predictor to file so that it can be compiled to run on a device, with an optional stepInterval in milliseconds"""
        name = self.model_name
        if (intervalMs > 0):
            ell_map = ell_utilities.ell_steppable_map_from_float_predictor(
                predictor, intervalMs, name + "InputCallback", name + "OutputCallback")
            ell_map.Save(filePath)
        else:
            ell_map = ell_utilities.ell_map_from_float_predictor(predictor)
            ell_map.Save(filePath)
