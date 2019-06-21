#!/usr/bin/env python3
###################################################################################################
#
#  Project:  Embedded Learning Library (ELL)
#  File:     eval_test.py
#  Authors:  Chris Lovett
#
#  Requires: Python 3.x
#
###################################################################################################

# evaluate accuracy of model against the given testing dataset.
import argparse
import json
import os
import sys
import time

sys.path += [os.path.join(os.path.dirname(__file__), "..")]
sys.path += [os.path.join(os.path.dirname(__file__), "..", "..")]
import numpy as np

import classifier
import featurizer
import wav_reader
import logger

THRESHOLD = 0.01
SMOOTHING = 0  # no smoothing window on classifier output


class FeatureReader:
    """ FeatureReader takes in the full set of features and returns classifier_input_size chunks of data """

    def __init__(self, features, classifier_input_size):
        """ Initialize the FeatureReader with the full set of features, and the classifier input size """
        width = int(len(features) / classifier_input_size)
        self.features = np.reshape(features, (width, classifier_input_size))
        self.pos = 0

    def read(self):
        """ Return the next classifier input of size (feature_width, classifier_input_size) """
        if self.pos < len(self.features):
            self.pos += 1
            return self.features[self.pos - 1]
        return None


class AudioModelTester:
    """
    The ModelTester helper class provides the ability to test an ELL model against a given dataset
    or list file input where the inputs are featurized already or they are raw wav files that need
    to be featurized.
    """

    def __init__(self, reset):
        """ Initialize the AudioModelTester with optional verbose and reset flags """
        self.passed = 0
        self.failed = 0
        self.rate = 0
        self.logger = logger.get()
        self.verbose = self.logger.getVerbose()
        self.silent = self.logger.getSilent()
        self.reset = reset
        self.best_time = None

    def get_prediction(self, name, transform, predictor, prediction_algorithm="max"):
        """
        Get the best prediction from the the given transform using the given predictor.
        The transform represents a single test row or test file so we know already it represents
        a single word. The 'best' prediction across all the features in that word is the one
        with highest confidence no matter where it was in that word.
         """
        eof = False
        best_prediction = None
        best_probability = 0
        label = None
        if self.reset:
            predictor.reset()
        sum_prediction = None
        count = 0

        while not eof:
            feature_data = transform.read()
            if feature_data is None:
                eof = True
            else:
                predictor.total_time = 0
                _, probability, label, output = predictor.predict(feature_data)
                if output is not None:
                    if sum_prediction is None:
                        sum_prediction = output
                    else:
                        sum_prediction += output
                count += 1
                elapsed = predictor.total_time * 1000
                if elapsed != 0:  # hmmm, sometimes python time.time() lies?
                    if self.best_time is None or elapsed < self.best_time:
                        self.best_time = elapsed
                        self.logger.info("best time={}".format(self.best_time))

                if probability is not None and probability > best_probability:
                    best_probability = probability
                    best_prediction = label

        if prediction_algorithm == "mean" and count > 0:
            prediction = np.argmax(sum_prediction)
            best_probability = sum_prediction[prediction] / count
            if predictor.categories and prediction < len(predictor.categories):
                best_prediction = predictor.categories[prediction]

        return (best_prediction, best_probability, label, elapsed)

    def process_prediction(self, name, prediction, expected, confidence):
        """
        Print result of a given prediction and whether the test passed or failed given the expected result
        """
        if prediction == expected:
            self.passed += 1
            self.rate = self.passed / (self.passed + self.failed)
            if self.verbose:
                self.logger.info("PASSED {:.2f}%: {} in {} with confidence {:.2f}%".format(
                    self.rate * 100, expected, name, confidence * 100))
            elif not self.silent:
                self.logger.info("PASSED {:.2f}%: {}".format(self.rate * 100, expected))
        else:
            self.failed += 1
            self.rate = self.passed / (self.passed + self.failed)
            if self.verbose:
                self.logger.error("FAILED {:.2f}%: expected {}, got {} in {}".format(
                    self.rate * 100, expected, prediction, name))
            elif not self.silent:
                self.logger.error("FAILED: {}, expecting {}, path={}".format(prediction, expected, name))
        return prediction == expected

    def run_test(self, featurizer_model, classifier_model, list_file, max_tests, dataset, categories, sample_rate,
                 auto_scale, output_file, algorithm="max", window_size=0):
        """
        Run the test using the given input models (featurizer and classifier) which may or may not be compiled.
        The test set is defined by a list_file or a dataset.  The list file lists .wav files which we will featurize
        using the given featurizer.  The dataset contains pre-featurized data as created by make_dataset.py.
        The categories define the names of the keywords detected by the classifier and the sample_rate defines the
        audio sample rate in Hertz -- all input audio is resampled at this rate before featurization.
        """
        predictor = classifier.AudioClassifier(classifier_model, categories, THRESHOLD, SMOOTHING)
        if window_size == 0:
            window_size = predictor.input_size
        transform = featurizer.AudioTransform(featurizer_model, window_size)

        if not self.silent:
            self.logger.info("Evaluation with transform input size {}, output size {}".format(
                transform.input_size, transform.output_size))
            self.logger.info("Evaluation with classifier input size {}, output size {}".format(
                predictor.input_size, predictor.output_size))

        if transform.using_map != predictor.using_map:
            raise Exception("cannot mix .ell and compiled models")

        results = []

        if list_file:
            with open(list_file, "r") as fp:
                testlist = [e.strip() for e in fp.readlines()]

            wav_dir = os.path.dirname(list_file)

            if max_tests:
                testlist = np.random.choice(testlist, max_tests, replace=False)

            start = time.time()

            for name in testlist:
                # e.g. bed/28497c5b_nohash_0.wav
                expected = name.split('/')[0]
                wav_file = os.path.join(wav_dir, name)
                # open the wav file.
                reader = wav_reader.WavReader(sample_rate, 1, auto_scale)
                reader.open(wav_file, transform.input_size, None)
                transform.open(reader)
                prediction, confidence, _, elapsed = self.get_prediction(name, transform, predictor, algorithm)
                self.process_prediction(name, prediction, expected, confidence)
                results += [prediction]
                if self.best_time is None or elapsed < self.best_time:
                    self.best_time = elapsed

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
                name = "row " + str(index)
                prediction, confidence, _, elapsed = self.get_prediction(name, reader, predictor)
                self.process_prediction(name, prediction, expected, confidence)
                if self.best_time is None or elapsed < self.best_time:
                    self.best_time = elapsed
                index += 1
        else:
            raise Exception("Missing list_file and dataset arguments")

        end = time.time()
        seconds = end - start

        self.logger.info("Saving '{}'".format(output_file))
        with open(output_file, "w") as f:
            json.dump(results, f)

        self.logger.info("Test completed in {:.2f} seconds".format(seconds))
        self.logger.info("{} passed, {} failed, pass rate of {:.2f} %".format(
            self.passed, self.failed, self.rate * 100))
        self.logger.info("Best prediction time was {} seconds".format(self.best_time))
        return self.rate, self.best_time


def verify_file_exists(name, path):
    if not os.path.isfile(path):
        logger.get().error("Could not find {} at: {}".format(name, path))
        sys.exit(1)


if __name__ == "__main__":
    parser = argparse.ArgumentParser("Test the given featurizer and classifier against test input set")
    parser.add_argument("--featurizer", "-f", required=True,
                        help="specify path to featurizer model (*.ell or compiled_folder/model_name)")
    parser.add_argument("--classifier", "-c", required=True,
                        help="specify path to classifier model (*.ell or compiled_folder/model_name)")
    parser.add_argument("--list_file", "-l", help="specify path to testing_list.txt")
    parser.add_argument("--dataset", "-d", help="specify path to cached dataset file (*.npz)")
    parser.add_argument("--categories", "-cat", help="specify path to categories file", required=True)
    parser.add_argument("--sample_rate", "-s", help="specify audio sample rate (default 16000)", default=16000,
                        type=int)
    parser.add_argument("--window_size", "-ws", help="specify classifier window size", default=0,
                        type=int)
    parser.add_argument("--reset", "-r", help="do a GRU reset between each test file", action="store_true")
    parser.add_argument("--max_tests", type=int, help="maximum number of words chosen at random from each word list",
                        default=None)
    parser.add_argument("--auto_scale", help="Whether to auto-scale audio input to range [-1, 1] (default false).",
                        action='store_true')
    parser.add_argument("--output", help="Name of text file to contain list of test results.", default="results.json")
    parser.add_argument("--prediction_algorithm", "-pa", help="Specify prediction algorithm (max or mean).",
                        default="max")

    logger.add_logging_args(parser)
    args = parser.parse_args()
    log = logger.setup(args)

    sample_rate = args.sample_rate

    if args.list_file:
        verify_file_exists("list_file", args.list_file)
    elif args.dataset:
        verify_file_exists("dataset", args.dataset)
    else:
        log.error("Expecting one of --list_file or --dataset")
        sys.exit(1)

    test = AudioModelTester(args.reset)
    test.run_test(args.featurizer, args.classifier, args.list_file, args.max_tests, args.dataset, args.categories,
                  sample_rate, args.auto_scale, args.output, args.prediction_algorithm, args.window_size)
