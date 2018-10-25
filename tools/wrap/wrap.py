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

import argparse
import json
import logging
import operator
import os
import platform
import subprocess
import sys
from shutil import copyfile

__script_path = os.path.dirname(os.path.abspath(__file__))
sys.path += [os.path.join(__script_path, "..", "utilities", "pythonlibs")]

import find_ell
import buildtools
import logger

# This script creates a compilable Python project for executing a given ELL model on a target platform.
# Compilation of the resulting project will require a C++ compiler.

class _PassArgsParser(argparse.ArgumentParser):
    def __init__(self, *args, **kwargs):
        super(_PassArgsParser, self).__init__(*args, **kwargs)

    def format_usage(self):
        usage = super(_PassArgsParser, self).format_usage().strip('\n')
        usage += " [-- <compile args>]\n"
        return usage

    def format_help(self):
        usage = super(_PassArgsParser, self).format_usage()
        help = super(_PassArgsParser, self).format_help()
        dashdash_help = "  --                    everything after '--' is passed to the compiler\n"
        return help.replace(usage, self.format_usage()) + dashdash_help

class ModuleBuilder:
    arguments = { "model_file"          : {"required":True,     "help": "path to the ELL model file"},
                  "module_name"         : {"default":None,      "help": "the name of the output module (defaults to the model filename)"},
                  "target"              : {"default":"host",    "help": "the target platform", "choices":["pi3", "pi0", "orangepi0", "pi3_64", "aarch64", "host"]},
                  "language"            : {"default":"python",  "help": "the language for the ELL module", "choices":["python", "cpp"]},
                  "llvm_format"         : {"default":"bc",      "help": "the format of the emitted code (default 'bc')", "choices":["ir", "bc", "asm", "obj"]},
                  "outdir"              : {"default":None,      "help": "the output directory"},
                  "verbose"             : {"default":False,     "help": "print verbose output"},
                  "profile"             : {"default":False,     "help": "enable profiling functions in the ELL module"},
                  "blas"                : {"default":"true",    "help": "enable or disable the use of Blas on the target device (default 'true')"},
                  "no_fuse_linear_ops"  : {"default":False,     "help": "disable the fusing of sequences of linear operations"},
                  "no_opt_tool"         : {"default":False,     "help": "disable the use of LLVM's opt tool"},
                  "no_llc_tool"         : {"default":False,     "help": "disable the use of LLVM's llc tool"},
                  "no_optimize"         : {"default":False,     "help": "disable ELL's compiler from optimizing emitted code"},
                  "optimization_level"  : {"default":"3",       "help": "the optimization level used by LLVM's opt and llc tools. If '0' or 'g', opt is not run (default '3')", "choices":["0", "1", "2", "3", "g"]},
                  "debug"               : {"default":False,     "help": "emit debug code"},
                }

    def __init__(self):
        self.config = None
        self.files = []
        self.includes = []
        self.tcc = []
        self.tools = None
        self.language = "python"
        self.target = "host"
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
        self.func_name = "Predict"
        self.objext = "o"
        self.logger = logger.get()

    def str2bool(self, v):
        return v.lower() in ("yes", "true", "t", "1")

    def get_objext(self, target):
        return "o"

    def parse_command_line(self, args=None):
        arg_parser = _PassArgsParser(prog="wrap", description="This tool wraps a given ELL model in a CMake buildable project that builds a language\n"
            "specific module that can call the ELL model on a given target platform.\n"
            "\nThe supported languages are:\n"
            "    python   (default)\n"
            "    cpp\n"
            "\nThe supported target platforms are:\n"
            "    pi0       Raspberry Pi 0\n"
            "    pi3       Raspberry Pi 3\n"
            "    orangepi0 Orange Pi Zero\n"
            "    aarch64   arm64 Linux, works on Qualcomm DragonBoards\n"
            "    host      (default) your host computer architecture\n")

        for arg in self.arguments.keys():
            if "required" in self.arguments[arg].keys():
                arg_parser.add_argument("--" + arg, help=self.arguments[arg]["help"], required=True)
            elif "choices" in self.arguments[arg].keys():
                arg_parser.add_argument("--" + arg, help=self.arguments[arg]["help"], default=self.arguments[arg]["default"], choices=self.arguments[arg]["choices"])
            elif self.arguments[arg]["default"] == False:
                arg_parser.add_argument("--" + arg, help=self.arguments[arg]["help"], action="store_true", default=False)
            else:
                arg_parser.add_argument("--" + arg, help=self.arguments[arg]["help"], default=self.arguments[arg]["default"])

        compile_args = []
        if '--' in args:
            index = args.index('--')
            compile_args = args[index+1:]
            args = args[:index]
        args = arg_parser.parse_args(args)

        self.model_file = args.model_file
        _, tail = os.path.split(self.model_file)
        self.model_file_base = os.path.splitext(tail)[0]
        self.model_name = args.module_name
        if not self.model_name:
            self.model_name = self.model_file_base.replace('-', '_')
        self.language = args.language
        self.target = args.target
        self.objext = self.get_objext(self.target)
        self.output_dir = args.outdir
        if self.output_dir is None:
            self.output_dir = self.target
        if os.path.isfile(self.output_dir + ".py"):
            raise Exception("You have a python module named '{}', which will conflict with the --outdir of '{}'. Please specify a different outdir.".format(self.output_dir + ".py", self.output_dir))
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
        self.swig = self.language != "cpp"
        self.cpp_header = self.language == "cpp"
        self.compile_args = compile_args

    def str2bool(self, v):
        return v.lower() in ("yes", "true", "t", "1")

    def find_files(self):
        self.cmake_template = os.path.join(os.path.dirname(os.path.abspath(__file__)), "templates/CMakeLists.%s.txt.in" % (self.language))
        if (not os.path.isfile(self.cmake_template)):
            raise Exception("Could not find CMakeLists template: %s" % (self.cmake_template))
        if self.language == "python":
            self.module_init_template = os.path.join(os.path.dirname(os.path.abspath(__file__)), "templates/__init__.py.in")
            if not os.path.isfile(self.module_init_template):
                raise Exception("Could not find __init__.py template: %s" % (self.module_init_template))
        self.files.append(os.path.join(self.ell_root, "CMake/OpenBLASSetup.cmake"))

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
                self.logger.info("copy \"%s\" \"%s\"" % (path, dest))
            copyfile(path, dest)

    def create_template_file(self, template_filename, output_filename):
        with open(template_filename) as f:
            template = f.read()
        
        template = template.replace("@ELL_outdir@", os.path.basename(self.output_dir))
        template = template.replace("@ELL_model@", self.model_file_base)
        template = template.replace("@ELL_model_name@", self.model_name)
        template = template.replace("@Arch@", self.target)
        template = template.replace("@OBJECT_EXTENSION@", self.objext)
        template = template.replace("@ELL_ROOT@", os.path.join(self.ell_root, "external").replace("\\","/"))
        output_template = os.path.join(self.output_dir, output_filename)
        with open(output_template, 'w') as f:
            f.write(template)

    def create_cmake_file(self):
        self.create_template_file(self.cmake_template, "CMakeLists.txt")

    def create_module_init_file(self):
        self.create_template_file(self.module_init_template, "__init__.py")

    def save_config(self):
        self.config['model'] = self.model_name
        self.config['func'] = self.model_name + "_" + self.func_name
        config_json = json.dumps(self.config, indent=2, sort_keys=True)
        _, tail = os.path.split(self.config_file)
        outputFile = os.path.join(self.output_dir, tail)
        self.logger.info("creating config file: '" + outputFile + "'")
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
            model_file=self.model_file, 
            func_name=self.func_name, 
            model_name=self.model_name, 
            target=self.target, 
            output_dir=self.output_dir, 
            use_blas=self.blas, 
            fuse_linear_ops=self.fuse_linear_ops, 
            profile=self.profile,
            llvm_format=self.llvm_format, 
            optimize=self.optimize, 
            debug=self.debug, 
            is_model_file=False, 
            swig=self.swig, 
            header=self.cpp_header,
            objext="." + self.objext,
            extra_options=self.compile_args
            )
        if self.swig:
            self.tools.swig(self.output_dir, self.model_file_base, self.language)
        if not self.no_opt_tool:
            out_file = self.tools.opt(self.output_dir, out_file, self.optimization_level)
        if not self.no_llc_tool:
            out_file = self.tools.llc(self.output_dir, out_file, self.target, self.optimization_level, "." + self.objext)
        self.create_cmake_file()
        if self.language == "python":
            self.create_module_init_file()
        if self.target == "host":
            self.logger.info("success, now you can build the '" + self.output_dir + "' folder")
        else:
            self.logger.info("success, now copy the '" + self.output_dir + "' folder to your target machine and build it there")

if __name__ == "__main__":
    logging.basicConfig(level=logging.INFO, format="%(message)s")
    builder = ModuleBuilder()
    builder.parse_command_line(sys.argv[1:])
    try:
        builder.run()
    except:
        errorType, value, traceback = sys.exc_info()
        msg = "### WrapException: %s: %s" % (str(errorType), str(value))
        logger.get().error(msg)
        sys.exit(1)
