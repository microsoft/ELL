#!/usr/bin/env python3
####################################################################################################
#
#  Project:  Embedded Learning Library (ELL)
#  File:     timing_aggregator.py
#  Authors:  Mason Remy
#
#  Requires: Python 3.x
#
####################################################################################################

import re
import argparse
import operator

class TimingResult:
    key = ""
    count = 0
    result_values = []
    average = 0
    result_range = ()

    def __init__(self, key="", raw_results=[]):
        self.key = key
        self.result_values = raw_results
        self.count = len(raw_results)
        if self.count > 0:
            self.average = sum(raw_results) / self.count
            self.result_range = (min(raw_results), max(raw_results))

    def print_summary(self, indenting=0):
        indent = "\t" * indenting
        print("{}{}".format(indent, self.key))
        print("{}\tAvg   = {}".format(indent, self.average))
        print("{}\tRange = {}".format(indent, self.result_range))
        print("{}\tCount = {}".format(indent, self.count))

    def print_raw_results(self, indenting=0):
        indent = "\t" * indenting
        print("{}{} times:".format(indent, self.key))
        for result in self.result_values:
            print("{}\t{}".format(indent, result))

def parse_output(output_lines):
    timing_pattern = "(gemm.*) time = (.*)"
    timing_regex_matcher = re.compile(timing_pattern)
    time_dict = {}
    for line in output_lines:
        match = timing_regex_matcher.match(line)
        if match:
            key = match.group(1)
            value = float(match.group(2))
            if key in time_dict:
                time_dict[key].append(value)
            else:
                time_dict[key] = [value]
    results = {}
    for key in time_dict:
        results[key] = TimingResult(key, time_dict[key])
    return results

def get_ratios_against_best(timing_results_dict):
    best_time = None
    for key in timing_results_dict:
        if best_time == None or best_time > timing_results_dict[key].average:
            best_time = timing_results_dict[key].average
    ratios_dict = {}
    for key in timing_results_dict:
        ratios_dict[key] = timing_results_dict[key].average / best_time
    ordered_key_ratio_pairs = sorted(ratios_dict.items(), key=operator.itemgetter(1))
    return ordered_key_ratio_pairs

def print_results(results_dict, include_raw_results=True, include_statistics=True, include_ratios=True):
    for key in results_dict:
        if include_raw_results:
            results_dict[key].print_raw_results()
            print()
        if include_statistics:
            results_dict[key].print_summary()
            print()
    if include_ratios:
        ratios_list = get_ratios_against_best(results_dict)
        print("Ratios:")
        for (key, ratio) in ratios_list:
            print("{} : {}".format(key, ratio))

if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("-f", "--file", required=True)
    args = parser.parse_args()

    with open(args.file, 'r') as f:
        lines = f.readlines()
    results_dict = parse_output(lines)
    print_results(results_dict)
