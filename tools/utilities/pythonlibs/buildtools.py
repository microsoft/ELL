####################################################################################################
##
##  Project:  Embedded Learning Library (ELL)
##  File:     buildtools.py
##  Authors:  Chris Lovett
##
##  Requires: Python 3.x
##
####################################################################################################
import subprocess
import os
import json

class EllBuildTools:
    def __init__(self, ell_root, verbose = False):
        self.verbose = verbose
        self.ell_root = ell_root
        self.build_root = os.path.join(self.ell_root, "build")
        self.compiler = None
        self.swigexe = None
        self.llcexe = None
        self.blas = None
        if not os.path.isdir(self.build_root):
            raise Exception("Could not find '%s', please make sure to build the ELL project first" % (self.build_root))
        self.find_tools()

    def find_tools(self):
        jsonPath = os.path.join(self.build_root, "tools/tools.json")
        if not os.path.isfile(jsonPath):
            raise Exception("Could not find build output: " + jsonPath)

        with open(jsonPath) as f:
            self.tools = json.loads(f.read())

        self.compiler = self.tools['compile']
        if self.compiler == "":
            raise Exception("tools.json is missing compiler info")

        self.swigexe = self.tools['swig']
        if self.swigexe == "":
            raise Exception("tools.json is missing swig info")

        self.llcexe = self.tools['llc']
        if self.llcexe == "":
            raise Exception("tools.json is missing llc info")
        
        if ("blas" in self.tools):
            self.blas = self.tools['blas']  # this one can be empty.

    def run(self, command, print_output=True):
        if (self.verbose):
            print(" ".join(command))
        proc = subprocess.Popen(command, stdout=subprocess.PIPE, stderr=subprocess.PIPE, bufsize=0, universal_newlines = True)
        output = ''
        for line in proc.stdout:
            output += line
            if print_output or verbose:
                print(line.strip("\n"))
        for line in proc.stderr:
            output += line
            if print_output or verbose:
                print(line.strip("\n"))
        proc.wait()
        if not proc.returncode == 0:
            raise Exception(command[0] + " failed: " + output)        
        return output

    def swig(self, output_dir, model_name, language):
        # swig -python -modern -c++ -Fmicrosoft -py3 -outdir . -c++ -I%ELL_ROOT%/interfaces/common/include -I%ELL_ROOT%/interfaces/common -I%ELL_ROOT%/libraries/emitters/include -o _darknetReferencePYTHON_wrap.cxx darknetReference.i
        args = [self.swigexe,
            '-' + language,
            '-c++',
            '-Fmicrosoft']
        if language == "python":
            args = args + ["-py3"]
        if language == "javascript":
            args = args + ["-v8"]
        args = args + ['-outdir', output_dir,
            '-I' + os.path.join(self.ell_root, 'interfaces/common'),
            '-I' + os.path.join(self.ell_root, 'interfaces/common/include'),
            '-I' + os.path.join(self.ell_root, 'libraries/emitters/include'),
            '-o', os.path.join(output_dir, model_name + language.upper() + '_wrap.cxx'),
            os.path.join(output_dir, model_name + ".i")
        ]
        print("generating " + language + " interfaces for " + model_name + " in " + output_dir)
        self.run(args)
    
    def get_llc_options(self, target):        
        common = ["-filetype=obj", "-O3"]
        # arch processing
        if target == "pi3": # Raspberry Pi 3
            return common + ["-mtriple=armv7-linux-gnueabihf", "-mcpu=cortex-a53", "-relocation-model=pic"]
        elif target == "pi0": # Raspberry Pi Zero
            return common + ["-mtriple=arm-linux-gnueabihf", "-relocation-model=pic"]
        elif target == "aarch64" or target == "pi3_64": # arm64 Linux
            return common + ["-mtriple=aarch64-unknown-linux-gnu", "-relocation-model=pic"]
        else: # host
            return common + ["-relocation-model=pic"]
        
    def llc(self, output_dir, model_name, target):
        # llc -filetype=obj _darknetReference.ll -O3 -mtriple=armv7-linux-gnueabihf -mcpu=cortex-a53 -relocation-model=pic
        args = [self.llcexe, 
                os.path.join(output_dir, model_name + ".bc"), 
                "-o", os.path.join(output_dir, model_name + ".obj"), 
                ]
        args = args + self.get_llc_options(target)
        print("running llc...")
        self.run(args)

    def compile(self, model_file, func_name, model_name, target, output_dir, profile=False):
        args = [self.compiler, 
                "-imap", 
                model_file,
                "-cfn", func_name, 
                "-cmn", model_name, 
                "--bitcode", 
                "--swig", 
                "--target", target, 
                "-od", output_dir,
                ]
        args.append("--blas")
        hasBlas = target != "host" or (self.blas is not None and self.blas != "")
        args.append(str(hasBlas).lower())

        if profile:
            args.append("--profile")

        print("compiling model...")
        self.run(args)
    