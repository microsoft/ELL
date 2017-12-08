####################################################################################################
##
##  Project:  Embedded Learning Library (ELL)
##  File:     buildtools.py
##  Authors:  Chris Lovett, Kern Handa
##
##  Requires: Python 3.x
##
####################################################################################################
import subprocess
import os
import json

class EllBuildToolsRunException(Exception):
    def __init__(self, cmd, output=""):
        Exception.__init__(self)
        self.cmd = cmd
        self.output = output

class EllBuildTools:
    def __init__(self, ell_root, verbose = False):
        self.verbose = verbose
        self.ell_root = ell_root
        self.build_root = os.path.join(self.ell_root, "build")
        self.compiler = None
        self.swigexe = None
        self.llcexe = None
        self.optexe = None
        self.blas = None
        self.find_tools()

    def find_tools(self):
        if not os.path.isdir(self.build_root):
            raise Exception("Could not find '%s', please make sure to build the ELL project first" % (self.build_root))

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

        self.optexe = self.tools['opt']
        if self.optexe == "":
            raise Exception("tools.json is missing opt info")

        if ("blas" in self.tools):
            self.blas = self.tools['blas']  # this one can be empty.

    def run(self, command, print_output=True, shell=False):
        cmdstr = command if isinstance(command, str) else " ".join(command)
        if self.verbose:
            print(cmdstr, flush=True)
        try:
            with subprocess.Popen(
                command, stdout=subprocess.PIPE, stderr=subprocess.PIPE, bufsize=0, universal_newlines = True, shell=shell
            ) as proc:
                output = ''
                for line in proc.stdout:
                    output += line
                    if print_output or self.verbose:
                        print(line.strip("\n"), flush=True)
                for line in proc.stderr:
                    output += line
                    if print_output or self.verbose:
                        print(line.strip("\n"), flush=True)
                if proc.returncode:
                    raise EllBuildToolsRunException(cmdstr, output)
                return output
        except FileNotFoundError:
            raise EllBuildToolsRunException(cmdstr)

    def swig_header_dirs(self):
        return [os.path.join(self.ell_root, d) for d in [
            'interfaces/common',
            'interfaces/common/include',
            'libraries/emitters/include'
            ]]

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
        args = args + ['-outdir', output_dir] + ['-I' + d for d in self.swig_header_dirs()] + [
            '-o', os.path.join(output_dir, model_name + language.upper() + '_wrap.cxx'),
            os.path.join(output_dir, model_name + ".i")
        ]
        print("generating " + language + " interfaces for " + model_name + " in " + output_dir)
        return self.run(args)

    def get_llc_options(self, target):
        common = ["-filetype=obj"]
        # arch processing
        if target == "pi3": # Raspberry Pi 3
            return common + ["-mtriple=armv7-linux-gnueabihf", "-mcpu=cortex-a53", "-relocation-model=pic"]
        if target == "orangepi0": # Orange Pi Zero
            return common + ["-mtriple=armv7-linux-gnueabihf", "-mcpu=cortex-a7", "-relocation-model=pic"]
        elif target == "pi0": # Raspberry Pi Zero
            return common + ["-mtriple=arm-linux-gnueabihf", "-relocation-model=pic"]
        elif target == "aarch64" or target == "pi3_64": # arm64 Linux
            return common + ["-mtriple=aarch64-unknown-linux-gnu", "-relocation-model=pic"]
        else: # host
            return common + ["-relocation-model=pic"]

    def llc(self, output_dir, input_file, target, optimization_level="3"):
        # llc -filetype=obj _darknetReference.ll -O3 -mtriple=armv7-linux-gnueabihf -mcpu=cortex-a53 -relocation-model=pic
        model_name = os.path.splitext(os.path.basename(input_file))[0]
        out_file = os.path.join(output_dir, model_name + ".obj")
        args = [self.llcexe,
                input_file,
                "-o", out_file,
                "-O" + optimization_level
                ]
        args = args + self.get_llc_options(target)
        print("running llc...")
        self.run(args)
        return out_file

    def opt(self, output_dir, input_file, optimization_level="3"):
        # opt compiled_model.ll -o compiled_model_opt.ll -O3
        model_name = os.path.splitext(os.path.basename(input_file))[0]
        out_file = os.path.join(output_dir, model_name + ".opt.bc")
        args = [self.optexe,
                input_file,
                "-o", out_file,
                "-O" + optimization_level
            ]
        print("running opt...")
        self.run(args)
        return out_file

    def compile(self, model_file, func_name, model_name, target, output_dir, useBlas=False, profile=False, fuseLinearOps=True, llvm_format="bc",
                optimize=True, debug=False):
        format_flag = {
            "bc": "--bitcode",
            "ir": "--ir",
            "asm": "--assembly"
        }[llvm_format]
        output_ext = {
            "bc": ".bc",
            "ir": ".ll",
            "asm": ".s"
        }[llvm_format]
        out_file = os.path.join(output_dir, model_name + output_ext)
        args = [self.compiler,
                "-imap",
                model_file,
                "-cfn", func_name,
                "-cmn", model_name,
                format_flag,
                "--swig",
                "--target", target,
                "-od", output_dir,
                "--fuseLinearOps", str(fuseLinearOps)
                ]
        args.append("--blas")
        if not optimize:
            args += ["--optimize", "false"]
        if debug:
            args += ["--debug", "true"]
        hasBlas = bool(useBlas)
        if target == "host" and hasBlas and not self.blas:
            hasBlas = False
        args.append(str(hasBlas).lower())

        if profile:
            args.append("--profile")

        print("compiling model...")
        self.run(args)
        return out_file

