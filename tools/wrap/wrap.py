#!/usr/bin/env python3
####################################################################################################
##
##  Project:  Embedded Learning Library (ELL)
##  File:     wrap.py
##  Authors:  Chris Lovett, Kern Handa
##
##  Requires: Python 3.x
##
####################################################################################################
import os
import sys
sys.path.append(os.path.join(os.path.dirname(os.path.abspath(__file__)), '../utilities/pythonlibs'))
import find_ell
import argparse
import subprocess
import json
import operator
from shutil import copyfile
import buildtools

# This script creates a compilable Python project for executing a given ELL model on a target platform.
# Compilation of the resulting project will require a C++ compiler.

class ModuleBuilder:
    def __init__(self):
        self.target = "host"
        self.config = None
        self.model_file = None
        self.output_dir = None
        self.files = []
        self.includes = []
        self.tcc = []
        self.tools = None
        self.func_name = "Predict"
        self.objext = "obj"
        self.tools = None
        self.language = "python"
        self.verbose = False
        self.llvm_format = None
        self.no_opt_tool = False
        self.no_llc_tool = False
        self.profile = False
        self.blas = True
        self.optimize = True
        self.optimization_level = None
        self.fuse_linear_ops = True
        self.debug = False
        self.model_name = ""

    def str2bool(self, v):
        return v.lower() in ("yes", "true", "t", "1")

    def parse_command_line(self, args=None):
        arg_parser = argparse.ArgumentParser("This tool wraps a given ELL model in a CMake buildable project that builds a language\n"
            "specific module that can call the ELL model on a given target platform.\n"
            "\nThe supported languages are:\n"
            "    python   (default)\n"
            "    cpp\n"
            "\nThe supported target platforms are:\n"
            "    pi0      Raspberry Pi 0\n"
            "    pi3      Raspberry Pi 3\n"
            "    aarch64  arm64 Linux, works on Qualcomm DragonBoards\n"
            "    host     (default) your host computer architecture\n")

        # required arguments
        arg_parser.add_argument("model_file", help="path to the ELL model file")

        # optional arguments
        arg_parser.add_argument("--language", "-lang", help="the language for the ELL module", choices=["python", "cpp"], default=self.language)
        arg_parser.add_argument("--target", "-target", help="the target platform", choices=["pi3", "pi0", "orangepi0", "pi3_64", "aarch64", "host"], default=self.target)
        arg_parser.add_argument("--outdir", "-outdir", help="the output directory")
        arg_parser.add_argument("--profile", "-profile", help="enable profiling functions in the ELL module", action="store_true")
        arg_parser.add_argument("--verbose", "-v", help="print verbose output", action="store_true")
        arg_parser.add_argument("--blas", help="enable or disable the use of Blas on the target device (default 'True')", default="True")
        arg_parser.add_argument("--llvm-format", help="the format of the emitted code (default 'bc')", choices=["ir", "bc", "asm"], default="bc")
        arg_parser.add_argument("--no-fuse-linear-ops", help="disable the fusing of sequences of linear operations", action="store_true")
        arg_parser.add_argument("--no-opt-tool", help="disable the use of LLVM's opt tool", action="store_true")
        arg_parser.add_argument("--no-llc-tool", help="disable the use of LLVM's llc tool", action="store_true")
        arg_parser.add_argument("--no-optimize", help="disable ELL's compiler from optimizing emitted code", action="store_true")
        arg_parser.add_argument("--optimization-level", help=("the optimization level used by LLVM's opt and llc tools. If '0' or 'g', opt "
            "is not run (default '3')"), choices=["0", "1", "2", "3", "g"], default="3")
        arg_parser.add_argument("--debug", help="emit debug code", action="store_true")

        args = arg_parser.parse_args(args)

        self.model_file = args.model_file
        _, tail = os.path.split(self.model_file)
        self.model_name = os.path.splitext(tail)[0]

        self.language = args.language
        self.target = args.target
        self.output_dir = args.outdir
        if self.output_dir is None:
            self.output_dir = self.target
        self.profile = args.profile
        self.verbose = args.verbose
        self.llvm_format = args.llvm_format
        self.optimization_level = args.optimization_level
        self.no_opt_tool = args.no_opt_tool or self.optimization_level in ['0', 'g']
        self.no_llc_tool = args.no_llc_tool
        self.optimize = not args.no_optimize
        self.fuse_linear_ops = not args.no_fuse_linear_ops
        self.debug = args.debug
        self.blas = self.str2bool(args.blas)

    def find_files(self):
        self.cmake_template = os.path.join(os.path.dirname(os.path.abspath(__file__)), "templates/CMakeLists.%s.txt.in" % (self.language))
        if (not os.path.isfile(self.cmake_template)):
            raise Exception("Could not find CMakeLists template: %s" % (self.cmake_template))
        self.files.append(os.path.join(self.ell_root, "CMake/OpenBLASSetup.cmake"))
        self.includes.append(os.path.join(self.ell_root, "interfaces/common/include/CallbackInterface.h"))
        self.tcc.append(os.path.join(self.ell_root, "interfaces/common/tcc/CallbackInterface.tcc"))

    def copy_files(self, filelist, folder):
        if not folder:
            target_dir = self.output_dir
        else:
            target_dir = os.path.join(self.output_dir, folder)
        os.makedirs(target_dir, exist_ok=True)
        for path in filelist:
            if not os.path.isfile(path):
                raise Exception("expected file not found: " + path)
            _, file_name = os.path.split(path)
            dest = os.path.join(target_dir, file_name)
            if self.verbose:
                print("copy \"%s\" \"%s\"" % (path, dest))
            copyfile(path, dest)

    def create_cmake_file(self):
        with open(self.cmake_template) as f:
            template = f.read()

        template = template.replace("@ELL_model@", self.model_name)
        template = template.replace("@Arch@", self.target)
        template = template.replace("@OBJECT_EXTENSION@", self.objext)
        template = template.replace("@ELL_ROOT@", os.path.join(self.ell_root, "external").replace("\\","/"))
        output_template = os.path.join(self.output_dir, "CMakeLists.txt")
        with open(output_template, 'w') as f:
            f.write(template)

    def save_config(self):
        self.config['model'] = self.model_name
        self.config['func'] = self.model_name + "_" + self.func_name
        config_json = json.dumps(self.config, indent=2, sort_keys=True)
        _, tail = os.path.split(self.config_file)
        outputFile = os.path.join(self.output_dir, tail)
        print("creating config file: '" + outputFile + "'")
        with open(outputFile, 'w') as f:
            f.write(config_json)
            f.close()

    def run(self):
        self.build_root = find_ell.find_ell_build()
        self.ell_root = os.path.dirname(self.build_root)
        self.tools = buildtools.EllBuildTools(self.ell_root, self.verbose)
        self.find_files()
        self.copy_files(self.files, "")
        self.copy_files(self.includes, "include")
        self.copy_files(self.tcc, "tcc")
        out_file = self.tools.compile(
            self.model_file, self.func_name, self.model_name, self.target, self.output_dir, self.blas, self.fuse_linear_ops, self.profile,
            self.llvm_format, self.optimize, self.debug)
        if self.language != "cpp":
            self.tools.swig(self.output_dir, self.model_name, self.language)
        if not self.no_opt_tool:
            out_file = self.tools.opt(self.output_dir, out_file, self.optimization_level)
        if not self.no_llc_tool:
            out_file = self.tools.llc(self.output_dir, out_file, self.target, self.optimization_level)
        self.create_cmake_file()
        if self.target == "host":
            print("success, now you can build the '" + self.output_dir + "' folder")
        else:
            print("success, now copy the '" + self.output_dir + "' folder to your target machine and build it there")

if __name__ == "__main__":
    builder = ModuleBuilder()
    builder.parse_command_line()
    builder.run()
