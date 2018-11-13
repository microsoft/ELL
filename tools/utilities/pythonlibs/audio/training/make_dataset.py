#!/usr/bin/env python3
###################################################################################################
##
##  Project:  Embedded Learning Library (ELL)
##  File:     make_dataset.py
##  Authors:  Chris Lovett, Chuck Jacobs
##
##  Requires: Python 3.x
##
###################################################################################################
"""
Utility for featurizing a bunch of wav files into a training dataset.
"""
import sys
import argparse
import os
import sys

import numpy as np
import find_ell_root

from dataset import Dataset, NULL_LABEL_NAME
import featurizer
import wav_reader

def is_null_class(name):
    """ returns true if the name represents the null class, like _background_noise_ """
    return name.startswith("_")

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
            entries_to_visit[folder] = [ file_name ]

    return entries_to_visit

def lazy_apply_transform(transform):
    """ Turn the given transformed data into a Python generator with yield statement """
    while True:
        y = transform.read()
        if y is None:
            break
        # Return a transposed vector so the feature vectors stack up as a 2D sequence of vectors, rather than horizontally like one long vector
        yield y[np.newaxis, ...]

def sliding_window_frame(source, window_size, shift_amount):
    """ General windowing and merging generator that concatenates & shifts samples through a sliding window frame """
    # Source is a container or generator that returns numpy vectors
    # We buffer them and return arrays of length window_size, shifted by shift_amount 
    buffer = None
    for new_samples in source:
        # if new_samples is a scalar, turn it into a 1-element tuple    
        if np.isscalar(new_samples):
            new_samples = (new_samples,)
        if buffer is None:
            buffer = new_samples
        else:
            buffer = np.concatenate((buffer, new_samples))

        while len(buffer) >= window_size:
            yield buffer[:window_size]
            buffer = buffer[shift_amount:]

    # return the remainder, if any, to ensure we at least return 1 full frame
    if buffer is not None and len(buffer) < window_size:
        shape = buffer.shape
        if len(shape) == 2:
            new_sample = np.zeros((window_size - len(buffer), shape[1]))
        else:
            new_sample = np.zeros((window_size - len(buffer)))
        buffer = np.concatenate((buffer, new_sample))
        yield buffer[:window_size]

def get_wav_features(input_filename, transform, sample_rate, window_size, shift):
    """
    Transform the given .wav input file into a set of features given the required sample rate
    window size and shift.  The window size is the number of features we need to give the 
    classifier and the shift is the amount by which that window slides as new transformed
    features are added.
    """
    transform_input_size = transform.input_size
    transform_output_size = transform.output_size
    channels = 1  # we only do mono audio right now...
    source = wav_reader.WavReader(sample_rate, channels)
    source.open(input_filename, transform_input_size)
    # apply the featurizing transform 
    transform.open(source)
    
    source = lazy_apply_transform(transform)

    # and apply the classifier window frame size
    source = sliding_window_frame(source, window_size, shift)
    
    rows_generated = 0
    for row in source:
        features = np.ravel(row)
        rows_generated += 1
        yield features

def _get_dataset(entry_map, transform, sample_rate, window_size, shift):    
    data_rows = []
    label_rows = []

    for e in entry_map:
        label = os.path.basename(e)
        
        if is_null_class(label): # special class directory
            label = NULL_LABEL_NAME
        total = 0
        print("Transforming {} files from {} ... ".format(len(entry_map[e]), label), end='', flush=True)

        for file in entry_map[e]:
            full_path = os.path.join(e, file)
            file_features = list(get_wav_features(full_path, transform, sample_rate, window_size, shift))
            if len(file_features) > 0:
                rows = np.array([r[0] for r in file_features])
                labels = [label for r in file_features]
                data_rows.append(file_features)
                label_rows.append(labels)
            total += len(file_features)

        print(" found {} rows".format(total))

    features = np.concatenate(data_rows, axis=0)
    label_names = np.concatenate(label_rows, axis=0)
    parameters = (sample_rate, transform.input_size, transform.output_size, window_size, shift) # remember these settings in the dataset
    return Dataset(features, label_names, parameters)

def make_dataset(list_file, featurizer_path, sample_rate, window_size, shift):

    """
    Create a dataset given the input list file, a featurizer, the desired .wav sample rate,
    classifier window_size and window shift amount.  The dataset is saved to the same file name
    with .npz extension.
    """
    transform = featurizer.AudioTransform(featurizer_path, 0)
    input_size = transform.input_size
    output_shape = transform.model.output_shape
    output_size = output_shape.Size()
    feature_size = output_shape.Size()
    feature_shape = (output_shape.rows, output_shape.columns, output_shape.channels)

    entry_map = parse_list_file(list_file)

    dataset = _get_dataset(entry_map, transform, sample_rate, window_size, shift)
    if len(dataset.features) == 0:
        print("No features found in list file")

    dataset_name = os.path.basename(list_file)
    dataset_path = os.path.splitext(dataset_name)[0] + ".npz"
    print("Saving: {}".format(dataset_path))
    dataset.save(dataset_path)
    

if __name__ == "__main__":
    arg_parser = argparse.ArgumentParser(description="Create featurized dataset using " +
        "the given featurizer and list of wav files")

    # options
    arg_parser.add_argument("--list_file", "-l", help="The path to the list file to process")
    arg_parser.add_argument("--featurizer", "-f", help="Compiled featurizer module to use", 
        default="featurizer/mfcc")
    arg_parser.add_argument("--sample_rate", "-r", help="Sample rate of input", 
        type=int, default=16000)
    arg_parser.add_argument("--window_size", "-ws", 
        help="Classifier window size (default: 80)", type=int, 
        default=80)
    arg_parser.add_argument("--shift", "-s", 
        help="Classifier shift amount (default: 10)", type=int, 
        default=10)
    args = arg_parser.parse_args()

    make_dataset(args.list_file, args.featurizer, args.sample_rate, 
                  args.window_size, args.shift)

