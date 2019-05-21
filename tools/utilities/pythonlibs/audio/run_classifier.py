#!/usr/bin/env python3
###################################################################################################
#
#  Project:  Embedded Learning Library (ELL)
#  File:     test_audio.py
#  Authors:  Chris Lovett
#
#  Requires: Python 3.x
#
###################################################################################################
import argparse
import os

# helper classes
import featurizer
import classifier
import microphone
import speaker
import wav_reader


THRESHOLD = 0.6  # default is only report predictions with greater than 60% confidence
SAMPLE_RATE = 16000  # default is classifier was trained on 16kHz samples
CHANNELS = 1  # default classifier was trained on mono audio
SMOOTHING = 0  # default no smoothing


def get_prediction(reader, transform, predictor, categories):
    # set up inputs and outputs
    transform.open(reader)
    results = None
    try:
        while True:
            feature_data = transform.read()
            if feature_data is None:
                break
            else:
                prediction, probability, label, _ = predictor.predict(feature_data)
                if probability is not None:
                    if not results or results[1] < probability:
                        results = (prediction, probability, label)
                    percent = int(100 * probability)
                    print("<<< DETECTED ({}) {}% '{}' >>>".format(prediction, percent, label))

    except KeyboardInterrupt:
        pass

    transform.close()
    average_time = predictor.avg_time() + transform.avg_time()
    print("Average processing time: {}".format(average_time))
    if results is None:
        raise Exception("test_keyword_spotter failed to find any predictions!")
    return tuple(list(results) + [average_time])


def test_keyword_spotter(featurizer_model, classifier_model, categories, wav_files, threshold, sample_rate,
                         output_speaker=False, auto_scale=False, reset=False):

    predictor = classifier.AudioClassifier(classifier_model, categories, threshold, SMOOTHING)
    transform = featurizer.AudioTransform(featurizer_model, predictor.input_size)

    if transform.using_map != predictor.using_map:
        raise Exception("cannot mix .ell and compiled models")

    the_speaker = None
    if output_speaker:
        the_speaker = speaker.Speaker()

    results = []
    if wav_files:
        if not os.path.isdir(wav_files):
            raise Exception("--wav_files {} dir not found".format(wav_files))
        file_list = os.listdir(wav_files)
        file_list.sort()
        for filename in file_list:
            ext = os.path.splitext(filename)[1]
            if ext != ".wav":
                print("Skipping non-wav file: ", filename)
            else:
                reader = wav_reader.WavReader(sample_rate, CHANNELS, auto_scale)
                path = os.path.join(wav_files, filename)
                print("opening ", path)
                reader.open(path, transform.input_size, the_speaker)
                result = get_prediction(reader, transform, predictor, categories)
                results += [result]
                if reset:
                    predictor.reset()
    else:
        reader = microphone.Microphone(True, True)
        reader.open(transform.input_size, sample_rate, CHANNELS)
        print("Please type 'x' and enter to terminate this app...")
        result = get_prediction(reader, transform, predictor, categories)
        results += [result]

    return results


if __name__ == "__main__":
    parser = argparse.ArgumentParser("test the classifier and featurizer against mic or wav file input")
    parser.add_argument("--wav_files", help="optional path to folder containing wav files to test", default=None)
    parser.add_argument("--featurizer", "-f", required=True,
                        help="specify path to featurizer model (*.ell or compiled_folder/model_name)")
    parser.add_argument("--classifier", "-c", required=True,
                        help="specify path to classifier model (*.ell or compiled_folder/model_name)")
    parser.add_argument("--categories", "-cat", help="specify path to categories file", required=True)
    parser.add_argument("--sample_rate", "-s", default=SAMPLE_RATE, type=int,
                        help="Audio sample rate expected by classifier")
    parser.add_argument("--threshold", "-t", help="Classifier threshold (default 0.6)", default=THRESHOLD, type=float)
    parser.add_argument("--speaker", help="Output audio to the speaker.", action='store_true')
    parser.add_argument("--auto_scale", help="Whether to auto-scale audio input to range [-1, 1] (default false).",
                        action='store_true')
    parser.add_argument("--reset", help="Whether to reset model between tests (default false).",
                        action='store_true')

    args = parser.parse_args()

    results = test_keyword_spotter(args.featurizer, args.classifier, args.categories, args.wav_files, args.threshold,
                                   args.sample_rate, args.speaker, args.auto_scale, args.reset)

    print(results)
