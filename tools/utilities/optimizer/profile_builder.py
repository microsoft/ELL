#!/usr/bin/env python3

####################################################################################################
#
#  Project:  Embedded Learning Library (ELL)
#  File:     profile_builder.py
#  Authors:  Mason Remy
#
#  Requires: Python 3.x
#
####################################################################################################

import sys
import argparse
import os
import zipfile
import shutil
import dask

import logger
import find_ell
import optimizer_util

script_path = os.path.dirname(os.path.abspath(__file__))
sys.path.append(os.path.join(script_path, "..", "..", "wrap"))
import wrap

global_logger = logger.setup()


class BuildJob():
    def __init__(self, profiler_data, ell_root, ell_build_root, target="pi3", language="cpp", include_exercise_models=False):
        self.profiler_data = profiler_data
        self.ell_root = ell_root
        self.ell_build_root = ell_build_root
        self.target = target
        self.language = language
        self.include_exercise_models = include_exercise_models

    def run(self):
        model_id_string = "{} ({})".format(self.profiler_data.model_name, self.profiler_data.profile_options.to_long_string())

        global_logger = logger.setup(format="{} %(message)s".format(model_id_string))
        global_logger.info("Compiling model {} ...".format(model_id_string))
        profile_src_path = os.path.join(self.ell_build_root, "tools", "utilities", "profile")

        # Take all .cpp, .h, .cmd, .sh, and .cmake files from the build/tools/utilities/profile directory, and only hard-code the CMakeLists file to take
        all_profile_files = os.listdir(profile_src_path)
        profile_extensions_to_deploy = [".cpp", ".h", ".cmd", ".sh", ".cmake"]
        profile_src_files = [filename for filename in all_profile_files if True in [filename.endswith(ext) for ext in profile_extensions_to_deploy]]

        cmakelists_file_to_deploy = "CMakeLists-device-parallel.txt.in"
        profile_src_files.append(cmakelists_file_to_deploy)
        profile_src_file_renames = {
            cmakelists_file_to_deploy : "CMakeLists.txt"
        }

        base_wrap_args = [
            "--model_file",
            self.profiler_data.model_path,
            "--module_name",
            "ELL", # Profiler C++ code assumes the module name is ELL
            "--target",
            self.target,
            "--language",
            self.language,
            "--outdir",
            self.profiler_data.built_profilers_path,
        ]

        base_wrap_args.extend(self.profiler_data.profile_options.base_wrap_args())

        opt_wrap_args = [
            "--llvm_format",
            "ir"
        ]
        no_opt_wrap_args = [
            "--no_opt_tool",
            "--no_llc_tool",
            "--llvm_format",
            "obj"
        ]

        profile_wrap_args = ["--profile"]

        profile_options_additional_compile_args = self.profiler_data.profile_options.additional_compile_args()
        if profile_options_additional_compile_args:
            additional_compile_args = ["--"] + profile_options_additional_compile_args
        else:
            additional_compile_args = []

        no_opt_noprofile_builder = wrap.ModuleBuilder()
        no_opt_noprofile_wrap_args = base_wrap_args + no_opt_wrap_args + additional_compile_args
        no_opt_noprofile_builder.parse_command_line(no_opt_noprofile_wrap_args)

        no_opt_profile_builder = wrap.ModuleBuilder()
        no_opt_profile_wrap_args = base_wrap_args + no_opt_wrap_args + profile_wrap_args + additional_compile_args
        no_opt_profile_builder.parse_command_line(no_opt_profile_wrap_args)

        opt_noprofile_builder = wrap.ModuleBuilder()
        opt_noprofile_wrap_args = base_wrap_args + opt_wrap_args + additional_compile_args
        opt_noprofile_builder.parse_command_line(opt_noprofile_wrap_args)

        opt_profile_builder = wrap.ModuleBuilder()
        opt_profile_wrap_args = base_wrap_args + opt_wrap_args + profile_wrap_args + additional_compile_args
        opt_profile_builder.parse_command_line(opt_profile_wrap_args)

        # Profiler and ExecuteModel binaries expect to find the following header and object files, built with different compile args:
        # compiled_model.o
        # compiled_model.h
        # compiled_model_opt.o
        # compiled_model_opt.h
        # compiled_model_noprofile.o
        # compiled_model_noprofile.h
        # compiled_model_noprofile_opt.o
        # compiled_model_noprofile_opt.h
        built_name_prefix = "compiled_model"
        noprofile_suffix = "_noprofile"
        opt_suffix = "_opt"
        obj_suffix = ".o"
        header_suffix = ".h"
        file_suffixes = [obj_suffix, header_suffix]
        base_model_filename = os.path.basename(self.profiler_data.model_path)
        base_model_name = os.path.splitext(base_model_filename)[0]

        no_opt_noprofile_renames = { (base_model_name + file_suffix) : (built_name_prefix + noprofile_suffix + file_suffix) for file_suffix in file_suffixes }
        no_opt_profile_renames = { (base_model_name + file_suffix) : (built_name_prefix + file_suffix) for file_suffix in file_suffixes }
        opt_noprofile_renames = { (base_model_name + file_suffix) : (built_name_prefix + noprofile_suffix + opt_suffix + file_suffix) for file_suffix in file_suffixes }
        opt_profile_renames = { (base_model_name + file_suffix) : (built_name_prefix + opt_suffix + file_suffix) for file_suffix in file_suffixes }

        builders_and_rename_maps = [
            (no_opt_profile_builder, no_opt_profile_renames),
            (opt_profile_builder, opt_profile_renames)
        ]
        if self.include_exercise_models:
            builders_and_rename_maps.extend([
                (no_opt_noprofile_builder, no_opt_noprofile_renames),
                (opt_noprofile_builder, opt_noprofile_renames)
            ])

        target_files = []
        for builder_and_rename_map in builders_and_rename_maps:
            target_files.extend([builder_and_rename_map[1][filename] for filename in builder_and_rename_map[1]])

        existing_files = os.listdir(self.profiler_data.built_profilers_path)
        need_to_build = False in [filename in existing_files for filename in target_files]

        if need_to_build:
            try:
                for builder_and_rename_map in builders_and_rename_maps:
                    builder = builder_and_rename_map[0]
                    rename_map = builder_and_rename_map[1]
                    builder.run()
                    for filename in rename_map:
                        src_path = os.path.join(self.profiler_data.built_profilers_path, filename)
                        dst_path = os.path.join(self.profiler_data.built_profilers_path, rename_map[filename])
                        shutil.copy(src_path, dst_path)
            except:
                errorType, value, traceback = sys.exc_info()
                msg = "### WrapException: %s: %s" % (str(errorType), str(value))
                global_logger.error(msg)
                sys.exit(1)
        else:
            global_logger.info("Target files already exist in path {}, skipping build".format(self.profiler_data.built_profilers_path))

        keep_files = target_files

        # Copy profile source code into place
        for filename in profile_src_files:
            src_path = os.path.join(profile_src_path, filename)
            dst_name = profile_src_file_renames[filename] if filename in profile_src_file_renames else filename
            dst_path = os.path.join(self.profiler_data.built_profilers_path, dst_name)
            keep_files.append(dst_name)
            shutil.copy(src_path, dst_path)

        all_built_files = os.listdir(self.profiler_data.built_profilers_path)
        delete_files = [filename for filename in all_built_files if filename not in keep_files]
        for filename in delete_files:
            full_path = os.path.join(self.profiler_data.built_profilers_path, filename)
            if os.path.isfile(full_path):
                global_logger.info("Cleaning temporary build file {}".format(full_path))
                os.remove(full_path)

        global_logger.info("Done compiling model {}.".format(model_id_string))


def build_model_profilers(model_path, output_path, ell_root, ell_build_root, all_profile_options, models=None, target="pi3", parallel_count=1, include_exercise_models=False):
    os.makedirs(output_path, exist_ok=True)
    all_profiler_data = optimizer_util.make_profiler_data(model_path, output_path, all_profile_options, models)
    build_jobs = [BuildJob(profiler_data, ell_root, ell_build_root, target) for profiler_data in all_profiler_data]
    values = [dask.delayed(job.run)() for job in build_jobs]
    if len(build_jobs) < parallel_count:
        parallel_count = len(build_jobs)
    global_logger.info("Compiling {} models with {} processes".format(len(build_jobs), parallel_count))
    results = dask.compute(*values, scheduler="processes", num_workers=parallel_count)
    return all_profiler_data


def add_profile_builder_args(arg_parser):
    arg_parser.add_argument("--parallel_build", help="The maximum number of build processes to run in parallel.", default=4, type=int)
    arg_parser.add_argument("--include_exercise_models", help="Build and run the exercise_models executables that don't have profiling enabled", default=False, action="store_true")


if __name__ == "__main__":
    parser = argparse.ArgumentParser("Build model profilers with different convolution configurations")
    parser.add_argument("--output_path", "-o", help="Path to store built profiler object files in", required=True)
    add_profile_builder_args(parser)
    find_ell.add_ell_root_args(parser)
    optimizer_util.add_all_shared_args(parser)
    args = parser.parse_args()

    if not args.ell_root:
        raise Exception("ELL root not found")
    if not args.ell_build_root:
        raise Exception("ELL build root not found")

    all_profile_options = optimizer_util.ProfileOptions.parse_option_list_from_file(args.options)

    build_model_profilers(model_path=args.model_path,
                          output_path=args.output_path,
                          ell_root=args.ell_root,
                          ell_build_root=args.ell_build_root,
                          target=args.target,
                          all_profile_options=all_profile_options,
                          models=args.models,
                          parallel_count=args.parallel_build,
                          include_exercise_models=args.include_exercise_models)
