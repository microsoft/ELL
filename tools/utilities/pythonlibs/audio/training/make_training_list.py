#!/usr/bin/env python3
###################################################################################################
#
#  Project:  Embedded Learning Library (ELL)
#  File:     make_training_list.py
#  Authors:  Chris Lovett
#
#  Requires: Python 3.x
#
###################################################################################################

import argparse
import os

import numpy as np


def is_background_noise(filename):
    return "_background_noise_" in filename


def load_list_file(filename):
    with open(filename, "r") as fp:
        return [e.strip() for e in fp.readlines()]


def save_list_file(data, filename):
    with open(filename, "w") as f:
        for line in data:
            f.write(line)
            f.write('\n')


def clean_list_file(filename, bad_list):
    if not os.path.isfile(filename):
        print("Cannot find file: {}".format(filename))
        return

    testlist = load_list_file(filename)

    count = 0
    cleanlist = []
    for line in testlist:
        if line in bad_list:
            count += 1
        else:
            cleanlist += [line]

    if count > 0:
        print("removed {} bad files from {}".format(count, filename))
        save_list_file(cleanlist, filename)


def shuffle(filename, output):

    if not os.path.isfile(filename):
        print("Cannot find file: {}".format(filename))
        return

    testlist = load_list_file(filename)

    shuffled = np.random.choice(testlist, len(testlist), replace=False)
    save_list_file(shuffled, output)


def make_training_list(wav_files, max_files_per_directory, bad_list=None):
    """
    Create a training list file given the directory where the wav files are organized into subdirectories,
    with one subdirectory per keyword to be recognized.  This training list will exclude any files
    already referenced by the 'testing_list.txt' or 'validation_list.txt'. It will also shuffle the
    training list, and the testing_list and validation_list to ensure training and testing is fair.
    """
    if not os.path.isdir(wav_files):
        print("wav_file directory not found")
        return

    bad = []
    if bad_list:
        if not os.path.isfile(bad_list):
            bad_list = os.path.join(wav_files, bad_list)
            if not os.path.isfile(bad_list):
                print("Bad list {} not found".format(bad_list))
            else:
                bad = load_list_file(bad_list)

    testing_list = os.path.join(wav_files, "testing_list.txt")
    if not os.path.isfile(testing_list):
        print("### error: testing list '{}' file not found".format(testing_list))
        return

    validation_list = os.path.join(wav_files, "validation_list.txt")
    if not os.path.isfile(validation_list):
        print("### error: validation list '{}' file not found".format(validation_list))
        return

    if len(bad) > 0:
        print("Cleaning: {}".format(testing_list))
        clean_list_file(testing_list, bad)
        print("Cleaning: {}".format(validation_list))
        clean_list_file(validation_list, bad)

    print("Shuffling: {}".format(testing_list))
    shuffle(testing_list, testing_list)

    print("Shuffling: {}".format(validation_list))
    shuffle(validation_list, validation_list)

    ignore_list = load_list_file(testing_list)
    ignore_list += load_list_file(os.path.join(wav_files, "validation_list.txt"))

    list_file_name = os.path.join(wav_files, "training_list.txt")
    keywords = []
    for f in os.listdir(wav_files):
        if is_background_noise(f):  # skip the background noise folder.
            continue
        elif os.path.isdir(os.path.join(wav_files, f)):
            keywords += [f]
    keywords.sort()

    skipped = 0
    count = 0
    with open(list_file_name, "w") as f:
        for dir_name in keywords:
            files = os.listdir(os.path.join(wav_files, dir_name))
            file_list = []
            for n in files:
                if os.path.splitext(n)[1] == ".wav":
                    entry = dir_name + "/" + n
                    if entry in bad:
                        continue
                    if entry in ignore_list:
                        skipped += 1
                    else:
                        file_list += [entry]
                        count += 1

            max = max_files_per_directory
            if len(file_list) < max_files_per_directory:
                max = len(file_list)

            # shuffle the training list also
            file_list = np.random.choice(np.array(file_list), max, replace=False)
            print("choosing {} random files from {}".format(len(file_list), dir_name))

            for e in file_list:
                f.write(e + "\n")

    # write the categories file listing the keywords found.
    categories_file = os.path.join(wav_files, "categories.txt")
    with open(categories_file, "w") as f:
        for n in keywords:
            f.write(n + "\n")

    print("Created {}".format(categories_file))
    print("Created {} containing {} wav files".format(list_file_name, count))


if __name__ == "__main__":
    arg_parser = argparse.ArgumentParser(description="Create training_list.txt \
that includes a randomly selected set of wav files from the given directory tree \
up to the given maximum number of files per directory.")

    # options
    arg_parser.add_argument("--max_files_per_directory", "-max",
                            help="Maximum number of files to include from each subdirectory (default: 5000)",
                            type=int, default=5000)
    arg_parser.add_argument("--wav_files", "-w", help="Directory containing the wav files to process", required=True)
    arg_parser.add_argument("--bad_list", "-b",
                            help="List of bad files to exclude from testing, training and validation lists")
    args = arg_parser.parse_args()

    make_training_list(args.wav_files, args.max_files_per_directory, args.bad_list)
