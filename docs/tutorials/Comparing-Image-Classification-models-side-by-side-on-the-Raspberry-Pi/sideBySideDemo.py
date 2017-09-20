####################################################################################################
##
##  Project:  Embedded Learning Library (ELL)
##  File:     sideBySideDemo.py
##  Authors:  Byron Changuion
##
##  Requires: Python 3.x
##
####################################################################################################

import sys
import os
import argparse
import numpy as np
import cv2
script_path = os.path.dirname(os.path.abspath(__file__))
sys.path.append(os.path.join(script_path, "../../../tools/utilities/pythonlibs"))
import demoHelper as d

# note: to run this in headless mode on a Linux machine run the following from your terminal window
# export DISPLAY=:0
# then add the '-save' argument to get tagged frames to be saved to disk.

def get_model_helpers(demoArgs):
    """
    Returns a list of model helpers, initialized from the commandline arguments in demoArgs
    """
    models = []
    numModels = 0

    # Split the labels string on commas.
    demoArgs.labelsList = [x.strip() for x in demoArgs.labels.split(',')]

    if demoArgs.models:
        # Split the models string on commas
        demoArgs.modelsList = [x.strip() for x in demoArgs.models.split(',')]
        numModels = len(demoArgs.modelsList)
        demoArgs.compiledList = [None] * numModels
    else:
        # Split the compiled string on commas
        demoArgs.compiledList = [x.strip() for x in demoArgs.compiledModels.split(',')]
        numModels = len(demoArgs.compiledList)
        demoArgs.modelsList = [None] * numModels
    # If the number of elements in the labelsList is 1, then use the same labels file
    # for all models
    if (len(demoArgs.labelsList) == 1):
        demoArgs.labelsList = demoArgs.labelsList * numModels

    # Iterate over the model list and instantiate a helper for each.
    # Interactions with each model will be done via this wrapper class.
    helperArgs = demoArgs
    for i in range(numModels):
        # Instantiate a new helper for the model and initialize it
        helper = d.DemoHelper()
        helperArgs.labels = demoArgs.labelsList[i]
        helperArgs.model = demoArgs.modelsList[i]
        helperArgs.compiledModel = demoArgs.compiledList[i]        
        helper.initialize(helperArgs)
        helper.init_image_source()
        # Add it to the list
        models.append(helper)

    return models

def main(args):
    """Main function for the Side By Side tutorial"""

    helper = d.DemoHelper()    
    arg_parser = argparse.ArgumentParser(
        "Runs a number of ELL models that predict the same categories, passing images from camera or static image file\n"
        "in a round-robin fashion. The output is a tiled image, where each tile is the result of one model."
        "Either the ELL model files, or the compiled models' Python modules must be given,\n"
        "using the --models or --compiledModels options respectively.\n"
        "Example:\n"
        "   python sideBySideDemo.py categories1.txt,categories2.txt --compiledModels models/pi3/model1,models/pi3/model2\n"
        "   python sideBySideDemo.py sameCategories.txt --models model3.ell,model4.ell\n"
        "This shows opencv window with image classified by the models using given labels")
    helper.add_arguments(arg_parser)        
    demoArgs = arg_parser.parse_args(args)
    models = get_model_helpers(demoArgs)
    if (len(models) < 1):
        print('Found no models to run')
        sys.exit()

    tiledImage = d.TiledImage(len(models))
    done = False
    while (not done):
        # Grab next frame
        frame = models[0].get_next_frame()

        # Run through models in random order to get a fairer average of evaluation time
        modelIndexes = np.arange(len(models))
        np.random.shuffle(modelIndexes)

        for modelIndex in modelIndexes:
            model = models[modelIndex]
            # Prepare the image to send to the model.
            # This involves scaling to the required input dimension and re-ordering from BGR to RGB
            data = model.prepare_image_for_predictor(frame)

            # Get the compiled model to classify the image, by returning a list of probabilities for the classes it can detect
            model.predict(data)

            # Get the (at most) top 5 predictions that meet our threshold.
            top5 = model.get_top_n(model.results, 5)

            # Turn the top5 into a text string to display
            header_text = ", ".join(["(" + str(int(100 * element[1])) + "%)" + model.get_label(element[0]) for element in top5])

            # Draw the prediction text as a header
            modelFrame = np.copy(frame)
            model.draw_header(modelFrame, header_text)
            # Draw the evaluation time as the footer
            evaluationTime = model.get_times() * 1000 # time in ms
            if (evaluationTime is not None):
                footerText = '{:.0f}'.format(evaluationTime) + 'ms/frame, ' + model.model_name
                model.draw_footer(modelFrame, footerText)

            # Set the image with the header and footer text as one of the tiles
            tiledImage.set_image_at(modelIndex, modelFrame)
            tiledImage.show()

            done = models[0].done()
            if done: break

if __name__ == "__main__":
    args = sys.argv
    args.pop(0) # when an args list is passed to parse_args, the first argument (program name) needs to be dropped
    main(sys.argv)
