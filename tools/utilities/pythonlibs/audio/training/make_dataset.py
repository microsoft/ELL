#!/usr/bin/env python3
###################################################################################################
#
#  Project:  Embedded Learning Library (ELL)
#  File:     make_dataset.py
#  Authors:  Chris Lovett, Chuck Jacobs
#
#  Requires: Python 3.x
#
###################################################################################################
"""
Utility for featurizing a bunch of wav files into a training dataset.
"""
import argparse
import os
import sys

sys.path += [os.path.join(os.path.dirname(__file__), "..")]
import numpy as np

from dataset import Dataset
import featurizer
import wav_reader
import noise_mixer
import multiprocessing
from functools import partial


def parse_list_file(list_file):
    """
    Load the list file which contains 'dir/filename' format on each line
    We sort these into one set per directory, the directory name then becomes the supervised
    training label we want the model to learn.
    """
    data_root = os.path.dirname(list_file)
    with open(list_file, "r") as fp:
        full_list = [e.strip() for e in fp.readlines()]
    # group the list by folders
    entries_to_visit = {}
    for e in full_list:
        label, file_name = os.path.split(e)
        folder = os.path.abspath(os.path.join(data_root, label))
        if folder in entries_to_visit:
            entries_to_visit[folder].append(file_name)
        else:
            entries_to_visit[folder] = [file_name]

    return entries_to_visit


def lazy_apply_transform(transform):
    """ Turn the given transformed data into a Python generator with yield statement """
    while True:
        y = transform.read()
        if y is None:
            break
        # Return a transposed vector so the feature vectors stack up as a 2D sequence of vectors,
        # rather than horizontally like one long vector
        yield y[np.newaxis, ...]


def sliding_window_frame(transform, source, window_size, shift_amount, noise_mixer):
    """ General windowing and merging generator that concatenates & shifts samples through a sliding window frame """
    # Source is a container or generator that returns numpy vectors
    # We buffer them and return arrays of length window_size, shifted by shift_amount
    buffer = None
    count = 0
    for new_samples in source:
        # if new_samples is a scalar, turn it into a 1-element tuple
        if np.isscalar(new_samples):
            new_samples = (new_samples,)
        if buffer is None:
            buffer = new_samples
        else:
            buffer = np.concatenate((buffer, new_samples))

        while len(buffer) >= window_size:
            count += 1
            yield buffer[:window_size]
            buffer = buffer[shift_amount:]

    # return the remainder, if any, to ensure we at least return 1 full frame by
    # adding duplicates of previous sample (adding a bit of noise for variety...)
    std_dev = new_samples.std()
    while buffer is not None and len(buffer) < window_size and (len(buffer) > window_size / 4 or count == 0):
        mixed = None
        if noise_mixer:
            mixed = noise_mixer.read_noise()
            if mixed is not None:
                transformed = transform.model.transform(mixed)
                mixed = transformed[np.newaxis, ...]
        if mixed is None:
            # just add random noise to the last sample we read
            mixed = new_samples + np.random.normal(scale=std_dev / 2, size=new_samples.shape[1])[np.newaxis, ...]
        buffer = np.concatenate((buffer, mixed))

    if len(buffer) == window_size:
        yield buffer[:window_size]


def get_wav_features(input_filename, transform, sample_rate, window_size, shift, auto_scale, mixer):
    """
    Transform the given .wav input file into a set of features given the required sample rate
    window size and shift.  The window size is the number of features we need to give the
    classifier and the shift is the amount by which that window slides as new transformed
    features are added.
    """
    transform_input_size = transform.input_size
    channels = 1  # we only do mono audio right now...
    source = wav_reader.WavReader(sample_rate, channels, auto_scale)
    source.open(input_filename, transform_input_size)

    if mixer:
        mixer.open(source)
        source = mixer

    # apply the featurizing transform
    transform.open(source)

    source = lazy_apply_transform(transform)

    # and apply the classifier window frame size
    try:
        source = sliding_window_frame(transform, source, window_size, shift, mixer)

        rows_generated = 0
        for row in source:
            features = np.ravel(row)
            rows_generated += 1
            yield features
    except Exception as e:
        print("### error transforming input file {}: {}".format(input_filename, e))

    if rows_generated == 0:
        print("### no rows generated for input file: {}".format(input_filename))


def multiprocess_data(file, e, featurizer_path, sample_rate, window_size, shift, auto_scale, noise_path,
                      max_noise_ratio, noise_selection, label):
    data_rows = []
    label_rows = []
    transform = featurizer.AudioTransform(featurizer_path, 0)
    mixer = None
    if noise_path:
        noise_files = [os.path.join(noise_path, f) for f in os.listdir(noise_path)
                       if os.path.splitext(f)[1] == ".wav"]
        mixer = noise_mixer.AudioNoiseMixer(noise_files, max_noise_ratio, noise_selection)
    full_path = os.path.join(e, file)
    file_features = list(get_wav_features(full_path, transform, sample_rate, window_size, shift, auto_scale,
                                          mixer))
    if len(file_features) > 0:
        labels = [label for r in file_features]
        data_rows.append(file_features)
        label_rows.append(labels)
    return data_rows, label_rows


def _get_dataset(entry_map, categories, featurizer_path, window_size, shift, noise_path, multicore=False,
                 max_noise_ratio=0.1, noise_selection=0.1):
    data_rows = []
    label_rows = []
    transform = featurizer.AudioTransform(featurizer_path, 0)
    sample_rate = transform.get_metadata("sample_rate")
    if not sample_rate:
        raise Exception("Featurizer is missing 'sample_rate' metadata")
    sample_rate = int(sample_rate)
    auto_scale = transform.get_metadata("auto_scale")
    if auto_scale:
        auto_scale = bool(auto_scale)
        if auto_scale:
            print("Featurizer requires auto-scaling of audio input to float range [-1, 1]")
    else:
        auto_scale = False
    mixer = None
    if noise_path:
        noise_files = [os.path.join(noise_path, f) for f in os.listdir(noise_path)
                       if os.path.splitext(f)[1] == ".wav"]
        mixer = noise_mixer.AudioNoiseMixer(noise_files, max_noise_ratio, noise_selection)
    for e in entry_map:
        label = os.path.basename(e)
        if label not in categories:
            raise Exception("label {} not found in categories file".format(label))
        print("Transforming {} files from {} ... ".format(len(entry_map[e]), label), end='', flush=True)
        total = 0
        if multicore:
            p = multiprocessing.Pool(multiprocessing.cpu_count())
            temp_partial = partial(multiprocess_data, e=e, featurizer_path=featurizer_path, sample_rate=sample_rate,
                                   window_size=window_size, shift=shift, auto_scale=auto_scale, noise_path=noise_path,
                                   max_noise_ratio=max_noise_ratio, noise_selection=noise_selection, label=label)
            temp_file = p.map(temp_partial, entry_map[e])
            for temp_list in temp_file:
                temp1, temp2 = temp_list
                for x in temp1:
                    data_rows.append(x)
                for y in temp2:
                    label_rows.append(y)
            total += len(label_rows)
            print(" found {} rows".format(total))
            p.close()
            p.join()
        else:
            for file in entry_map[e]:
                full_path = os.path.join(e, file)
                file_features = list(get_wav_features(full_path, transform, sample_rate, window_size, shift, auto_scale,
                                                      mixer))
                if len(file_features) > 0:
                    labels = [label for r in file_features]
                    data_rows.append(file_features)
                    label_rows.append(labels)
                total += len(file_features)

            print(" found {} rows".format(total))

    features = np.concatenate(data_rows, axis=0)
    label_names = np.concatenate(label_rows, axis=0)
    # remember these settings in the dataset
    parameters = (sample_rate, transform.input_size, transform.output_size, window_size, shift)
    return Dataset(features, label_names, categories, parameters)


def make_dataset(list_file, outdir, categories_path, featurizer_path, window_size, shift,
                 noise_path=None, max_noise_ratio=0.1, noise_selection=0.1, multicore=False, use_cache=False):
    """
    Create a dataset given the input list file, a featurizer, the desired .wav sample rate,
    classifier window_size and window shift amount.  The dataset is saved to the same file name
    with .npz extension.  This will do nothing if dataset is already created, unless use_cache=False.
    """
    dataset_name = os.path.basename(list_file)
    dataset_path = os.path.splitext(dataset_name)[0] + ".npz"
    if not outdir:
        dir_name = os.path.dirname(list_file)
        dataset_path = os.path.join(dir_name, dataset_path)
    else:
        dataset_path = os.path.join(outdir, dataset_path)

    if use_cache and os.path.isfile(dataset_path):
        return
    entry_map = parse_list_file(list_file)
    if not categories_path:
        dir_name = os.path.dirname(list_file)
        categories_path = os.path.join(dir_name, "categories.txt")
    if not os.path.isfile(categories_path):
        raise Exception("{} file not found".format(categories_path))
    categories = [x.strip() for x in open(categories_path, 'r').readlines()]
    print("Transforming {} with window size={} and shift={}".format(
        os.path.basename(list_file), window_size, shift))

    dataset = _get_dataset(entry_map, categories, featurizer_path, window_size, shift, noise_path, multicore,
                           max_noise_ratio, noise_selection)
    if len(dataset.features) == 0:
        print("No features found in list file")

    print("Saving: {}".format(dataset_path))
    dataset.save(dataset_path)


if __name__ == "__main__":
    arg_parser = argparse.ArgumentParser(description="""Create featurized dataset using the given featurizer
  and list of wav files""")

    # options
    arg_parser.add_argument("--list_file", "-l", help="The path to the list file to process")
    arg_parser.add_argument("--outdir", "-o", help="The path where you want the *.npz files saved")
    arg_parser.add_argument("--categories", "-c",
                            help="The full list of labels (a given list file might only see a subset of these)")
    arg_parser.add_argument("--featurizer", "-f", help="Compiled featurizer module to use", default="featurizer/mfcc")
    arg_parser.add_argument("--window_size", "-ws", help="Classifier window size (default: 80)", type=int, default=80)
    arg_parser.add_argument("--shift", "-s", help="Classifier shift amount (default: 10)", type=int, default=10)
    arg_parser.add_argument("--noise_path", help="Path to noise folder, if provided noise from this folder will be "
                            "mixed with some of the audio files in list_file", default=None)
    arg_parser.add_argument("--max_noise_ratio", type=float, default=0.1,
                            help="Specifies the ratio of noise to audio (default 0.1)")
    arg_parser.add_argument("--noise_selection", type=float, default=0.1,
                            help="Ratio of audio files to mix with noise (default 0.1)")
    arg_parser.add_argument("--multicore", help="parallel processing of data", action="store_true")
    args = arg_parser.parse_args()

    if args.noise_path and not os.path.isdir(args.noise_path):
        print("Noise dir '{}' not found".format(args.noise_path))
        sys.exit(1)

    make_dataset(args.list_file, args.outdir, args.categories, args.featurizer, args.window_size,
                 args.shift, args.noise_path, args.max_noise_ratio, args.noise_selection, args.multicore)
