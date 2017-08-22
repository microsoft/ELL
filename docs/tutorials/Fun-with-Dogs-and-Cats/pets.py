####################################################################################################
##
##  Project:  Embedded Learning Library (ELL)
##  File:     pets.py
##  Authors:  Chris Lovett
##
##  Requires: Python 3.x
##
####################################################################################################

import sys
import os
import numpy as np
import cv2
import demoHelper as d
import subprocess
if (os.name == "nt"):
    import winsound

# note: to run this in headless mode on a Linux machine run the following from your terminal window
# export DISPLAY=:0
# then add the '-save' argument to get tagged frames to be saved to disk.

class Demo:
    def __init__(self, helper):        
        self.script_path = os.path.dirname(os.path.abspath(__file__))
        self.helper = helper
        self.dogs = helper.load_labels("doglabels.txt")
        self.cats = helper.load_labels("catlabels.txt")
        self.woof = os.path.join(self.script_path, "woof.wav")
        self.meow = os.path.join(self.script_path, "meow.wav")

    def play(self, filename):
        if (os.name == "nt"):
            winsound.PlaySound(filename, winsound.SND_FILENAME | winsound.SND_ASYNC)
        else:
            command = ["aplay", filename]
            proc = subprocess.Popen(command, stdout=subprocess.PIPE, stderr=subprocess.PIPE, bufsize=0, universal_newlines = True)
            proc.wait()

    def run(self):            
        # Initialize image source
        self.helper.init_image_source()
        lastPrediction = ""
        lastCaption = ""
        while (not self.helper.done()):

            # Grab next frame
            frame = self.helper.get_next_frame()

            # Prepare the image to send to the model.
            # This involves scaling to the required input dimension and re-ordering from BGR to RGB
            data = self.helper.prepare_image_for_predictor(frame)

            # Get the model to classify the image, by returning a list of probabilities for the classes it can detect
            predictions = self.helper.predict(data)

            # Get the (at most) top 5 predictions that meet our threshold. This is returned as a list of tuples,
            # each with the text label and the prediction score.
            top5 = self.helper.get_top_n(predictions, 5)
            text = ""
            caption = ""
            if (len(top5) > 0):
                winner = top5[0]
                label = winner[0]
                if (label in self.dogs):
                    text = "Dog"
                elif (label in self.cats):
                    text = "Cat"

            if (not text == ""):
                caption = text + "(" + str(int(100*winner[1])) + "%)"
            else:
                caption = ""

            save = False
            if (text != lastPrediction and text != ""):
                save = True
                lastPrediction = text
                if (text == "Dog"):
                    self.play(self.woof)
                elif (text == "Cat"):
                    self.play(self.meow)

            if (caption != lastCaption):
                print(caption)
                lastCaption = caption

            # Draw the text on the frame
            frameToShow = frame
            helper.draw_label(frameToShow, caption)
            helper.draw_fps(frameToShow)

            # Show the new frame
            helper.show_image(frameToShow, save)

if __name__ == "__main__":
    helper = d.DemoHelper()
    if (not helper.parse_arguments(sys.argv)):
        helper.print_usage()
    else:
        demo = Demo(helper)
        demo.run()
