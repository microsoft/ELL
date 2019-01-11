#!/usr/bin/env python3
####################################################################################################
##
##  Project:  Embedded Learning Library (ELL)
##  File:     parse_experiment_results.py
##  Authors:  Chuck Jacobs
##
##  Requires: Python 3.x
##
####################################################################################################

import argparse
import ast
import sys

SORTED_ROW_KEYS = ["Original_Train", "FineTuned_Train", "Original_Test", "FineTuned_Test"]
COLUMN_SORT_KEYS = [("TrainingExamples", False), ("L2Regularization", True), ("Regularization", True)]
BLOCK_HEADERS = {"TrainingExamples":"Parameters", "Original_Train":"ModelAccuracy", "LoadModelTime":"Timing"}

def get_raw_experiment_data(filename):
    lines = []

    if filename:
        with open(test_results_filename) as fp:
            lines = fp.readlines()
    else:
        lines = sys.stdin.readlines()
    return lines

def get_out_file(filename):
    if filename:
        return open(filename, "w")
    else:
        return sys.stdout

def parse_experiment_output(test_results_filename, output_filename, format="tsv"):
    
    # read data and collect set of unique "key" names
    items, ordered_keys = read_experiment_output(test_results_filename)
    
    key_fn = sort_rows_key_fn(ordered_keys.copy())
    ordered_keys.sort(key=key_fn)
    ordered_keys.sort(key=lambda x: 1 if is_detail(x) else 0)

    # now write out items
    output_dict = {}
    for item in items:
        for key in ordered_keys:
            if key not in output_dict:
                output_dict[key] = []
            if key not in item:
                output_dict[key] += ['""']
            else:
                output_dict[key] += [item[key]]
    
    permutation = get_column_permutation_vector(output_dict)
    with get_out_file(output_filename) as out:
        delimiter = get_delimiter_string(format)
        prev_was_block_header = False
        for key in ordered_keys:
            if is_block_header(key) and not prev_was_block_header:
                write_block_header(out, format, key)
                prev_was_block_header = True
            else:
                prev_was_block_header = False
            values = output_dict[key]
            values = [values[permutation[i]] for i in range(len(values))]
            row = [key] + values
            write_row(out, format, row)
            
def read_experiment_output(test_results_filename):
    lines = get_raw_experiment_data(test_results_filename)
    
    lines = [l.strip() for l in lines]
    # read data and collect set of unique "key" names
    items = []
    ordered_keys = []

    known_keys = set()
    current_item = {}
    # add extra newline to make sure we add the last item
    lines += ""
    for line in lines:
        if not line:
            # reset current item
            if current_item:
                items += [current_item]
                current_item = {}
        else:
            key, value = get_key_and_value(line, known_keys)
            current_item[key] = value
            if key not in known_keys:
                known_keys.add(key)
                ordered_keys += [key]
            
    return items, ordered_keys

def get_key_and_value(line, known_keys):
    parts = line.split()
    key = parts[0].strip(":")
    value = parts[1]
    return key, value
    
def sort_rows_key_fn(keys):
    sorted_key_pos = [keys.index(k) for k in SORTED_ROW_KEYS]
    sorted_key_pos.sort()
    def key_fn(val):
        if val in SORTED_ROW_KEYS:
            return sorted_key_pos[SORTED_ROW_KEYS.index(val)]
        else:
            return keys.index(val)
    return key_fn

def get_column_permutation_vector(output_dict):
    perm = []

    for key, rev in reversed(COLUMN_SORT_KEYS):
        if key in output_dict:
            values = output_dict[key]
            if len(perm) < len(values):
                perm += range(len(perm), len(values))
            perm.sort(key=lambda x: ast.literal_eval(values[x]), reverse=rev)

    return perm

def get_delimiter_string(format):
    if format == "csv":
        return ","
    else:
        return "\t"

def is_detail(key):
    return key.startswith("FullyConnected_") or key.startswith("Convolutional_")

def is_block_header(key):
    if key in BLOCK_HEADERS:
        return True
    return False

def write_block_header(out, format, key):
    write_row(out, format, "")
    if key in BLOCK_HEADERS:
        write_row(out, format, [BLOCK_HEADERS[key]])

def start_row(out, format):
    if format == "xlsx":
        out.write("<row>\n")

def end_row(out, format):
    if format == "xlsx":
        out.write("</row>\n")

def write_row(out, format, row):
    start_row(out, format)
    if format == "xlsx":
        for item in row:
            out.write("<c><v>{}</v></c>".format(item))
    else:
        delimiter = get_delimiter_string(format)
        out.write(delimiter.join(row)+"\n")
    end_row(out, format)

if __name__ == "__main__":
    parser = argparse.ArgumentParser("Parse fine-tuning test output")
    parser.add_argument("test_results_file", help="File with the experiment test output (if omitted, read from stdin)", nargs="?")
    parser.add_argument("--out", help="output file (if omitted, write to stdout)")
    parser.add_argument("--format", help="output format", default="tsv", choices=["csv", "tsv", "xlsx"])
    args = parser.parse_args()

    parse_experiment_output(args.test_results_file, args.out, args.format)
