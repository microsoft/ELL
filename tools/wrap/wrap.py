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
        self.condig = None
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

    def print_usage(self):
        print("Usage: wrap.py label_file model_file [-lang name|-target name|-outdir name|-v]")
        print("This tool wraps a given ELL model in a CMake buildable project that builds a language")
        print("specific module that can call the ELL model on a given target platform.")
        print("")
        print("The supported languages are:")
        print("    python   (default)")
        print("")
        print("he supported target platforms are:")
        print("    pi3      raspberry pi 3")
        print("    aarch64  arm64 Linux, works on Qualcomm DragonBoards")
        print("    host     (default) your host computer architecture")
        

    def parse_command_line(self, argv):
        i = 1
        while (i < len(argv)):
            arg = argv[i]
            if (arg[0] == "-"):
                option = arg[1:]
                if (option.lower() == "target"):
                    i = i + 1
                    self.target = argv[i].lower()
                elif (option.lower() == "outdir"):
                    i = i + 1
                    self.output_dir = argv[i]
                elif (option.lower() == "lang"):
                    i = i + 1
                    self.language = argv[i].lower()
                elif (option.lower() == "v"):
                    self.verbose = True
                else:
                    print("Unknown option: " + arg)
                    return False                
            elif (self.label_file == None):
                self.label_file = arg
            elif (self.model_file == None):
                self.model_file = arg
            else:
                print("Too many arguments")
                return False
            i = i + 1

        if (self.model_file == None):
            print("missing model file argument")
            return False  
        if (self.label_file == None):
            print("missing label file argument")
            return False        

        if (self.output_dir == None):
            self.output_dir = self.target

        head, tail = os.path.split(self.model_file)
        self.model_name =  os.path.splitext(tail)[0]        
        
        return True

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
        self.tools.compile(self.model_file, self.func_name, self.model_name, self.target, self.output_dir)
        self.tools.swig(self.output_dir, self.model_name, self.language)
        self.tools.llc(self.output_dir, self.model_name, self.target)
        self.create_cmake_file()
        if (self.target == "host"):
            print("success, now you can build the '" + self.output_dir + "' folder")
        else:
            print("success, now copy the " + self.output_dir + " to your target machine and build it there")
        
if __name__ == "__main__":
    builder = ModuleBuilder()
    if (not builder.parse_command_line(sys.argv)):
        builder.print_usage()
    else:
        builder.run()