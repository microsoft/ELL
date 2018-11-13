import os
import sys
sys.path += [ "d:/git/ell/ell/tools/utilities/pythonlibs/audio"]
import numpy as np
import time

script_dir = os.path.dirname(os.path.abspath(__file__))
sys.path += [ script_dir ]
sys.path += [ os.getcwd() ]

import microphone
from compiled_classifier import model
from compiled_featurizer import mfcc

THRESHOLD = 0.8

mic = microphone.Microphone(True)
mic.open(256, 8000, 1)

categories = [x.strip() for x in open('categories.txt','r').readlines()]
transform = mfcc.MfccWrapper()

class Classifier(model.ModelWrapper):
    def __init__(self):
        super(Classifier, self).__init__()
        self.last_vad = None

    def VadCallback(self, buffer):
        vad = buffer[0]
        if vad != self.last_vad:
            print("vad={}".format(vad))
            self.last_vad = vad

    def Predict(self, input):
        return super(Classifier, self).Predict(model.FloatVector(input))

classifier = Classifier()

print("Enter 'x' to quit...")

best_time = None
while True:
    data = mic.read()
    if data is None:
        break
    features = transform.Predict(mfcc.FloatVector(data))
    start = time.time()
    prediction = classifier.Predict(features)
    elapsed = time.time() - start
    if best_time is None or elapsed < best_time:
        best_time = elapsed

    prediction = np.array(prediction)
    top = np.argmax(prediction)
    score = prediction[top]
    if score > THRESHOLD:
        label = categories[top]
        print("{} ({:3f} %)".format(label, score ))

print("Best prediction in {} seconds.format(best_time))
