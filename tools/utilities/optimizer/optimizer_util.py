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

from enum import Enum
import os
import zipfile
import json

script_path = os.path.dirname(os.path.abspath(__file__))
default_options_file = os.path.join(script_path, "default_options.json")


class ParallelOptions(Enum):
    Parallel = True
    Sequential = False


class ConvolutionMethod(Enum):
    Simple = "simple"
    Unrolled = "unrolled"
    Winograd = "winograd"


class WinogradConfiguration:
    def __init__(self, separable_block_depth, non_separable_filter_block_depth, non_separable_channel_depth):
        self.separable_block_depth = separable_block_depth
        self.non_separable_filter_block_depth = non_separable_filter_block_depth
        self.non_separable_channel_depth = non_separable_channel_depth

    def to_short_string(self):
        return "{}_{}_{}".format(self.separable_block_depth, self.non_separable_filter_block_depth, self.non_separable_channel_depth)

    def to_long_string(self):
        return "SeparableBlockDepth: {}, NonSeparableFilterBlockDepth: {}, NonSeparableChannelDepth: {}".format(self.separable_block_depth, self.non_separable_filter_block_depth, self.non_separable_channel_depth)

    def to_option_map(self):
        return {"separableBlockDepth":self.separable_block_depth,
                "nonseparableFilterBlockDepth":self.non_separable_filter_block_depth,
                "nonseparableChannelDepth":self.non_separable_channel_depth}


class OptionsKeys(Enum):
    Parallel = "parallel"
    ConvolutionMethod = "convolution_methods"
    WinogradParameters = "winograd_parameters"


class ProfileOptions:
    def __init__(self, convolution_method, parallel_option, winograd_configuration=None):
        if convolution_method not in ConvolutionMethod:
            raise Exception("{} ({}) is not a value of ConvolutionMethod".format(convolution_method, type(convolution_method)))
        self.convolution_method = convolution_method

        if parallel_option not in ParallelOptions:
            raise Exception("{} ({}) is not a value of ParallelOptions".format(parallel_option, type(parallel_option)))
        self.parallel_option = parallel_option

        if convolution_method is ConvolutionMethod.Winograd and type(winograd_configuration) is not WinogradConfiguration:
            raise Exception("Winograd method specified but configuration parameters {} ({}) is not a WinogradConfiguration object".format(winograd_configuration, type(winograd_configuration)))
        self.winograd_configuration = winograd_configuration

    @staticmethod
    def parse_option_list_from_file(options_json_file_path):
        # Parses a given json file describing all of the options to profile
        # Returns a list of ProfileOptions where each entry is a ProfileOptions
        # object with a different combination of options from the given file
        options = None
        with open(options_json_file_path) as f:
            options = json.load(f)
        if OptionsKeys.ConvolutionMethod.value not in options or len(options[OptionsKeys.ConvolutionMethod.value]) == 0:
            raise Exception("No convolution methods specified")
        if OptionsKeys.Parallel.value not in options or len(options[OptionsKeys.Parallel.value]) == 0:
            raise Exception("No parallel settings specified")

        def parse_valid_options(key, option_type):
            options_strings = options[key]
            value_to_key = { x.value : x for x in option_type }
            unrecognized_values = []
            for val in unrecognized_values:
                if val not in value_to_key:
                    unrecognized_values.append(val)
            if len(unrecognized_values) > 0:
                raise Exception("Unrecognized option values: {}, valid options are {}".format(unrecognized_values, list(value_to_key.keys())))
            return [value_to_key[val] for val in options_strings]

        convolution_methods = parse_valid_options(OptionsKeys.ConvolutionMethod.value, ConvolutionMethod)
        parallel_methods = parse_valid_options(OptionsKeys.Parallel.value, ParallelOptions)

        winograd_profile_options = []
        if ConvolutionMethod.Winograd in convolution_methods:
            winograd_test_values = [int(val) for val in options[OptionsKeys.WinogradParameters.value]]
            winograd_configs = [WinogradConfiguration(x, y, z) for x in winograd_test_values for y in winograd_test_values for z in winograd_test_values]
            winograd_profile_options = [ProfileOptions(ConvolutionMethod.Winograd, parallel, winograd_config) for parallel in parallel_methods for winograd_config in winograd_configs]

        non_winograd_profile_options = [ProfileOptions(conv, parallel, None) for conv in convolution_methods for parallel in parallel_methods if conv is not ConvolutionMethod.Winograd]

        return winograd_profile_options + non_winograd_profile_options

    def to_short_string(self):
        parallel_str = "_parallel" if self.parallel_option.value else ""
        if self.convolution_method is ConvolutionMethod.Winograd:
            return "{}_{}{}".format(self.convolution_method.value, self.winograd_configuration.to_short_string(), parallel_str)
        else:
            return "{}{}".format(self.convolution_method.value, parallel_str)

    def to_long_string(self):
        if self.convolution_method is ConvolutionMethod.Winograd:
            return "Convolution Method: {}, {}, Parallel: {}".format(self.convolution_method.value, self.winograd_configuration.to_long_string(), self.parallel_option.value)
        else:
            return "Convolution Method: {}, Parallel: {}".format(self.convolution_method.value, self.parallel_option.value)

    def base_wrap_args(self):
        return []

    def additional_compile_args(self):
        if self.convolution_method is ConvolutionMethod.Winograd:
            # Generate ["--modelOption", "optionName,optionValue", ...] for each winograd parameter option
            winograd_options_map = self.winograd_configuration.to_option_map()
            winograd_options_map.update({"preferredConvolutionMethod": "winograd"})
            options_list = ["{},{}".format(key, winograd_options_map[key]) for key in winograd_options_map]
            model_options_list = ["--modelOption"] * len(options_list)
            args = [value for pair in zip(model_options_list, options_list) for value in pair]
        else:
            args = ["--convolutionMethod", self.convolution_method.value]
        if self.parallel_option.value:
            args.append("-par")
        return args


def add_options_args(arg_parser):
    arg_parser.add_argument("--options", help="Path to options json file specifying profiling options", default=default_options_file)


def add_model_args(arg_parser):
    arg_parser.add_argument("--model_path", help="Directory containing the models to process", required=True)
    arg_parser.add_argument("--models", help="Names of models to run", nargs="*", default=None)


def add_target_args(arg_parser):
    arg_parser.add_argument("--target", help="Target to cross-compile for", default="pi3")


def add_all_shared_args(arg_parser):
    add_options_args(arg_parser)
    add_model_args(arg_parser)
    add_target_args(arg_parser)


class ProfilerData:
    def __init__(self, model_tag, model_name, profile_options, model_path, built_profilers_path):
        self.friendly_name = "optimization profiler : {} {}_{}".format(model_tag, model_name, profile_options.to_short_string())
        self.model_tag = model_tag
        self.model_name = model_name
        self.profile_options = profile_options
        self.model_path = model_path
        self.built_profilers_path = built_profilers_path
        os.makedirs(built_profilers_path, exist_ok=True)
        self.command = "build_and_run.sh"


def unzip_model(input_path):
    with zipfile.ZipFile(input_path, "r") as zip_file:
        zip_file.extractall(os.path.dirname(input_path))


def find_ell_models(search_path, models=None):
    # Searches a path for an .ell model file or a .ell.zip zipped model file
    # Creates and returns a dictionary mapping { base_filename -> model_path }
    # If there are conflicts with the base_filename, a #N suffix is added, where N is an incrementing integer
    ell_suffix = "ell"
    ell_zip_suffix = "ell.zip"

    model_names_to_paths = {}
    for dir, _, files in os.walk(search_path):
        if files:
            for f in files:
                file_path = os.path.join(dir, f)
                add_path = None
                if f.endswith(ell_suffix):
                    add_path = file_path
                elif f.endswith(ell_zip_suffix):
                    unzip_model(file_path)
                    add_path = os.path.splitext(file_path)[0]
                if add_path:
                    init_name = os.path.splitext(os.path.basename(add_path))[0]
                    # If we're only taking models with particular names, filter out any that aren't in the name list
                    if models and init_name not in models:
                        continue

                    name = init_name
                    idx = 1
                    # If this model name has already been found but points to a different path, modify this name to have an
                    # incrementing integer suffix so the models can be distinguished
                    while name in model_names_to_paths and model_names_to_paths[name] is not add_path:
                        name = "{}_{}".format(init_name, idx)
                        idx += 1
                    model_names_to_paths[name] = (init_name, add_path)
    return model_names_to_paths


def make_profiler_data(model_path, built_profilers_path, all_profile_options, models=None):
    model_tags_to_paths = find_ell_models(model_path, models)

    all_profiler_data = [ProfilerData(tag,
                                      model_tags_to_paths[tag][0],
                                      profile_options,
                                      model_tags_to_paths[tag][1],
                                      os.path.join(built_profilers_path, "{}_{}".format(tag, profile_options.to_short_string())))
                                      for tag in model_tags_to_paths for profile_options in all_profile_options]
    return all_profiler_data
