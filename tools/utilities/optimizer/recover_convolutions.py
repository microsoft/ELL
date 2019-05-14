#!/usr/bin/env python3

####################################################################################################
#
#  Project:  Embedded Learning Library (ELL)
#  File:     recover_convolutions.py
#  Authors:  Chuck Jacobs, Mason Remy
#
#  Requires: Python 3.x
#
####################################################################################################

import argparse
import sys

# Recover convolution operations from fully-refined profiler output

# Example profile output strings that are parsed:
#   Unrolled:
#
#   Node[5209]:	ReceptiveFieldMatrixNode<float>                                	time: 547.28833 ms	count: 100
#   Node[5210]:	MatrixMatrixMultiplyNode<float>                                	time: 4057.05273 ms	count: 100
#
#   Node[5216]:	UnrolledConvolutionNode<float>                                 	time: 758.23413 ms	count: 100

#   Simple:
#
#   Node[5038]:	SimpleConvolutionComputeNode<float>                            	time: 39697.14917 ms	count: 100

#   Winograd:
#
#   Node[5192]:	WinogradConvolutionComputeNode<float>                          	time: 1083.53955 ms	count: 100

#   profile_text is a list of strings: one per line of the profile output

# parses both `make_profiler.sh`- and `test_models.py`-format profile output
def get_time(line):
    parts = line.split()
    if len(parts) == 7:
        return float(parts[3])
    elif len(parts) > 7 and parts[1].strip() == "type:":
        return float(parts[4])


def get_method(line):
    if "UnrolledConvolutionNode" in line:
        return "unrolled"
    elif "SimpleConvolutionComputeNode" in line:
        return "simple" 
    elif "WinogradConvolutionComputeNode" in line:
        return "winograd"
    return "Unknown"


def match_convolution(it):
    line = next(it)
    if line.startswith("Running ELL profiler tool"):
        return None, None
    if line.startswith("Node["):
        if "ReceptiveFieldMatrixNode" in line:
            # Special case: convolution is split into 2 nodes
            t1 = get_time(line)
            line2 = next(it)
            assert("MatrixMatrixMultiplyNode" in line2)
            t2 = get_time(line2)
            return t1 + t2, "unrolled"
        elif "UnrolledConvolutionNode" in line or "SimpleConvolutionComputeNode" in line or "WinogradConvolutionComputeNode" in line:
            return get_time(line), get_method(line)
    return None, None


def get_convolution_times(profile_text=[]):
    convolution_times = []

    it = iter(profile_text)
    try:
        while True:
            t, m = match_convolution(it)
            if t is not None:
                convolution_times.append((t, m))
    except StopIteration:
        pass
    return convolution_times


if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument("profile_file", help="file with profile results")
    parser.add_argument("--notimes", help="omit time info", action="store_true")
    args = parser.parse_args()
    lines = open(args.profile_file).readlines()
    times = get_convolution_times(lines)

    for t in times:
        if args.notimes:
            print(t[1])
        else:
            print("{}\t{}".format(t[1], t[0]))
