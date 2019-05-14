#!/usr/bin/env python3

####################################################################################################
#
#  Project:  Embedded Learning Library (ELL)
#  File:     profile_optimizer.py
#  Authors:  Mason Remy
#
#  Requires: Python 3.x
#
####################################################################################################

import argparse
import itertools
import json
import re
import shutil
import subprocess
import os

import find_ell
import buildtools
import optimizer_util

import recover_convolutions
import platform
import logger


def parse_id(id):
    return id[4:-1]


def remove_winograd_options(node_options):
    temp_winograd_config = optimizer_util.WinogradConfiguration(1, 1, 1)
    winograd_option_map = temp_winograd_config.to_option_map()
    for opt in winograd_option_map:
        if opt in node_options:
            del node_options[opt]
    return node_options


def get_options(best_convolutions_info, profile_options):
    result = best_convolutions_info[2]
    if profile_options.parallel_option is optimizer_util.ParallelOptions.Parallel:
        result["parallelize"] = "true"
    else:
        result["parallelize"] = "false"
    if profile_options.convolution_method is optimizer_util.ConvolutionMethod.Winograd:
        result.update(profile_options.winograd_configuration.to_option_map())
    return result


def process_model_results(model_path, output_dir, profile_options_and_files, ell_root, ell_build_root, target):
    global_logger = logger.get()

    model_name = os.path.splitext(os.path.basename(model_path))[0]
    global_logger.info("Generating optimized model file for model {}".format(model_name))

    os.makedirs(output_dir, exist_ok=True)
    platform_bin_dir = os.path.join(ell_build_root, "bin")
    if platform.system() is "Windows":
        platform_bin_dir = os.path.join(platform_bin_dir, "Release")

    print_exe = os.path.join(platform_bin_dir, "print")
    p = subprocess.run([str(print_exe), str(model_path), "--nodeDetails", "false", "--includeNodeId"], universal_newlines=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    convolution_nodes = p.stdout.splitlines()
    conv_ids = [parse_id(l.split()[0]) for l in convolution_nodes if "Convolution" in l]
    experiment_files = {}  # path->results map
    experiment_options = {} # path->options map

    for profile_options, profile_files in profile_options_and_files:
        for profile_file in profile_files:
            with open(profile_file) as f:
                lines = f.readlines()
                convolution_times = recover_convolutions.get_convolution_times(lines)
                experiment_profile_times = [(x[0], {"preferredConvolutionMethod": x[1]}) for x in convolution_times]
                if len(experiment_profile_times) > 0:
                    experiment_files[profile_file] = experiment_profile_times
                    experiment_options[profile_file] = profile_options

    if len(experiment_files) == 0:
        global_logger.error("No profiles read for model {}".format(model_name))
        return

    # Initial "best" is all the layers from the first experiment
    k = list(experiment_files.keys())[0]
    best_convolutions = [(k, times[0], times[1]) for times in experiment_files[k]]
    for k, v in experiment_files.items():
        for i in range(len(best_convolutions)):
            if v[i][0] < best_convolutions[i][1]:
                best_convolutions[i] = (k,) + v[i]

    # Get dict of node options
    all_option_names = set()
    all_node_options = {}
    for i in range(len(best_convolutions)):
        node_id = conv_ids[i]
        profile_file = best_convolutions[i][0]
        node_options = get_options(best_convolutions[i], experiment_options[profile_file])
        if node_options["preferredConvolutionMethod"] is not optimizer_util.ConvolutionMethod.Winograd.value:
            # Some unrolled convolutions are used alongside winograd convolutions, but still come from
            # winograd convolution profile data. To reduce node options bloat, remove the winograd
            # parameters from the unrolled nodes' node options.
            # Note: With too many unnecessary node options a maximum command-line length
            # issue can be hit on windows while building the optimized models. While this error is still
            # possible with a large enough model, this quick fix avoids that issue for smaller models
            node_options = remove_winograd_options(node_options)
        all_node_options[node_id] = node_options
        for k in node_options.keys():
            all_option_names.add(k)

    with open(os.path.join(output_dir, 'compile_options.json'), 'w') as outfile:
        json.dump({'nodes' : all_node_options}, outfile, indent=2)

    # Generate command-line strings for compile tool
    option_strings = []
    for node_id in all_node_options:
        node_options = all_node_options[node_id]
        for option in node_options:
            option_strings += ["--nodeOption", "{},{},{}".format(node_id, option, node_options[option])]

    args = ["-imap", str(model_path), "--outputDirectory", str(output_dir), "--target", target, "--mapWithOptions"] + option_strings

    compile_switches_file = os.path.join(output_dir, "compile_switches.txt")
    with open(compile_switches_file, "w") as f:
        f.write(" ".join([str(s) for s in args]))
    compile_exe = os.path.join(platform_bin_dir, "compile")
    subprocess.run([str(compile_exe)] + args)
