#!/usr/bin/env python3

####################################################################################################
#
#  Project:  Embedded Learning Library (ELL)
#  File:     profile_runner.py
#  Authors:  Mason Remy
#
#  Requires: Python 3.x
#
####################################################################################################

import sys
import argparse
import os
import shutil
from dask import compute, delayed
import dask.multiprocessing

import optimizer_util
import remoterunner
import picluster

import logger

global_logger = logger.setup()

remote_working_directory = "/home/pi/profilers"
default_platform_regex = {
    "pi3": "ARMv7 Processor rev 4 \(v7l\) \(4 cores\) BCM2835",
    "pi0": "ARMv6-compatible processor rev 7 \(v6l\) \(1 cores\) BCM2835"
}


class RemoteProfileRunner:
    def __init__(self, profiler_data, cluster, fallback_ip_address, username, password, output_path, platform_re=None, ignore_extensions=[".ll", ".bc"], logging_args=None):
        self.profiler_data = profiler_data
        self.cluster = cluster
        self.fallback_ip_address = fallback_ip_address
        self.username = username
        self.password = password
        self.output_path = output_path
        os.makedirs(self.output_path, exist_ok=True)
        self.platform_re = platform_re
        self.machine = None
        self.ipaddress = None
        self.ignore_extensions = ignore_extensions
        self.logging_args = logging_args

    def run(self):
        copyback_files = ["profile_report_noopt.txt", "profile_report_opt.txt"]
        if self.cluster:
            self.machine = self.cluster.wait_for_free_machine(self.profiler_data.friendly_name, rePlatform=self.platform_re)
            self.ipaddress = self.machine.ip_address
        else:
            self.ipaddress = self.fallback_ip_address
        # Copy over all of the files in self.profiler_data.local_working_dir except for the llvm IR files
        # The IR files tend to be large and are not needed on the target device
        source_dir_files = os.listdir(self.profiler_data.built_profilers_path)
        ignore_files = [filename for filename in source_dir_files if True in [filename.endswith(ext) for ext in self.ignore_extensions]]
        ignore_files.extend(copyback_files)
        copy_source_files = [os.path.join(self.profiler_data.built_profilers_path, filename) for filename in source_dir_files if filename not in ignore_files]

        runner = remoterunner.RemoteRunner(ipaddress=self.ipaddress,
                                           username=self.username,
                                           password=self.password,
                                           source_files=copy_source_files,
                                           target_dir=remote_working_directory,
                                           copyback_files=copyback_files,
                                           copyback_dir=self.profiler_data.built_profilers_path,
                                           command=self.profiler_data.command,
                                           start_clean=True,
                                           cleanup=True,
                                           logfile=self.logging_args)
        try:
            runner.run_command()
            if self.cluster:
                self.cluster.unlock(self.ipaddress)
            profile_files = []
            for copyback_file in copyback_files:
                src_path = os.path.join(self.profiler_data.built_profilers_path, copyback_file)
                dst_name = "{}_{}_{}".format(self.profiler_data.model_name, self.profiler_data.profile_options.to_short_string(), copyback_file)
                dst_path = os.path.join(self.output_path, dst_name)
                shutil.copy(src_path, dst_path)
                profile_files.append(dst_path)
            return_val = (self.profiler_data, profile_files)
            return return_val
        except:
            errorType, value, traceback = sys.exc_info()
            global_logger.error("### Caught exception while running remote command: {}: {}\n{}".format(errorType, value, traceback))


def run_profilers_with_profiler_data(all_profiler_data, output_path, cluster_address, ipaddress, api_key, password, username="pi", target="pi3", platform_regex=None, parallel_run=24, logging_args=None):
    if not platform_regex:
        if target in default_platform_regex:
            platform_regex = default_platform_regex[target]
        if not platform_regex:
            global_logger.error("Platform regex not specified and no default platform regex for {} exists in {}.".format(target, default_platform_regex))
            return

    if len(all_profiler_data) < parallel_run:
        parallel_run = len(all_profiler_data)

    cluster = None
    if cluster_address:
        cluster = picluster.PiBoardTable(cluster_address, api_key)
    else:
        global_logger.info("Cluster not specified, using device with ip {} for testing and setting parallel run count to 1".format(ipaddress))
        parallel_run = 1

    global_logger.info("Running {} profiles".format(len(all_profiler_data)))
    runners = [RemoteProfileRunner(profiler_data,
                                    cluster,
                                    ipaddress,
                                    username,
                                    password,
                                    os.path.join(output_path, profiler_data.model_tag),
                                    platform_regex,
                                    logging_args=logging_args)
                                    for profiler_data in all_profiler_data]
    values = [delayed(runner.run)() for runner in runners]
    results = compute(*values, scheduler="threads", num_workers=parallel_run)
    global_logger.info("Done running profiling")
    if None in results:
        global_logger.error("Run failures detected, results = {}".format(results))
    tag_to_profile_options_and_files_dict = {}
    for result in results:
        if result:
            profiler_data = result[0]
            profiler_files = result[1]
            tag = profiler_data.model_tag
            profile_options = profiler_data.profile_options
            if tag not in tag_to_profile_options_and_files_dict:
                tag_to_profile_options_and_files_dict[tag] = []

            tag_to_profile_options_and_files_dict[tag].append((profile_options, profiler_files))
    return tag_to_profile_options_and_files_dict


def run_profilers(input_path, model_path, models, output_path, all_profile_options, cluster, ipaddress, api_key, password, username="pi", target="pi3", platform_regex=None, parallel_run=24, logging_args=None):
    os.makedirs(output_path, exist_ok=True)
    all_profiler_data = optimizer_util.make_profiler_data(model_path, input_path, all_profile_options, models)
    run_profilers_with_profiler_data(all_profiler_data, output_path, target, platform_regex, parallel_run, logging_args)


def add_profile_runner_args(arg_parser):
    arg_parser.add_argument("--platform_regex", help="Regex to match test platform name on. Defaults = {}".format(default_platform_regex))
    arg_parser.add_argument("--parallel_run", help="The maximum number of profile operations to run in parallel. Also the maximum number of devices to attempt to claim.", default=24, type=int)
    arg_parser.add_argument("--cluster", help="http address of the cluster server that controls access to the target devices", default=os.getenv("RPI_CLUSTER"))
    arg_parser.add_argument("--ipaddress", help="The address of the target device if you don't have a cluster")
    arg_parser.add_argument("--apikey", help="The ApiKey to use for the cluster", default=os.getenv("RPI_APIKEY"))
    arg_parser.add_argument("--username", help="The username for pi machine", default="pi")
    arg_parser.add_argument("--password", help="The password for pi machines", default=os.getenv("RPI_PASSWORD"))


if __name__ == "__main__":
    parser = argparse.ArgumentParser("Run built profilers on target devices")
    parser.add_argument("--input_path", "-i", help="Path to built profilers", required=True)
    parser.add_argument("--output_path", "-o", help="Path to store profiling results in", required=True)
    logger.add_logging_args(parser)
    add_profile_runner_args(parser)
    optimizer_util.add_all_shared_args(parser)
    args = parser.parse_args()

    all_profile_options = optimizer_util.ProfileOptions.parse_option_list_from_file(args.options)

    run_profilers(input_path=args.input_path,
                  model_path=args.model_path,
                  models=args.models,
                  output_path=args.output_path,
                  all_profile_options=all_profile_options,
                  cluster=args.cluster,
                  ipaddress=args.ipaddress,
                  api_key=args.apikey,
                  password=args.password,
                  username=args.username,
                  target=args.target,
                  platform_regex=args.platform_regex,
                  parallel_run=args.parallel_run,
                  logging_args=args)
