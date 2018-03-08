#!/usr/bin/env python3
###################################################################################################
##
##  Project:  Embedded Learning Library (ELL)
##  File:     test_audio.py
##  Authors:  Chris Lovett
##
##  Requires: Python 3.x
##
###################################################################################################
import argparse
import sys
import os

import numpy as np

# helper classes
import classifier
import featurizer
import microphone
import speaker
import wav_reader


window_size = 40 # classifier is looking at this many features at once
threshold = 0.6 # only report predictions with greater than 60% confidence
sample_rate = 16000 # classifier was trained on 16kHz samples
channels = 1 # classifier was trained on mono audio
smoothing = 0.2 # 0.2 second smoothing window on classifier output

parser = argparse.ArgumentParser("test the classifier+featurizer against mic or wav file input")
parser.add_argument("--wav_file", help="optional path to wav file to test", default=None)
parser.add_argument("--feature_model", "-f", help="specify path to featurizer model (*.ell or compiled_folder/model_name)", required=True)
parser.add_argument("--classifier_model", "-c", help="specify path to classifier model (*.ell or compiled_folder/model_name)", required=True)
parser.add_argument("--categories", "-cat", help="specify path to categories file", required=True)
parser.add_argument("--sample_rate", "-s", help="Audio sample rate expected by classifier", default=16000, type=int)
parser.add_argument("--threshold", "-t", help="Classifier threshold (default 0.6)", default=0.6, type=float)

args = parser.parse_args()

transform = featurizer.AudioTransform(args.feature_model, window_size)
predictor = classifier.AudioClassifier(args.classifier_model, args.categories, None, threshold, smoothing)
sample_rate = args.sample_rate
threshold = args.threshold

if transform.using_map != predictor.using_map:
    raise Exception("cannot mix .ell and compiled models")

# setup inputs and outputs
if args.wav_file:
    speaker = speaker.Speaker() # output wav file to speakers at the same time
    reader = wav_reader.WavReader(sample_rate, channels)
    reader.open(args.wav_file, transform.input_size, speaker)
else:
    reader = microphone.Microphone(True)
    reader.open(transform.input_size, sample_rate, channels)
    print("Please type 'x' and enter to terminate this app...")

transform.open(reader)

try:
    while True :
        feature_data = transform.read()
        if feature_data is None:
            break
        else:
            prediction, probability, label = predictor.predict(feature_data)
            if probability is not None and probability >= threshold:                
                percent = int(100 * probability)
                print("<<< DETECTED ({}) {}% {} >>>".format(prediction, percent, label))

except KeyboardInterrupt:
    transform.close()


average_time = predictor.avg_time() + transform.avg_time()
print("Average processing time: {}".format(average_time))