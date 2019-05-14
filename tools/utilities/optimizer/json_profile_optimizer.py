#!/usr/bin/env python3
####################################################################################################
#
#  Project:  Embedded Learning Library (ELL)
#  File:     optimizer.py
#  Authors:  Ying Guo
#
#  Requires: Python 3.x
#
####################################################################################################
import argparse
import glob
import json
import logging
import numpy
import os

LOG_LEVEL_VERBOSE = 1


def verbose(self, message, *args, **kws):
    if self.isEnabledFor(LOG_LEVEL_VERBOSE):
        self._log(LOG_LEVEL_VERBOSE, message, args, **kws)


logging.addLevelName(LOG_LEVEL_VERBOSE, "VERBOSE")
logging.Logger.verbose = verbose
logger = logging.getLogger(__name__)
logger_console_handler = logging.StreamHandler()


def read_profiles(profile_files):
    # look through all the profile files
    profiles = {}
    for p in profile_files:
        if os.path.exists(p) and os.path.isfile(p) and p.endswith(".json"):
            logger.verbose("Examining file {}...".format(p))
            profile = {}
            with open(file=p, mode='r', newline="\n") as f:
                profile = json.load(f)
            if "model" not in profile.keys() or "profile" not in profile.keys():
                logger.warning("file {} does not contain model or profile.".format(p))
            elif not os.path.isfile(profile["model"]):
                logger.warning("file {} does not contain valid input ELL model.".format(p))
            elif len(profile["profile"]) == 0:
                logger.warning("file {} profile is not available.".format(p))
            else:
                # find best run for each model
                # TODO - here we ignore the global compiler options.
                best = {}
                for p in profile["profile"]:
                    for r in p["result"].keys():
                        perf = p["result"][r]
                        if "model" in perf.keys() and "node" in perf.keys():
                            if len(best) == 0:
                                best = perf
                            elif best["model"]["time_ms"] / best["model"]["count"] > \
                                    perf["model"]["time_ms"] / perf["model"]["count"]:
                                best = perf
                if len(best) > 0:
                    logger.verbose("Found valid profile!")
                    profiles[profile["model"]] = best
    if len(profiles) == 0:
        raise ValueError("No valid profiles found!")
    return profiles


def optimize(profile_files, output):
    profiles = read_profiles(profile_files)

    # build up options based on valid profiles
    base_model = {}
    base_nodes = set()
    options = {}
    model_choice = []
    model_path = []
    for m in profiles.keys():
        logger.verbose("Examing model {}...".format(m))
        if len(base_nodes) == 0:
            # set the first model as base model
            logger.verbose("Use model {} as base...".format(m))
            with open(file=m, mode="r", newline="\n") as f:
                base_model = json.load(f)
            for n in base_model["model"]["nodes"]:
                base_nodes.add("node[" + n["id"] + "]")
        if not set(profiles[m]["node"].keys()).issubset(base_nodes):
            logger.warning("model {} is not compatible with base.".format(m))
        else:
            with open(file=m, mode="r", newline="\n") as f:
                model = json.load(f)
            # now build all options
            model_choice.append(model)
            model_path.append(m)
            for n in profiles[m]["node"].keys():
                id = n[5:-1]
                if id not in options.keys():
                    options[id] = []
                options[id].append(profiles[m]["node"][n]["time_ms"] / profiles[m]["node"][n]["count"])
    if len(model_choice) == 0:
        raise ValueError("No valid model choices!")

    # NOTE: no longer need "profiles" object here

    # optimize model based on options
    optimized_model = base_model
    if len(model_choice) == 1:
        logger.warning("Only one valid model choices!")
    else:
        for i, n in enumerate(base_model["model"]["nodes"]):
            # TODO - only optimize based on convolution method at this point
            # TODO - only chose the convolution method based on perf at this point. Need other strategy.
            decision = 0
            if "ConvolutionalLayerNode" in n["_type"]:
                decision = numpy.argmin(options[n["id"]])
            optimized_model["model"]["nodes"][i] = model_choice[decision]["model"]["nodes"][i]

    # check if the model is modified
    modified = True
    for i, m in enumerate(model_choice):
        if optimized_model == m:
            modified = False
            logger.info("Model {} is already the most optimized solution.".format(model_path[i]))
            break

    # save optimized model
    if modified:
        with open(file=output, mode="w", newline="\n") as f:
            logger.info("Save optimized model to {}.".format(output))
            json.dump(optimized_model, f)

    return modified


if __name__ == '__main__':

    parser = argparse.ArgumentParser(
        formatter_class=argparse.ArgumentDefaultsHelpFormatter,
        description="ELL optimizer script for optimizing model based on profiles.\n")

    verbosity = parser.add_mutually_exclusive_group()
    verbose_help = "Turn on verbosity logging level. Available levels are {}".format(
        list(logging._levelToName.values()))
    verbosity.add_argument("--verbosity", "-v", help=verbose_help, choices=list(logging._levelToName.values()),
                           default="INFO")
    verbosity.add_argument("--quiet", "-q", help="Turn off logging.", action="store_true", default=False)
    parser.add_argument("--log", "-l", help="Store output to log file.", default=None)

    source = parser.add_mutually_exclusive_group(required=True)
    source.add_argument("--file", "-f", nargs="+", default=None,
                        help="ELL model profiles. Multiple files are allowed, each seperated by space.")
    source.add_argument("--dir", "-d", default=None,
                        help="Directory that contains ELL model profiles. Wildcard * is allowed.")

    parser.add_argument("--output", "-o", help="Output optimized ELL model.", default="model.ell")

    args = parser.parse_args()

    logger.setLevel(args.verbosity)
    logger_console_handler.setLevel(args.verbosity)
    logger.addHandler(logger_console_handler)

    if args.log:
        logger_file_handler = logging.FileHandler(args.log)
        logger_file_handler.setLevel(args.verbosity)
        logger.addHandler(logger_file_handler)

    if args.dir:
        # get file list from directory
        path = args.dir
        if "*" not in path:
            path = os.path.join(path, "*.json")
        args.file = glob.glob(path)

    if args.file is None:
        raise ValueError("No valid input files found!")

    optimize(args.file, args.output)
