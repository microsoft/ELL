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
    if (not os.path.exists('cntkDarknetBinarized.model')):
        print("Please download the 'cntkDarknetBinarized.model' file, see README.md")
        sys.exit(1)

    # ModelConfig for Darknet Binarized model from ELL Model Gallery
    # Follow the instructions in README.md to download the model if you intend to use it.
    helper = mh.ModelHelper(sys.argv, "CntkDarknetBinarized", [
                            "cntkDarknetBinarized.model"], "darknetImageNetLabels.txt")

    # Import the model
    model = get_ell_predictor(helper)

    # Save the model
    helper.save_ell_predictor_to_file(model, "cntkDarknetBinarized.map")

    # Initialize image source
    helper.init_image_source()

    last_prediction = ""

    while (True):
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
        text = "".join(
            [str(element[0]) + "(" + str(int(100 * element[1])) + "%)  " for element in top5])

        if (text != lastPrediction):
            print(text)
            lastPrediction = text

        # Draw the text on the frame
        frameToShow = frame
        helper.draw_label(frameToShow, text)
        helper.draw_fps(frameToShow)

        # Show the new frame
        cv2.imshow('frame', frameToShow)

        # Wait for Esc key
        if cv2.waitKey(1) & 0xFF == 27:
            break


if __name__ == "__main__":
    main()
