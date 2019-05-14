#!/usr/bin/env python3

####################################################################################################
#
#  Project:  Embedded Learning Library (ELL)
#  File:     profile_and_optimize.py
#  Authors:  Mason Remy
#
#  Requires: Python 3.x
#
####################################################################################################

import argparse
import os
import shutil
import sys

import logger
import optimizer_util
import profile_builder
import profile_runner
import profile_optimizer

script_path = os.path.dirname(os.path.abspath(__file__))
sys.path += [os.path.join(script_path, '..', 'pythonlibs')]
import find_ell

default_profile_and_optimize_working_dir = "__profile_and_optimize_tmp"


def partition_list(input_list, max_partition_size):
    partitions = []
    for i in range(0, len(input_list), max_partition_size):
        partitions.append(input_list[i : i + max_partition_size])
    return partitions


def profile_and_optimize_models_segmented(segment_size, model_path, temp_dir, output_path, ell_root, ell_build_root, all_profile_options, models, target, platform_regex, parallel_build_count, parallel_run_count, include_exercise_models, logging_args):
    # partition the models by segment_size
    model_partitions = partition_list(models, segment_size)
    for model_partition in model_partitions:
        profile_and_optimize_models(model_path, temp_dir, output_path, ell_root, ell_build_root, all_profile_options, model_partition, target, platform_regex, parallel_build_count, parallel_run_count, include_exercise_models, logging_args)


def profile_and_optimize_models(model_path, temp_dir, output_path, ell_root, ell_build_root, all_profile_options, models, target, platform_regex, parallel_build_count, parallel_run_count, include_exercise_models, logging_args):
    os.makedirs(temp_dir, exist_ok=True)
    os.makedirs(output_path, exist_ok=True)
    all_profiler_data = profile_builder.build_model_profilers(model_path=model_path,
                                                              output_path=temp_dir,
                                                              ell_root=ell_root,
                                                              ell_build_root=ell_build_root,
                                                              all_profile_options=all_profile_options,
                                                              models=models,
                                                              target=target,
                                                              parallel_count=parallel_build_count,
                                                              include_exercise_models=include_exercise_models)

    tag_to_profile_options_and_files_dict = profile_runner.run_profilers_with_profiler_data(all_profiler_data=all_profiler_data,
                                                                                            output_path=temp_dir,
                                                                                            cluster_address=args.cluster,
                                                                                            ipaddress=args.ipaddress,
                                                                                            api_key=args.apikey,
                                                                                            password=args.password,
                                                                                            username=args.username,
                                                                                            target=target,
                                                                                            platform_regex=platform_regex,
                                                                                            parallel_run=parallel_run_count,
                                                                                            logging_args=args)

    merged_model_paths = { profiler_data.model_tag : profiler_data.model_path for profiler_data in all_profiler_data }
    for model_tag in merged_model_paths:
        profile_optimizer.process_model_results(model_path=merged_model_paths[model_tag],
                                                output_dir=os.path.join(output_path, model_tag),
                                                profile_options_and_files=tag_to_profile_options_and_files_dict[model_tag],
                                                ell_root=ell_root,
                                                ell_build_root=ell_build_root,
                                                target=target)
    shutil.rmtree(temp_dir)


if __name__ == "__main__":
    parser = argparse.ArgumentParser("Profile and optimize models for a given target")
    optimizer_util.add_all_shared_args(parser)
    parser.add_argument("--output_path", "-o", help="Path to store optimized models files in", required=True)
    parser.add_argument("--temp_dir", help="Directory to store temporary files in", default=os.path.join(".", default_profile_and_optimize_working_dir))
    parser.add_argument("--concurrent_models", help="How many models to optimize concurrently. Concurrently optimizing models will take more space on the local file system. Builds and runs will still be run parallel regardless of this setting.", default=1, type=int)
    profile_builder.add_profile_builder_args(parser)
    profile_runner.add_profile_runner_args(parser)
    find_ell.add_ell_root_args(parser)
    logger.add_logging_args(parser)
    args = parser.parse_args()

    if not args.ell_root:
        raise Exception("ELL root not found")
    if not args.ell_build_root:
        raise Exception("ELL build root not found")

    all_profile_options = optimizer_util.ProfileOptions.parse_option_list_from_file(args.options)

    models = args.models
    if models is None:
        models = set()
        ell_models = optimizer_util.find_ell_models(args.model_path)
        for model_tag in ell_models:
            models.add(ell_models[model_tag][0])
        models = list(models)

    profile_and_optimize_models_segmented(segment_size=args.concurrent_models,
                                          model_path=args.model_path,
                                          temp_dir=args.temp_dir,
                                          output_path=args.output_path,
                                          ell_root=args.ell_root,
                                          ell_build_root=args.ell_build_root,
                                          all_profile_options=all_profile_options,
                                          models=models,
                                          target=args.target,
                                          platform_regex=args.platform_regex,
                                          parallel_build_count=args.parallel_build,
                                          parallel_run_count=args.parallel_run,
                                          include_exercise_models=args.include_exercise_models,
                                          logging_args=args)
