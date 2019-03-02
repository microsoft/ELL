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

# helper classes
import classifier
import featurizer
import microphone
import speaker
import wav_reader


THRESHOLD = 0.6  # default is only report predictions with greater than 60% confidence
SAMPLE_RATE = 16000  # default is classifier was trained on 16kHz samples
CHANNELS = 1  # default classifier was trained on mono audio
SMOOTHING = 0  # default no smoothing


def test_keyword_spotter(featurizer_model, classifier_model, categories, wav_file, threshold, sample_rate,
                         output_speaker=False):
    predictor = classifier.AudioClassifier(classifier_model, categories, threshold, SMOOTHING)
    transform = featurizer.AudioTransform(featurizer_model, predictor.input_size)

    if transform.using_map != predictor.using_map:
        raise Exception("cannot mix .ell and compiled models")

    # set up inputs and outputs
    if wav_file:
        the_speaker = None
        if output_speaker:
            the_speaker = speaker.Speaker()
        reader = wav_reader.WavReader(sample_rate, CHANNELS)
        reader.open(wav_file, transform.input_size, the_speaker)
    else:
        reader = microphone.Microphone(True, True)
        reader.open(transform.input_size, sample_rate, CHANNELS)
        print("Please type 'x' and enter to terminate this app...")

    transform.open(reader)
    results = None
    try:
        while True:
            feature_data = transform.read()
            if feature_data is None:
                break
            else:
                prediction, probability, label = predictor.predict(feature_data)
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


if __name__ == "__main__":
    parser = argparse.ArgumentParser("test the classifier and featurizer against mic or wav file input")
    parser.add_argument("--wav_file", help="optional path to wav file to test", default=None)
    parser.add_argument("--featurizer", "-f", required=True,
                        help="specify path to featurizer model (*.ell or compiled_folder/model_name)")
    parser.add_argument("--classifier", "-c", required=True,
                        help="specify path to classifier model (*.ell or compiled_folder/model_name)")
    parser.add_argument("--categories", "-cat", help="specify path to categories file", required=True)
    parser.add_argument("--sample_rate", "-s", default=SAMPLE_RATE, type=int,
                        help="Audio sample rate expected by classifier")
    parser.add_argument("--threshold", "-t", help="Classifier threshold (default 0.6)", default=THRESHOLD, type=float)
    parser.add_argument("--speaker", help="Output audio to the speaker.", action='store_true')

    args = parser.parse_args()
    test_keyword_spotter(args.featurizer, args.classifier, args.categories, args.wav_file, args.threshold,
                         args.sample_rate, args.speaker)
