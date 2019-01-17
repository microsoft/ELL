#!/usr/bin/env python3
####################################################################################################
#
#  Project:  Embedded Learning Library (ELL)
#  File:     make_profiler.py
#  Authors:  Ying Guo
#
#  Requires: Python 3.x
#
####################################################################################################
import argparse
import datetime
import glob
import json
import logging
import os
import parse
import sys

__script_path = os.path.dirname(os.path.abspath(__file__))
sys.path += [os.path.join(__script_path, "..", "pitest")]
sys.path += [os.path.join(__script_path, "..", "..", "wrap")]

from drivetest import DriveTest, COMPILE_INCREMENTAL
from wrap import ModuleBuilder

LOG_LEVEL_VERBOSE = 1


def verbose(self, message, *args, **kws):
    if self.isEnabledFor(LOG_LEVEL_VERBOSE):
        self._log(LOG_LEVEL_VERBOSE, message, args, **kws)


logging.addLevelName(LOG_LEVEL_VERBOSE, "VERBOSE")
logging.Logger.verbose = verbose
logger = logging.getLogger(__name__)
logger_console_handler = logging.StreamHandler()


def show_profile(profile):
    logger.info("\nModel: {}".format(profile["model"]))
    logger.info("Profile Count: {}".format(len(profile["profile"])))
    for i, p in enumerate(profile["profile"]):
        logger.info("\nProfile {} with {} runs:\n".format(i, len(p["result"])))
        logger.info("\tconfig:")
        for arg in p["config"].keys():
            logger.info("\t\t{}: {}".format(arg, str(p["config"][arg])))
        runs = sorted(list(p["result"].keys()), reverse=True)
        for r in runs:
            logger.info("\n\t{}:".format(r))
            performance = p["result"][r]
            if performance is not None:
                logger.info("\t\t{}{}{}{}{}".format("name".ljust(20), "type".ljust(70), "average_time_ms".ljust(10),
                                                    "total_time_ms".rjust(15), "count".rjust(7)))
                if "node" in performance.keys():
                    logger.verbose("\t\t------------------------------------------------------------------------------\
-------------------------------------------------")
                    ancestors = sorted(list(performance["node"].keys()))
                    for a in ancestors:
                        ancestor = performance["node"][a]
                        logger.verbose("\n\t\t{}{}{}{}".format(
                            a.ljust(90),
                            format(ancestor["time_ms"] / ancestor["count"], ".4f").rjust(10),
                            format(ancestor["time_ms"], ".4f").rjust(15),
                            str(ancestor["count"]).rjust(10)))
                        for n in ancestor["descendents"]:
                            logger.verbose("\t\t     {}{}{}{}{}".format(
                                n["name"].ljust(15),
                                n["type"].ljust(70),
                                format(n["time_ms"] / n["count"], ".4f").rjust(10),
                                format(n["time_ms"], ".4f").rjust(15),
                                str(n["count"]).rjust(10)))
                if "model" in performance.keys():
                    logger.info("\n\t\t-------------------------------------------------------------------------------\
------------------------------------------------")
                    logger.info("\t\tModel Total: {}{}{}".format(
                        format(performance["model"]["time_ms"] / performance["model"]["count"], ".4f").rjust(87),
                        format(performance["model"]["time_ms"], ".4f").rjust(15),
                        str(performance["model"]["count"]).rjust(10)))
            else:
                logger.warning("\t\tno performance result")


def options_to_profile(option):
    profile = None
    model = None
    config = {}

    # generate default arguments
    for arg in ModuleBuilder.arguments.keys():
        if "default" in ModuleBuilder.arguments[arg].keys() and ModuleBuilder.arguments[arg]["default"] is not None:
            config[arg] = ModuleBuilder.arguments[arg]["default"]

    # override default with passed in values
    i = 0
    while i < len(option):
        if option[i] == "--":
            i += 1
        elif option[i].startswith("--"):
            if i + 1 >= len(option) or option[i + 1].startswith("--"):
                config[option[i][2:]] = True  # TODO - assuming flag without argument sets to True
                i += 1
            elif option[i] == "--model_file":
                model = option[i + 1]
                i += 2
            else:
                config[option[i][2:]] = option[i + 1]
                i += 2
        else:
            i += 1

    if model is not None:
        # add missing parameters
        if "--outdir" not in option:
            config["outdir"] = os.path.dirname(model)
        if "--profile" not in option:
            config["profile"] = True

        # convert back to argument list with default and added values
        argv = ["--model_file", model]
        for key in config.keys():
            if type(config[key]) is not bool:
                argv.append("--" + key)
                argv.append(config[key])
            elif config[key]:
                argv.append("--" + key)

        # generate profile
        profile = {}
        profile["model"] = os.path.abspath(model)
        profile["profile"] = []
        profile["profile"].append({"config": config, "args": argv, "result": {}})
    else:
        raise ValueError("--model_file and --target are required.")

    return profile


def parse_performance_log(log):
    result = {}
    for line in log:
        node = parse.parse("{}:\ttype: {}\ttime: {:f} ms\tcount: {:d}\tancestor: {}", line)
        if node:
            if "node" not in result.keys():
                result["node"] = {}

            id = "node[" + node[4] + "]"
            if id not in result["node"].keys():
                ancestor = {"time_ms": 0, "count": 0}
                result["node"][id] = ancestor
            ancestor = result["node"][id]

            if "descendents" not in ancestor:
                ancestor["descendents"] = []
                ancestor["count"] = node[3]

            if ancestor["count"] != node[3]:
                raise ValueError("number of count does not match.")
            ancestor["descendents"].append({"name": node[0], "type": node[1], "time_ms": node[2], "count": node[3]})
            ancestor["time_ms"] = ancestor["time_ms"] + node[2]

        else:
            model = parse.parse("Total time: {:f} ms\tcount: {:d}", line)
            if model:
                if "model" not in result.keys():
                    result["model"] = {"time_ms": model[0], "count": model[1]}
    return result


def run(args, options):

    result = None
    argv = options.copy()

    if "--model_file" in options and options.index("--model_file") + 1 < len(options):
        model = options[options.index("--model_file") + 1]
    else:
        raise ValueError("--model_file not found")

    if "--target" in options and options.index("--target") + 1 < len(options):
        target = options[options.index("--target") + 1]
    else:
        raise ValueError("--target not found")

    verbose = False
    if "--verbose" in options:
        verbose = True

    if "--profile" not in options:
        options = ["--profile"] + options

    # make a copy to modify output path to work with drivetest
    argv = options.copy()

    if "--outdir" in options and options.index("--outdir") + 1 < len(options):
        output_dir = options[options.index("--outdir") + 1]
        argv[options.index("--outdir") + 1] = os.path.join(output_dir, target)
    else:
        # insert to beginning instead of append to the end to avoid messing up additional args to compiler
        # TODO - change this order requirement to make it more flexible
        output_dir = os.path.splitext(model)[0]
        argv = ["--outdir", os.path.join(output_dir, target)] + argv

    log = None
    # TODO - drivetest.py does not support "orangepi0"
    with DriveTest(model=model,  # TODO - no labels, no expected
                   wrap_options=argv, iterations=args.iteration,
                   outdir=output_dir,
                   compile=COMPILE_INCREMENTAL, test=True,
                   ipaddress=args.ip, cluster=args.cluster, apikey=args.key, username=args.username,
                   password=args.password,
                   target=target, target_dir="/home/pi/" + target,
                   verbose=verbose,
                   ) as driver:
        driver.run_test()
        log = driver.profile_log

    if log is not None and len(log) > 0:
        result = parse_performance_log(log)

    return result


if __name__ == '__main__':
    parser = argparse.ArgumentParser(
        formatter_class=argparse.ArgumentDefaultsHelpFormatter,
        description="""ELL python script for generate and view profiler.
Options for profiling may be read from option files or pass in as arguments.
Full argument are required when passing profile options in as arguments.
Use --profile_options to review available options.""")

    verbosity = parser.add_mutually_exclusive_group()
    verbose_help = "Turn on verbosity logging level. Available levels are {}".format(
        list(logging._levelToName.values()))
    verbosity.add_argument("--verbosity", "-v", help=verbose_help, default="INFO",
                           choices=list(logging._levelToName.values()))
    verbosity.add_argument("--quiet", "-q", help="Turn off logging.", action="store_true", default=False)
    parser.add_argument("--log", "-l", help="Store output to log file.", default=None)

    parser.add_argument("--show", "-s", action="store_true", default=False,
                        help="Display profile option files only. Do not compile and profile.")
    parser.add_argument("--merge", "-m", action="store_true", default=False,
                        help="Merge profile option files only based on model file. Do not compile and profile.")
    parser.add_argument("--generate", "-g", action="store_true", default=False,
                        help="Generated profile option file from arguments only. Do not compile and profile.")

    parser.add_argument("--iteration", "-i", help="Number of iteration to run each model.", type=int, default=1)

    # profile options
    source = parser.add_mutually_exclusive_group()
    source.add_argument("--file", "-f", nargs="+", default=None, help="Input file that contains profile options. \
Multiple files are allowed, each seperated by space.")
    source.add_argument("--dir", "-d", help="Directory that contains profile option files. Wildcard * is allowed.",
                        default=None)
    parser.add_argument("--profile_options", "-o", help="Display profile options.", action="store_true", default=False)

    # cluster
    cluster = parser.add_argument_group("Cluster Options",
                                        "Specify pi cluster parameters if target platform is pi0 or pi3.")
    cluster.add_argument("--cluster", help="HTTP address of cluster manager.", default=os.getenv("RPI_CLUSTER", None))
    cluster.add_argument("--key", help="The ApiKey to use for the cluster.", default=os.getenv("RPI_APIKEY", None))
    cluster.add_argument("--ip", help="IP address of target device.", default=None)
    cluster.add_argument("--username", help="The username of target device.", default=os.getenv("RPI_USERNAME", None))
    cluster.add_argument("--password", help="The password of target device.", default=os.getenv("RPI_PASSWORD", None))

    args, argv = parser.parse_known_args()

    logger.setLevel(args.verbosity)
    logger_console_handler.setLevel(args.verbosity)
    logger.addHandler(logger_console_handler)

    if args.log:
        logger_file_handler = logging.FileHandler(args.log)
        logger_file_handler.setLevel(args.verbosity)
        logger.addHandler(logger_file_handler)

    if args.profile_options:
        # just display wrap options
        print("!!! NOTE: Profile options must use full arguments to avoid argument collision !!!")
        builder = ModuleBuilder()
        builder_args = ["-h"]
        builder.parse_command_line(builder_args)
        builder.run()
    else:
        if args.dir:
            # get file list from directory
            path = args.dir
            if "*" not in path:
                path = os.path.join(path, "*.json")
            args.file = glob.glob(path)

        if args.file:
            # read option from file
            all_profile = []
            for file in args.file:
                if os.path.exists(file) and os.path.isfile(file) and file.endswith(".json"):
                    profile = {}
                    with open(file=file, mode='r', newline="\n") as f:
                        profile = json.load(f)
                    if "model" not in profile.keys() or "profile" not in profile.keys():
                        logger.warning("file {} does not contain model or profile options.".format(file))
                    elif len(profile["profile"]) == 0:
                        logger.warning("file {} profile options are not available.".format(file))
                    else:
                        if args.show:
                            logger.info("=============================================================================\
==========================================================")
                            logger.info("{}".format(file))
                            logger.info("=============================================================================\
==========================================================")
                            show_profile(profile)
                            logger.info("\n")
                        elif args.merge:
                            all_profile.append(profile)
                        else:
                            logger.verbose("Run profile with {}".format(file))
                            for i, p in enumerate(profile["profile"]):
                                if "args" not in p.keys():
                                    logger.warning("file {} profile {} argument is not available.".format(file, i))
                                else:
                                    if "result" not in p.keys():
                                        p["result"] = {}
                                    entry = datetime.datetime.now().strftime("%Y-%m-%d %H:%M:%S")
                                    p["result"][entry] = run(args, p["args"])
                            with open(file=file, mode='w', newline="\n") as f:
                                logger.info("Save profile to {}".format(file))
                                json.dump(profile, f)
            if args.merge and len(all_profile) > 0:
                # merge profile files
                for i in range(len(all_profile)):
                    if all_profile[i]["model"] is not None:
                        merged_profile = all_profile[i]
                        j = i + 1
                        while j < len(all_profile):
                            if all_profile[j]["model"] is not None and os.path.samefile(merged_profile["model"],
                                                                                        all_profile[j]["model"]):
                                # matched model file
                                all_profile[j]["model"] = None
                                profiles = all_profile[j]["profile"]
                                unmatched_profiles = []
                                for profile in profiles:
                                    match = False
                                    for profile_merged in merged_profile["profile"]:
                                        if profile["config"] is not None and \
                                           profile_merged["config"] == profile["config"]:
                                            # matched config
                                            for entry in profile["result"].keys():
                                                profile_merged["result"][entry] = profile["result"][entry]
                                            match = True
                                            continue
                                    if not match:
                                        # unmatched config
                                        unmatched_profiles.append(profile)
                                merged_profile["profile"] = merged_profile["profile"] + unmatched_profiles
                            j += 1
                        # write out merged profile
                        file = os.path.splitext(merged_profile["model"])[0] + "_merged.json"
                        with open(file=file, mode='w', newline="\n") as f:
                            logger.info("Save merged profile to {}".format(file))
                            json.dump(merged_profile, f)
        else:
            # read option from argument
            output = options_to_profile(argv)
            if not args.generate:
                entry = datetime.datetime.now().strftime("%Y-%m-%d %H:%M:%S")
                # only one profile if options read from command line
                output["profile"][0]["result"][entry] = run(args, output["profile"][0]["args"])
            out_dir = output["profile"][0]["config"]["outdir"]
            out_file = os.path.join(out_dir, os.path.splitext(os.path.basename(output["model"]))[0] + ".json")
            if not os.path.exists(out_dir) or not os.path.isdir(out_dir):
                os.mkdir(out_dir)
            with open(file=out_file, mode='w', newline="\n") as f:
                logger.info("Save profile to {}".format(out_file))
                json.dump(output, f)
