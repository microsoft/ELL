###############################################################################
#
#  Project:  Embedded Learning Library (ELL)
#  File:     convert_dataset.py
#  Authors:  Chris Lovett
#  Requires: Python 3.x
#
###############################################################################

import argparse
import os
import sys
import numpy as np
from cntk.io import MinibatchSource, CTFDeserializer, StreamDef, StreamDefs, \
                    INFINITELY_REPEAT


def check_path(path):
    """Check that the given path exists"""
    if not os.path.exists(path):
        readme_file = os.path.normpath(os.path.join(
            os.path.dirname(path), "..", "README.md"))
        raise RuntimeError(
            "File '%s' does not exist. " +
            "Please follow the instructions at %s to download and prepare it."
            % (path, readme_file))


def create_reader(path, is_training, input_dim, label_dim):
    """Create MinibatchSource for reaching training data from given file"""
    return MinibatchSource(CTFDeserializer(path, StreamDefs(
        features=StreamDef(field='features', shape=input_dim, is_sparse=False),
        labels=StreamDef(field='labels', shape=label_dim, is_sparse=False)
    )), randomize=is_training, 
        max_sweeps=INFINITELY_REPEAT if is_training else 1)


def convert_file(filename):
    """ Converts the given data file into the ELL dataset file format """
    num_features = 784
    num_classes = 10

    basename, ext = os.path.splitext(os.path.basename(filename))
    with open(basename + ".ds", "w") as output:

        path = os.path.normpath(os.path.abspath(filename))
        check_path(path)

        reader = create_reader(path, False, num_features, num_classes)

        batch = reader.get_next_minibatch(1)
        while batch:
            labels = batch[reader.streams.labels].asarray()[0][0]
            features = batch[reader.streams.features].asarray()[0][0]

            index = np.where(labels == 1)[0][0]
            output.write("%d " % (index))
            for i, x in enumerate(features):
                if x != 0:
                    output.write("%d:%f " % (i, x/255.0))
            output.write("\n")

            batch = reader.get_next_minibatch(1)


if __name__ == '__main__':
    # Specify the target device to be used for computing, if you do not want to
    # use the best available one, e.g.
    # try_set_default_device(cpu())
    args = sys.argv[1:]
    parser = argparse.ArgumentParser()
    parser.add_argument('datafile',
      help='The file to convert from CTF format to ELL Sparse DataSet format')
    args = parser.parse_args(args)

    convert_file(args.datafile)
