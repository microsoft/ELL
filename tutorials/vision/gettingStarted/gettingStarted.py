import sys
import os
import numpy as np
import cv2

# Add paths to enable importing of ELL python module and the importers
sys.path.append('./../../../interfaces/python')
sys.path.append('./../../../interfaces/python/Release')
sys.path.append('./../../../interfaces/python/Debug')
sys.path.append('./../../../tools/importers/CNTK')
sys.path.append('./../../../tools/importers/Darknet')
sys.path.append('./..')
import ELL
import darknet_to_ell
import cntk_to_ell

# Enum to indicate whether the app will import a model from CNTK or Darknet
class ImportFrom:
    cntk = 0
    darknet = 1

# Class to hold info about the model that the app needs to call the model and display result correctly
class ModelConfig:
    def __init__(self, importFrom, modelFiles, labelsFile, inputHeightAndWidth = (224, 224), scaleFactor = 1/255, threshold = 0.25):
        """ Helper class to store information about the model we want to use.
        importFrom - ImportFrom.cntk or ImportFrom.darknet
        modelFiles - list of strings containing darknet .cfg filename and darknet .weights filename, or CNTK model file name.
        labelsFile - string name of labels that correspond to the predictions output of the model
        inputHeightAndWidth - a list of two values giving the rows and columns of the input image for the model e.g. (224, 224)
        scaleFactor - each input pixel may need to be scaled. It is common for models to require an 8-bit pixel
                      to be represented as a value between 0.0 and 1.0, which is the same as multiplying it by 1/255.
        threshold - specifies a prediction threshold. We will ignore prediction values less than this
        """
        self.import_from = importFrom
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


def get_ell_predictor(modelConfig):
    """Imports a model and returns an ELL.Predictor."""
    predictor = None
    if (modelConfig.import_from == ImportFrom.darknet):
        # Import a pretrained CNTK model
        predictor = darknet_to_ell.predictor_from_darknet_model(modelConfig.model_files[0], modelConfig.model_files[1])
    else:
        # Import a pretrained Darknet model
        predictor = cntk_to_ell.predictor_from_cntk_model(modelConfig.model_files[0])
    return predictor

def get_top_5(predictions, labels, threshold):
    """Return at most the top 5 predictions as a list of tuples that meet the threshold."""
    top5 = np.array(predictions).argsort()[-5:]
    
    result = []
    for element in top5:
        prediction = predictions[int(element)]
        if (prediction > threshold):
            result.append((labels[element], round(prediction,2)))

    return result

def prepare_image_for_predictor(image, outputShape, scaleFactor):
    """Crops, resizes image to outputshape. Returns image as numpy array in RGB order, with each pixel multiplied by the scaleFactor."""

    width = outputShape[1]
    height = outputShape[0]
    shape = image.shape
    h = shape[0]
    w = shape[1]
    xscale = width / w
    yscale = height / h
    if yscale > xscale:
        # need to crop in x-axis
        cropWidth = int(width/yscale)
        cx = int(((w - cropWidth)/ 2) + (cropWidth/2))
        cropped = cv2.getRectSubPix(image, (cropWidth, h), (cx, h/2))
    else:
        # crop in y-axis
        cropHeight = int(height/xscale)
        cy = int(((h - cropHeight)/ 2) + (cropHeight/2))
        cropped = cv2.getRectSubPix(image, (w, cropHeight), (w/2, cy))
    
    # now they have the same aspect ratio we can resize without distortion
    resized = cv2.resize(cropped, (width,height))
    resized = cv2.cvtColor(resized, cv2.COLOR_BGR2RGB)
    resized = resized * scaleFactor
    resized = resized.astype(np.float).ravel()

    return resized

def draw_label(image, label):
    """Helper to draw text onto an image"""
    cv2.rectangle(image, (0, 0), (image.shape[1], 40), (50, 200, 50), cv2.FILLED)
    cv2.putText(image, label, (10,25), cv2.FONT_HERSHEY_SIMPLEX, 0.9, (0, 0, 0), 2, 8)

    return

# ModelConfig for reference Darknet model
# Follow the instructions in README.md to download the model if you intend to use it.
darknetReferenceConfig = ModelConfig(ImportFrom.darknet, ["darknet.cfg", "darknet.weights"], "darknetImageNetLabels.txt")

# ModelConfig for VGG16 model from CNTK Model Gallery
# Follow the instructions in README.md to download the model if you intend to use it.
cntkVgg16Config = ModelConfig(ImportFrom.cntk, ["VGG16_ImageNet_Caffe.model"], "cntkVgg16ImageNetLabels.txt", scaleFactor=1.0)

def main():
    # Pick the model you want to work with
    currentConfig = darknetReferenceConfig

    # Import the model
    model = get_ell_predictor(currentConfig)

    # Start video capture device
    cap = cv2.VideoCapture(0)

    while (True):
        # Grab next frame
        ret, frame = cap.read()

        # Prepare the image to send to the model.
        # This involves scaling to the required input dimension and re-ordering from BGR to RGB
        data = prepare_image_for_predictor(frame, currentConfig.inputHeightAndWidth, currentConfig.scaleFactor)

        # Get the model to classify the image, by returning a list of probabilities for the classes it can detect
        predictions = model.Predict(data)

        # Get the (at most) top 5 predictions that meet our threshold. This is returned as a list of tuples,
        # each with the text label and the prediction score.
        top5 = get_top_5(predictions, currentConfig.labels, currentConfig.threshold)

        # Turn the top5 into a text string to display
        text = "".join([str(element[0]) + " " + str(element[1]) + "    " for element in top5])

        # Draw the text on the frame
        frameToShow = frame
        draw_label(frameToShow, text)

        # Show the new frame
        cv2.imshow('frame', frameToShow)

        # Wait for Esc key
        if cv2.waitKey(1) & 0xFF == 27:
            break

if __name__ == "__main__":
    main()