#!/usr/bin/env python3
###################################################################################################
##
##  Project:  Embedded Learning Library (ELL)
##  File:     eval_test.py
##  Authors:  Chris Lovett
##
##  Requires: Python 3.x
##
###################################################################################################

# evaluate accuracy of model against the given testing dataset.
import argparse
import os
import sys
import time

import numpy as np

import classifier
import featurizer
import wav_reader

def str2bool(v):
    return v.lower() in [ "true", "1", "yes"]
    
threshold = 0.01
window_size = 80
smoothing = 0.2 # 0.2 second smoothing window on classifier output
ignore_labels = [0]

class FeatureReader:
    def __init__(self, features, classifier_input_size):
        width = int(len(features) / classifier_input_size)
        self.features = np.reshape(features, (width, classifier_input_size))
        self.pos = 0
    
    def read(self):
        if self.pos < len(self.features):
            self.pos += 1
            return self.features[self.pos - 1]
        return None


class ModelTester:
    def __init__(self, verbose):
        
        self.passed = 0
        self.failed = 0
        self.rate = 0
        self.verbose = verbose

    def get_prediction(self, transform, predictor):        
        eof = False
        best_prediction = None
        best_probability = 0
        while not eof :
            feature_data = transform.read()
            if feature_data is None:
                eof = True
            else:
                prediction, probability, label = predictor.predict(feature_data)
                if probability is not None and probability > best_probability:
                    best_probability = probability
                    best_prediction = label

        return best_prediction

    def process_prediction(self, prediction, expected):
        if prediction == expected:
            self.passed += 1
            self.rate = self.passed * 100 / (self.passed + self.failed)
            if self.verbose:
                print("PASSED {:.2f}%: {}".format(self.rate, self.expected)) 
            else:
                print("PASSED {}".format(expected)) 
        else:
            self.failed += 1
            self.rate = self.passed / (self.passed + self.failed)
            if self.verbose:
                print("FAILED {:.2f}%: expected {}, got {}".format(self.rate, expected, prediction))
            else:
                print("FAILED: expected {}, got {}".format(expected, prediction))

    def RunTest(self, feature_model, classifier_model, list_file, dataset, categories, sample_rate):

        transform = featurizer.AudioTransform(feature_model, window_size)
        predictor = classifier.AudioClassifier(classifier_model, categories, ignore_labels, threshold, smoothing)

        print("Evaluation with transform input size {}, output size {}".format(transform.input_size, transform.output_size))
        print("Evaluation with classifier input size {}, output size {}".format(predictor.input_size, predictor.output_size))

        if transform.using_map != predictor.using_map:
            raise Exception("cannot mix .ell and compiled models")

        if list_file:
            with open(list_file, "r") as fp:
                testlist = [e.strip() for e in fp.readlines()]

            wav_dir = os.path.dirname(list_file)

            start = time.time()

            for name in testlist:
                # bed/28497c5b_nohash_0.wav
                expected = name.split('/')[0]
                wav_file = os.path.join(wav_dir, "audio", name)
                # open the wav file.
                reader = wav_reader.WavReader(sample_rate)
                reader.open(wav_file, transform.input_size, None)
                transform.open(reader)
                prediction = self.get_prediction(transform, predictor)
                self.process_prediction(prediction, expected)


        elif dataset:
            if type(dataset) is str:
                ds = np.load(dataset)
                features = ds['features']
                labels = ds['labels']
            else:
                features = dataset.features
                labels = dataset.label_names

            index = 0            
            
            start = time.time()

            for f in features:
                expected = labels[index]
                reader = FeatureReader(f, predictor.input_size)
                prediction = self.get_prediction(reader, predictor)
                self.process_prediction(prediction, expected)
                index += 1

        end = time.time()
        seconds = end - start

        print("Test completed in {:.2f} seconds".format(seconds))
        print("{} passed, {} failed, pass rate of {:.2f} %".format(self.passed, self.failed, self.rate * 100))
        return self.rate


if __name__ == "__main__":
    parser = argparse.ArgumentParser("test the given featurizer+classifier against test input set")
    parser.add_argument("--feature_model", "-f", help="specify path to featurizer model (*.ell or compiled_folder/model_name)", required=True)
    parser.add_argument("--classifier_model", "-c", help="specify path to classifier model (*.ell or compiled_folder/model_name)", required=True)
    parser.add_argument("--list_file", "-l", help="specify path to testing_list.txt")
    parser.add_argument("--dataset", "-d", help="specify path to cached dataset file (*.npz)")
    parser.add_argument("--categories", "-cat", help="specify path to categories file", required=True)
    parser.add_argument("--verbose", "-v", help="provide more output", default="False")
    parser.add_argument("--sample_rate", "-r", help="specify audio rate (default 16000)", default=16000, type=int)    

    args = parser.parse_args()
    verbose = str2bool(args.verbose)
    sample_rate = args.sample_rate

    if args.list_file:
        if not os.path.isfile(args.list_file):
            print("Could not find list file at: " + args.list_file)
            sys.exit(1)
    elif args.dataset:
        if not os.path.isfile(args.dataset):
            print("Could not find dataset file at: " + args.dataset)
            sys.exit(1)
    else:
        print("Expecting one of --list_file or --dataset")
        sys.exit(1)

    test = ModelTester(verbose)
    test.RunTest(args.feature_model, args.classifier_model, args.list_file, args.dataset, args.categories, sample_rate)