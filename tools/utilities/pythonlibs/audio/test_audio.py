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
import time

import numpy as np

# helper classes
import classifier
import featurizer
import microphone
import speaker
import wav_reader


THRESHOLD = 0.6 # default is only report predictions with greater than 60% confidence
SAMPLE_RATE = 16000 # default is classifier was trained on 16kHz samples
CHANNELS = 1 # default classifier was trained on mono audio
SMOOTHING = 0 # default no smoothing 

parser = argparse.ArgumentParser("test the classifier and featurizer against mic or wav file input")
parser.add_argument("--wav_file", help="optional path to wav file to test", default=None)
parser.add_argument("--featurizer", "-f", help="specify path to featurizer model (*.ell or compiled_folder/model_name)", required=True)
parser.add_argument("--classifier", "-c", help="specify path to classifier model (*.ell or compiled_folder/model_name)", required=True)
parser.add_argument("--categories", "-cat", help="specify path to categories file", required=True)
parser.add_argument("--sample_rate", "-s", help="Audio sample rate expected by classifier", default=SAMPLE_RATE, type=int)
parser.add_argument("--threshold", "-t", help="Classifier threshold (default 0.6)", default=THRESHOLD, type=float)
parser.add_argument("--speaker", help="Output audio to the speaker.", action='store_true')

args = parser.parse_args()

predictor = classifier.AudioClassifier(args.classifier, args.categories, [0], args.threshold, SMOOTHING)
transform = featurizer.AudioTransform(args.featurizer, predictor.input_size)

if transform.using_map != predictor.using_map:
    raise Exception("cannot mix .ell and compiled models")

# setup inputs and outputs
if args.wav_file:
    output_speaker = None
    if args.speaker:
        output_speaker = speaker.Speaker() # output wav file to speakers at the same time
    reader = wav_reader.WavReader(args.sample_rate, CHANNELS)
    reader.open(args.wav_file, transform.input_size, output_speaker)
else:
    reader = microphone.Microphone(True)
    reader.open(transform.input_size, args.sample_rate, CHANNELS)
    print("Please type 'x' and enter to terminate this app...")

transform.open(reader)

try:
    while True :
        feature_data = transform.read()
        if feature_data is None:
            break
        else:
            prediction, probability, label = predictor.predict(feature_data)
            if probability is not None:                
                percent = int(100 * probability)
                print("<<< DETECTED ({}) {}% '{}' >>>".format(prediction, percent, label))

except KeyboardInterrupt:
    pass


transform.close()

average_time = predictor.avg_time() + transform.avg_time()
print("Average processing time: {}".format(average_time))