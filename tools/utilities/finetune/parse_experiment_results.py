#!/usr/bin/env python3
####################################################################################################
#
#  Project:  Embedded Learning Library (ELL)
#  File:     parse_experiment_results.py
#  Authors:  Chuck Jacobs
#
#  Requires: Python 3.x
#
####################################################################################################

import argparse
import ast
from collections import defaultdict
import sys

SORTED_ROW_KEYS = ["Original_Train", "FineTuned_Train", "Original_Test", "FineTuned_Test"]
COLUMN_SORT_KEYS = [("TrainingExamples", False), ("L2Regularization", True), ("Regularization", True)]
BLOCK_HEADERS = {"TrainingExamples": "Parameters", "Original_Train": "ModelAccuracy", "LoadModelTime": "Timing",
                 "Total_NumWeights": "Sparsity"}
BLOCK_FOOTERS = {"Total_Sparsity": ""}

BAD_KEYS = ["RandomSeed", ]
BAD_KEY_SUFFIXES = ["PrimalObjective", "Mean", "Variance", "StdDev"]


def get_raw_experiment_data(filename):
    lines = []

    if filename:
        with open(filename) as fp:
            lines = fp.readlines()
    else:
        lines = sys.stdin.readlines()
    return lines


def get_out_file(filename):
    if filename:
        print("Using", filename)
        return open(filename, "w")
    else:
        return sys.stdout


def add_rest(x, iter):
    try:
        while True:
            x.append(next(iter))
    except StopIteration:
        return


def merge_lists(lists):
    result = lists[0]
    for x in lists[1:]:
        temp = result
        result = []
        avals = set(temp)
        bvals = set(x)

        iter1 = iter(temp)
        iter2 = iter(x)
        try:
            while True:
                a = next(iter1)
                b = next(iter2)
                while a != b:
                    if a not in bvals:
                        result.append(a)
                        a = next(iter1)
                    if b not in avals:
                        result.append(b)
                        b = next(iter2)
                result.append(a)

        except StopIteration:
            add_rest(result, iter1)
            add_rest(result, iter2)
    return result


def print_experiment_output(test_results_filenames, output_filename, format="tsv"):

    # Read data and collect set of unique "key" names
    results = []
    for test_results_filename in test_results_filenames:
        items, ordered_keys = read_experiment_output(test_results_filename)
        results += [(items, ordered_keys)]

    ordered_keys = merge_lists([x[1] for x in results])
    ordered_keys += ["Total_NumWeights", "Total_NumZeros", "Total_Sparsity"]
    key_fn = sort_rows_key_fn(ordered_keys.copy())
    ordered_keys.sort(key=key_fn)
    ordered_keys.sort(key=lambda x: 1 if is_detail(x) else 0)

    output_dicts = []
    for items, _ in results:
        # Create dictionary of output values
        totalweights = 0
        totalzeros = 0
        output_dict = defaultdict(list)
        for item in items:
            for key in ordered_keys:
                if key not in item:
                    output_dict[key] += ['""']
                else:
                    val = item[key]
                    output_dict[key] += [val]
                    if key.endswith("_NumWeights"):
                        totalweights += int(val)
                    if key.endswith("_NumZeros"):
                        totalzeros += int(val)

        # Now add computed statistics
        output_dict["Total_NumWeights"] = [str(totalweights)]
        output_dict["Total_NumZeros"] = [str(totalzeros)]
        output_dict["Total_Sparsity"] = [str(totalzeros / totalweights)]
        output_dicts.append(output_dict)

    print_items(output_dicts, ordered_keys, output_filename, format)


def print_items(output_dicts, ordered_keys, output_filename, format):
    with get_out_file(output_filename) as out_file:
        # Write out items
        prev_was_block_header = False
        for key in ordered_keys:
            if is_block_header(key) and not prev_was_block_header:
                write_block_header(out_file, format, key)
                prev_was_block_header = True
            else:
                prev_was_block_header = False
            row = [key]
            for output_dict in output_dicts:
                values = output_dict[key]
                permutation = get_column_permutation_vector(output_dict)
                values = [values[permutation[i]] for i in range(len(values))]
                row += values
            write_row(out_file, format, row)
            if is_block_footer(key):
                write_block_footer(out_file, format, key)


def read_experiment_output(test_results_filename):
    lines = get_raw_experiment_data(test_results_filename)

    lines = [x.strip() for x in lines]
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


def skip_item(key):
    if key in BAD_KEYS:
        return True
    for suffix in BAD_KEY_SUFFIXES:
        if key.endswith(suffix):
            return True
    return False


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


def is_block_footer(key):
    if key in BLOCK_FOOTERS:
        return True
    return False


def write_block_footer(out, format, key):
    write_row(out, format, "")


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
        out.write(delimiter.join(row) + "\n")
    end_row(out, format)


if __name__ == "__main__":
    parser = argparse.ArgumentParser("Parse fine-tuning test output")
    parser.add_argument("test_results_files", nargs="+",
                        help="File with the (textual, not json) experiment test output (if omitted, read from stdin)")
    parser.add_argument("--out", help="output file (if omitted, write to stdout)")
    parser.add_argument("--format", help="output format", default="tsv", choices=["csv", "tsv", "xlsx"])
    args = parser.parse_args()
    print_experiment_output(args.test_results_files, args.out, args.format)
