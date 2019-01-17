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


def load_list_file(filename):
    with open(filename, "r") as fp:
        return [e.strip() for e in fp.readlines()]


def make_training_list(wav_files, max_files_per_directory):
    """
    Create a training list file given the directory where the wav files are organized into subdirectories,
    with one subdirectory per keyword to be recognized.  This training list will exclude any files
    already referenced by the 'testing_list.txt' or 'validation_list.txt'
    """
    if not os.path.isdir(wav_files):
        print("wav_file directory not found")
        return

    ignore_list = load_list_file(os.path.join(wav_files, "testing_list.txt"))
    ignore_list += load_list_file(os.path.join(wav_files, "validation_list.txt"))

    list_file_name = os.path.join(wav_files, "training_list.txt")
    keywords = []
    for f in os.listdir(wav_files):
        if os.path.isdir(os.path.join(wav_files, f)):
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
                    if entry in ignore_list:
                        skipped += 1
                    else:
                        file_list += [entry]
                        file_list += [entry]
                        count += 1

            if len(file_list) > max_files_per_directory:
                file_list = np.random.choice(np.array(file_list), max_files_per_directory)
                print("choosing {} random files from {}".format(len(file_list), dir_name))
            else:
                print("found {} files in {}".format(len(file_list), dir_name))
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
    args = arg_parser.parse_args()

    make_training_list(args.wav_files, args.max_files_per_directory)
