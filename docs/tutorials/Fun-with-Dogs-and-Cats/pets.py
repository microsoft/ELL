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
import time
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
        self.last = time.time()

    def play(self, filename):
        if (os.name == "nt"):
            winsound.PlaySound(filename, winsound.SND_FILENAME | winsound.SND_ASYNC)
        else:
            command = ["aplay", filename]
            proc = subprocess.Popen(command, stdout=subprocess.PIPE, stderr=subprocess.PIPE, bufsize=0, universal_newlines = True)
            proc.wait()
    
    
    def labels_match(self, a, b):
        x = [s.strip().lower() for s in a.split(',')]
        y = [s.strip().lower() for s in b.split(',')]
        for w in x:
            if (w in y):
                return True
        return False

    def label_in_set(self, label, label_set):
        for x in label_set:
            if self.labels_match(label, x):
                return True
        return False


    def run(self):            
        # Initialize image source
        self.helper.init_image_source()
        lastPrediction = ""
        lastCaption = ""
        lastHist = None
        lastFrame = None     
        lastLabel = None
        significantDiff = 5000
        while not self.helper.done():

            # Grab next frame
            frame = self.helper.get_next_frame()

            # Prepare the image to send to the model.
            # This involves scaling to the required input dimension and re-ordering from BGR to RGB
            data = self.helper.prepare_image_for_predictor(frame)

            hist = np.histogram(data,16,[0,256])[0]
            diff = 1
            if lastHist is None:
                lastHist = hist           
            else:
                diff = max(lastHist - hist)
                lastHist = hist  

            # Get the model to classify the image, by returning a list of probabilities for the classes it can detect
            predictions = self.helper.predict(data)

            # Get the (at most) top 5 predictions that meet our threshold. This is returned as a list of tuples,
            # each with the text label and the prediction score.
            top5 = self.helper.get_top_n(predictions, 5)
            text = ""
            caption = ""
            label = ""
            if len(top5) > 0:
                winner = top5[0]
                label = self.helper.get_label(winner[0])
                if self.label_in_set(label, self.dogs):
                    text = "Dog"
                elif self.label_in_set(label, self.cats):
                    text = "Cat"

            if not text == "":
                caption = text + "(" + str(int(100*winner[1])) + "%)"
            else:
                caption = ""

            if caption != lastCaption:
                lastCaption = caption

            if lastLabel != label:
                print(label)
                lastLabel = label

            now = time.time()
            if diff >= significantDiff and now - self.last > 2:
                lastPrediction = ""

            save = False
            if text != lastPrediction and text != "":
                save = True
                self.last = now
                lastPrediction = text
                if text == "Dog":
                    self.play(self.woof)
                elif text == "Cat":
                    self.play(self.meow)

            # Draw the text on the frame
            frameToShow = frame
            helper.draw_label(frameToShow, caption)
            helper.draw_fps(frameToShow)

            # Show the new frame
            helper.show_image(frameToShow, save)

if __name__ == "__main__":
    args = sys.argv
    args.pop(0) # when an args list is passed to parse_args, the first argument (program name) needs to be dropped
    helper = d.DemoHelper()
    helper.parse_arguments(args,
            "Runs the given ELL model passing images from camera or static image file\n"
            "Either the ELL model file, or the compiled model's Python module must be given,\n"
            "using the --model or --compiledModel options respectively.\n"
            "Example:\n"
            "   python pets.py categories.txt --compiledModel tutorial1/pi3/model1\n"
            "   python pets.py categories.txt --model model1.ell\n"
            "This groups predictions from an image classifier by cats and dogs and plays an appropriate ")
            "sound for one of these groups is recognized")
    demo = Demo(helper)
    demo.run()
