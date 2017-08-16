####################################################################################################
##
##  Project:  Embedded Learning Library (ELL)
##  File:     make_python_model.py
##  Authors:  Chris Lovett
##
##  Requires: Python 3.x
##
####################################################################################################
import os
import sys
import find_ell
import subprocess
import json
import operator
from shutil import copyfile

# This script creates a compilable Python project for executing a given ELL model on a target platform.
# Compilation of the resulting project will require a C++ compiler.

class ModuleBuilder:
    def __init__(self):
        self.target = "host"
        self.config_file = None
        self.label_file = None
        self.condig = None
        self.model_file = None
        self.llc_options = None
        self.compile_target_name = None
        self.output_dir = None
        self.files = []
        self.includes = []
        self.tcc = []
        self.tools = None
        self.func_name = "predict"
        self.objext = "obj"

    def print_usage(self):
        print("Usage: make_python_project.py config_file label_file model_file [-target name -outdir name]")
        print("This tool creates a folder containing a CMake buildable project for executing a specific")
        print("ELL model on a given target platform where the supported target platforms are:")
        print("    pi3      raspberry pi 3")
        print("    aarch64  arm64 Linux, works on Qualcomm Dragon boards")
        print("    host     your host computer architecture")
        

    def parse_command_line(self, argv):
        i = 1
        while (i < len(argv)):
            arg = argv[i]
            if (arg[0] == "-" or arg[0] == "/"):
                option = arg[1:]
                if (option.lower() == "target"):
                    i = i + 1
                    self.target = argv[i]
                elif (option.lower() == "outdir"):
                    i = i + 1
                    self.output_dir = argv[i]
                else:
                    print("Unknown option: " + arg)
                    return False                
            elif (self.config_file == None):
                self.config_file = arg
            elif (self.label_file == None):
                self.label_file = arg
            elif (self.model_file == None):
                self.model_file = arg
            else:
                print("Too many arguments")
                return False
            i = i + 1

        if (self.config_file == None):
            print("missing config file argument")
            return False  
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
        
        with open(self.config_file) as f:
            self.config = json.loads(f.read())

        return True

    def set_target(self):
        # arch processing
        if(self.target == "pi3"): # Raspberry Pi 3
            print("targetting raspberry pi 3")
            self.llc_options = "-O3 -mtriple=armv7-linux-gnueabihf -mcpu=cortex-a53 -relocation-model=pic"
            self.compile_target_name = "pi3"
        elif(self.target == "pi0"): # Raspberry Pi Zero
            print("targetting raspberry pi zero")
            self.llc_options = "-O3 -mtriple=arm-linux-gnueabihf -relocation-model=pic"
            self.compile_target_name = "pi0"
        elif(self.target == "aarch64"): # arm64 Linux
            print("targetting arm64 Linux")
            self.llc_options = "-O3 -mtriple=aarch64-unknown-linux-gnu -relocation-model=pic"
            self.compile_target_name = "aarch64"
        else: # host
            print("targetting this host machine")
            self.llc_options = "-O3 -relocation-model=pic"
            self.compile_target_name = "host"
        
    def find_files(self):
        self.cmake_template = os.path.join(self.ell_root, "tutorials/vision/gettingStarted/CMake/CMakeLists.txt.in")
        self.files.append(self.label_file)
        self.files.append(os.path.join(self.ell_root, "CMake/OpenBLASSetup.cmake"))
        self.files.append(os.path.join(self.ell_root, "tools/utilities/pythonlibs/demo.py"))
        self.files.append(os.path.join(self.ell_root, "tools/utilities/pythonlibs/demoHelper.py"))        
        self.includes.append(os.path.join(self.ell_root, "interfaces/common/include/CallbackInterface.h"))
        self.includes.append(os.path.join(self.ell_root, "libraries/emitters/include/ClockInterface.h"))
        self.tcc.append(os.path.join(self.ell_root, "interfaces/common/tcc/CallbackInterface.tcc"))

    def copy_files(self, list, folder):
        target_dir = os.path.join(self.output_dir, folder)
        if (not os.path.isdir(target_dir)):
            os.mkdir(target_dir)
        for path  in list:
            if (not os.path.isfile(path)):
                raise Exception("expected file not found: " + path)
            head,file_name = os.path.split(path)
            dest = os.path.join(target_dir, file_name)
            copyfile(path, dest)

    def compile(self):
        args = [self.compiler, "-imap", self.model_file, "-cfn", self.func_name, "-cmn", self.model_name, "--bitcode", "--swig", "--blas", "--target", self.target, "-od", self.output_dir]
        print("compiling model...")
        result = subprocess.run(args, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
        if (not result.returncode == 0):
            raise Exception("SWIG failed: " + result.stdout.decode("utf-8")  + result.stderr.decode("utf-8") )
    
    def make_swig(self):
        swig_options = "-python -c++ -Fmicrosoft -py3"
        swig_output = os.path.join(self.output_dir, self.model_name + "PYTHON_wrap.cxx")
        swig_includes = [ os.path.join(self.ell_root, "interfaces/common/include"), 
                          os.path.join(self.ell_root, "interfaces/common"), 
                          os.path.join(self.ell_root, "libraries/emitters/include") ]
        
        args = [self.swig, "-python", "-c++", "-Fmicrosoft", "-py3", "-o", swig_output ]
        for i in swig_includes:
            args.append("-I" + i)
        args.append("-outdir")
        args.append(self.output_dir)
        args.append(os.path.join(self.output_dir, self.model_name + ".i"))
        print("generating swig wrappers...")
        result = subprocess.run(args, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
        if (not result.returncode == 0):
            raise Exception("SWIG failed: " + result.stdout.decode("utf-8")  + result.stderr.decode("utf-8") )

    def run_llc(self):
        objFile = os.path.join(self.output_dir, self.model_name + ".obj")
        bitCode = os.path.join(self.output_dir, self.model_name + ".bc")
        args = [self.llc, bitCode, "-o", objFile, "-filetype=obj"]
        args = args + self.llc_options.split(" ")
        print("targetting model for platform: " + self.target)
        result = subprocess.run(args, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
        if (not result.returncode == 0):
            raise Exception("LLC failed: " + result.stdout.decode("utf-8")  + result.stderr.decode("utf-8") )
        os.remove(bitCode) # doesn't need to be copied to the device
    
    def find_tools(self):
        jsonPath = os.path.join(self.build_root, "tools/project_builder/tools.json")
        if (not os.path.isfile(jsonPath)):
            raise Exception("Could not find build output: " + jsonPath)

        with open(jsonPath) as f:
            self.tools = json.loads(f.read())

        self.compiler = self.tools['compile']
        if (self.compiler == ""):
            raise Exception("tools.json is missing compiler info")

        self.swig = self.tools['swig']
        if (self.swig == ""):
            raise Exception("tools.json is missing swig info")

        self.llc = self.tools['llc']
        if (self.llc == ""):
            raise Exception("tools.json is missing llc info:")

    def create_cmake_file(self):
        with open(self.cmake_template) as f:
            template = f.read()
        
        template = template.replace("@ELL_model@", self.model_name)
        template = template.replace("@Arch@", self.target)
        template = template.replace("@OBJECT_EXTENSION@", self.objext)
        # and so this project can be built from anywhere on your host machine
        template = template.replace("${CMAKE_SOURCE_DIR}/../../../../../external", os.path.join(self.ell_root, "external").replace("\\","/"))
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
        self.set_target()
        self.build_root = find_ell.find_ell_build()
        self.ell_root = os.path.dirname(self.build_root)
        self.find_tools()
        self.find_files()
        self.copy_files(self.files, "")
        self.copy_files(self.includes, "include")
        self.copy_files(self.tcc, "tcc")
        self.create_cmake_file();
        self.compile()
        self.make_swig()
        self.run_llc()
        self.save_config()
        self.create_cmake_file()
        print("success, now copy the " + self.output_dir + " to your target machine and build it there")
        
if __name__ == "__main__":
    builder = ModuleBuilder()
    if (not builder.parse_command_line(sys.argv)):
        builder.print_usage()
    else:
        builder.run()