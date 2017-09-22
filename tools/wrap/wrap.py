####################################################################################################
##
##  Project:  Embedded Learning Library (ELL)
##  File:     wrap.py
##  Authors:  Chris Lovett
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
        self.label_file = None
        self.config = None
        self.model_file = None
        self.output_dir = None
        self.files = []
        self.includes = []
        self.tcc = []
        self.tools = None
        self.func_name = "predict"
        self.objext = "obj"
        self.tools = None
        self.language = "python"
        self.verbose = False
        self.profile = False

    def parse_command_line(self, argv):
        arg_parser = argparse.ArgumentParser("This tool wraps a given ELL model in a CMake buildable project that builds a language\n"
            "specific module that can call the ELL model on a given target platform.\n"
            "\nThe supported languages are:\n"
            "    python   (default)\n"
            "\nThe supported target platforms are:\n"
            "    pi3      raspberry pi 3\n"
            "    aarch64  arm64 Linux, works on Qualcomm DragonBoards\n"
            "    host     (default) your host computer architecture\n")

        # required arguments
        arg_parser.add_argument("label_file", help="path to the labels file for the ELL model")
        arg_parser.add_argument("model_file", help="path to the ELL model file")

        # optional arguments
        arg_parser.add_argument("--language", "-lang", help="the language for the ELL module", choices=["python"], default=self.language)
        arg_parser.add_argument("--target", "-target", help="the target platform", choices=["pi3", "pi3_64", "aarch64", "host"], default=self.target)
        arg_parser.add_argument("--outdir", "-outdir", help="the output directory")
        arg_parser.add_argument("--profile", "-profile", help="enable profiling functions in the ELL module", action="store_true")
        arg_parser.add_argument("--verbose", "-v", help="print verbose output", action="store_true")

        args = arg_parser.parse_args(argv)
        
        self.label_file = args.label_file
        self.model_file = args.model_file

        self.language = args.language
        self.target = args.target
        self.output_dir = args.outdir
        if self.output_dir is None:
            self.output_dir  = self.target
        self.profile = args.profile
        self.verbose = args.verbose

        _, tail = os.path.split(self.model_file)
        self.model_name =  os.path.splitext(tail)[0]        

    def find_files(self):
        self.cmake_template = os.path.join(os.path.dirname(os.path.abspath(__file__)), "templates/CMakeLists.%s.txt.in" % (self.language))
        if (not os.path.isfile(self.cmake_template)):
            raise Exception("Could not find CMakeLists template: %s" % (self.cmake_template))
        self.files.append(self.label_file)
        self.files.append(os.path.join(self.ell_root, "CMake/OpenBLASSetup.cmake"))       
        self.includes.append(os.path.join(self.ell_root, "interfaces/common/include/CallbackInterface.h"))
        self.includes.append(os.path.join(self.ell_root, "libraries/emitters/include/ClockInterface.h"))
        self.tcc.append(os.path.join(self.ell_root, "interfaces/common/tcc/CallbackInterface.tcc"))

    def copy_files(self, list, folder):
        if (folder == ""):
            target_dir = self.output_dir
        else:
            target_dir = os.path.join(self.output_dir, folder)
        if (not os.path.isdir(target_dir)):
            os.mkdir(target_dir)
        for path  in list:
            if (not os.path.isfile(path)):
                raise Exception("expected file not found: " + path)
            head,file_name = os.path.split(path)
            dest = os.path.join(target_dir, file_name)
            if (self.verbose):
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
        self.config['func'] =  self.model_name + "_" + self.func_name
        head, tail = os.path.split(self.label_file)
        self.config['labels'] =  tail
        configJSon = json.dumps(self.config, indent=2, sort_keys=True)
        head, tail = os.path.split(self.config_file)
        outputFile = os.path.join(self.output_dir, tail)
        print("creating config file: '" + outputFile + "'")
        with open(outputFile, 'w') as f:
            f.write(configJSon)
            f.close()

    def run(self):
        self.build_root = find_ell.find_ell_build()
        self.ell_root = os.path.dirname(self.build_root)
        self.tools = buildtools.EllBuildTools(self.ell_root, self.verbose)
        self.find_files()
        self.copy_files(self.files, "")
        self.copy_files(self.includes, "include")
        self.copy_files(self.tcc, "tcc")
        self.create_cmake_file()
        self.tools.compile(self.model_file, self.func_name, self.model_name, self.target, self.output_dir, self.profile)
        self.tools.swig(self.output_dir, self.model_name, self.language)
        self.tools.opt(self.output_dir, self.model_name)
        self.tools.llc(self.output_dir, self.model_name, self.target)
        self.create_cmake_file()
        if (self.target == "host"):
            print("success, now you can build the '" + self.output_dir + "' folder")
        else:
            print("success, now copy the " + self.output_dir + " to your target machine and build it there")
        
if __name__ == "__main__":
    builder = ModuleBuilder()

    argv = sys.argv
    argv.pop(0) # when an args list is passed to parse_args, the first argument (program name) needs to be dropped

    builder.parse_command_line(argv)
    builder.run()