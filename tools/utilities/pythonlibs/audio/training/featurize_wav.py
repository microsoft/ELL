# featurizes a wav file using given audio featurizer and saves result in numpy .npz file.

import argparse
import os
import sys

sys.path += [os.path.join(os.path.dirname(__file__), "..")]
import numpy as np

import featurizer
import make_dataset


def featurize_wav(wav_file, label, featurizer_path, sample_rate, window_size, shift, auto_scale, dump, drop_frames):

    transform = featurizer.AudioTransform(featurizer_path, 0)
    input_size = transform.input_size
    output_shape = transform.model.output_shape
    output_size = output_shape.Size()

    wav_log = None
    if dump:
      wav_log = open(wav_file + ".txt", "w")
      wav_log.write("{}:\n".format(wav_file))
      transform.set_log(wav_log)

    generator = make_dataset.get_wav_features(wav_file, transform, sample_rate, window_size, shift, auto_scale, None,
                                              drop_frames)
    features = list(generator)
    # reshape features to record the fact that the window_size is a kind of batch size for this type of model
    features = [np.reshape(f, (window_size, 1, 1, int(len(f) / window_size))) for f in features]
    labels = [label] * len(features)

    output_file = os.path.basename(wav_file)
    output_file = os.path.splitext(output_file)[0] + ".npz"

    parameters = (sample_rate, input_size, output_size, window_size, shift)  # remember these settings in the dataset
    np.savez(output_file, features=features, labels=labels, parameters=parameters)

    if dump:
      with open(wav_file + ".features.txt", "w") as f:
        f.write("{}:\n".format(wav_file))
        for a in features:
            f.write("{}\n".format(", ".join([str(x) for x in a.ravel()])))

      wav_log.close()
    return features


if __name__ == "__main__":
    arg_parser = argparse.ArgumentParser(description="Featurized a single .wav file the given featurizer.")

    # options
    arg_parser.add_argument("--wav_file", "-w", help="The path to the wav file", required=True)
    arg_parser.add_argument("--featurizer", "-f", help="ELL featurizer module to use", required=True)
    arg_parser.add_argument("--label", "-l", help="The truth label for this wav file", required=True)
    arg_parser.add_argument("--dump", "-d", help="Dump the data to text files *.wav.txt and *.wav.features.txt",
                            action="store_true")
    arg_parser.add_argument("--sample_rate", "-r", help="Sample rate of input", type=int, default=16000)
    arg_parser.add_argument("--window_size", "-ws", help="Classifier window size (default: 80)", type=int,
                            default=80)
    arg_parser.add_argument("--shift", "-s", help="Classifier shift amount (default: 10)", type=int,
                            default=10)
    arg_parser.add_argument("--drop_frames", "-df", help="Total initial frames to drop (default: 0)", type=int,
                            default=0)
    args = arg_parser.parse_args()

    featurize_wav(args.wav_file, args.label, args.featurizer, args.sample_rate,
                  args.window_size, args.shift, args.dump, args.drop_frames)
