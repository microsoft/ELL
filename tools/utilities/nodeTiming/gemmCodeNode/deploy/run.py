#!/usr/bin/env python3
####################################################################################################
#
#  Project:  Embedded Learning Library (ELL)
#  File:     run.py
#  Authors:  Mason Remy
#
#  Requires: Python 3.x
#
####################################################################################################

import argparse
import subprocess
import platform
import os
import timing_aggregator

script_path = os.path.dirname(os.path.realpath(__file__))

def run():
    platform_run_script = "run_all.sh"
    if platform.system() == "Windows":
        platform_run_script = "run_all.cmd"
    run_full_path = os.path.join(script_path, platform_run_script)
    results = subprocess.run([run_full_path], stdout=subprocess.PIPE)
    split_lines = results.stdout.decode("utf-8").split(os.linesep)
    return split_lines

if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("-c", "--count", default=1, type=int)
    args = parser.parse_args()

    accumulated_result_lines = []
    print("Running {} time(s)...".format(args.count))
    for i in range(args.count):
        accumulated_result_lines.extend(run())
        print("{}/{} complete".format(i + 1, args.count))
    
    results_dict = timing_aggregator.parse_output(accumulated_result_lines)
    timing_aggregator.print_results(results_dict)