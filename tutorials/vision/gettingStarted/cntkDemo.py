import sys
import os
import numpy as np
import cv2

import findEll
import cntk_to_ell
import modelHelper as mh

def get_ell_predictor(modelConfig):
    """Imports a model and returns an ELL.Predictor."""
    return cntk_to_ell.predictor_from_cntk_model(modelConfig.model_files[0])

def main():
    # Check for model file
    if (not os.path.exists('VGG16_ImageNet_Caffe.model')):
        print("Please download the 'VGG16_ImageNet_Caffe.model' file, see README.md")
        sys.exit(1)
        
    # ModelConfig for VGG16 model from CNTK Model Gallery
    # Follow the instructions in README.md to download the model if you intend to use it.
    helper = mh.ModelHelper(sys.argv, "VGG16ImageNet", ["VGG16_ImageNet_Caffe.model"], "cntkVgg16ImageNetLabels.txt", scaleFactor=1.0)

    # Import the model
    model = get_ell_predictor(helper)

    # Save the model
    helper.save_ell_predictor_to_file(model, "vgg16ImageNet.map")

    # Initialize image source
    helper.init_image_source()

    lastPrediction = ""

    while (not helper.done()):
        # Grab next frame
        frame = helper.get_next_frame()

        # Prepare the image to send to the model.
        # This involves scaling to the required input dimension and re-ordering from BGR to RGB
        data = helper.prepare_image_for_predictor(frame)

        # Get the model to classify the image, by returning a list of probabilities for the classes it can detect
        predictions = model.Predict(data)

        # Get the (at most) top 5 predictions that meet our threshold. This is returned as a list of tuples,
        # each with the text label and the prediction score.
        top5 = helper.get_top_n(predictions, 5)

        # Turn the top5 into a text string to display
        text = "".join([str(element[0]) + "(" + str(int(100*element[1])) + "%)  " for element in top5])

        if (text != lastPrediction):
            print(text)
            lastPrediction = text

        # Draw the text on the frame
        frameToShow = frame
        helper.draw_label(frameToShow, text)
        helper.draw_fps(frameToShow)

        # Show the new frame
        helper.show_image(frameToShow)


if __name__ == "__main__":
    main()
