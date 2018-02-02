####################################################################################################
##
##  Project:  Embedded Learning Library (ELL)
##  File:     buildtools.py
##  Authors:  Chris Lovett, Kern Handa
##
##  Requires: Python 3.x
##
####################################################################################################
import json
import os
import sys
sys.path += [os.path.dirname(os.path.abspath(__file__)) ]
import logger
import subprocess
from threading import Thread, Lock

class EllBuildToolsRunException(Exception):
    def __init__(self, cmd, output=""):
        Exception.__init__(self, cmd)
        self.cmd = cmd
        self.output = output

class EllBuildTools:
    def __init__(self, ell_root, verbose = False):
        self.verbose = verbose
        self.ell_root = ell_root
        self.build_root = None
        self.compiler = None
        self.swigexe = None
        self.llcexe = None
        self.optexe = None
        self.blas = None
        self.logger = logger.get()
        self.output = None
        self.lock = Lock()
        self.find_tools()

    def get_ell_build(self):
        if not self.build_root:
            import find_ell
            self.build_root = find_ell.find_ell_build()
        return self.build_root

    def find_tools(self):
        build_root = self.get_ell_build()
        if not os.path.isdir(build_root):
            raise Exception("Could not find '%s', please make sure to build the ELL project first" % (build_root))

        ell_tools_json = "ell_build_tools.json"
        jsonPath = os.path.join(build_root, ell_tools_json)
        if not os.path.isfile(jsonPath):
            raise Exception("Could not find build output: " + jsonPath)

        with open(jsonPath) as f:
            self.tools = json.loads(f.read())

        self.compiler = self.tools['compile']
        if self.compiler == "":
            raise Exception(ell_tools_json + " is missing compiler info")

        self.swigexe = self.tools['swig']
        if self.swigexe == "":
            raise Exception(ell_tools_json + " is missing swig info")

        self.llcexe = self.tools['llc']
        if self.llcexe == "":
            raise Exception(ell_tools_json + " is missing llc info")

        self.optexe = self.tools['opt']
        if self.optexe == "":
            raise Exception(ell_tools_json + " is missing opt info")

        if ("blas" in self.tools):
            self.blas = self.tools['blas']  # this one can be empty.

    def logstream(self, stream):
        try:
            while True:
                out = stream.readline()
                if out:
                    self.lock.acquire()
                    try:
                        self.output += out
                        msg = out.rstrip('\n')   
                        if self.verbose:                
                            self.logger.info(msg)
                    finally:
                        self.lock.release()
                else:
                    break
        except:
            errorType, value, traceback = sys.exc_info()
            msg = "### Exception: %s: %s" % (str(errorType), str(value))
            if not "closed file" in msg:
                self.logger.info(msg)

    def run(self, command, print_output=True, shell=False):
        cmdstr = command if isinstance(command, str) else " ".join(command)
        if self.verbose:
            self.logger.info(cmdstr)
        try:
            with subprocess.Popen(
                command, stdout=subprocess.PIPE, stderr=subprocess.PIPE, bufsize=0, universal_newlines = True, shell=shell
            ) as proc:
                self.output = ''
                    
                stdout_thread = Thread(target=self.logstream, args=(proc.stdout,))
                stderr_thread = Thread(target=self.logstream, args=(proc.stderr,))

                stdout_thread.start()
                stderr_thread.start()

                while stdout_thread.isAlive() or stderr_thread.isAlive():
                    pass

                proc.wait()
                
                if proc.returncode:
                    self.logger.error("command {} failed with error code {}".format(command[0], proc.returncode))
                    raise EllBuildToolsRunException(cmdstr, self.output)
                return self.output
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
        self.logger.info("generating " + language + " interfaces for " + model_name + " in " + output_dir)
        return self.run(args)

    def get_llc_options(self, target):
        common = ["-filetype=obj"]
        # arch processing
        if target == "pi3": # Raspberry Pi 3
            return common + ["-mtriple=armv7-linux-gnueabihf", "-mcpu=cortex-a53", "-relocation-model=pic"]
        if target == "orangepi0": # Orange Pi Zero
            return common + ["-mtriple=armv7-linux-gnueabihf", "-mcpu=cortex-a7", "-relocation-model=pic"]
        elif target == "pi0": # Raspberry Pi Zero
            return common + ["-mtriple=arm-linux-gnueabihf", "-mcpu=arm1176jzf-s", "-relocation-model=pic"]
        elif target == "aarch64" or target == "pi3_64": # arm64 Linux
            return common + ["-mtriple=aarch64-unknown-linux-gnu", "-relocation-model=pic"]
        else: # host
            return common + ["-relocation-model=pic"]

    def llc(self, output_dir, input_file, target, optimization_level="3"):
        # llc -filetype=obj _darknetReference.ll -O3 -mtriple=armv7-linux-gnueabihf -mcpu=cortex-a53 -relocation-model=pic
        model_name = os.path.splitext(os.path.basename(input_file))[0]
        if model_name.endswith('.opt'):
            model_name = model_name[:-4]
        out_file = os.path.join(output_dir, model_name + ".obj")
        args = [self.llcexe,
                input_file,
                "-o", out_file,
                "-O" + optimization_level
                ]
        args = args + self.get_llc_options(target)
        self.logger.info("running llc ...")
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
        self.logger.info("running opt ...")
        self.run(args)
        return out_file

    def compile(self, model_file, func_name, model_name, target, output_dir, use_blas=False, fuse_linear_ops=True, profile=False, llvm_format="bc",
                optimize=True, debug=False, is_model_file=False, swig=True, header=False):
        file_arg = "-imf" if is_model_file else "-imap"
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
        model_file_base = os.path.splitext(os.path.basename(model_file))[0]
        out_file = os.path.join(output_dir, model_file_base + output_ext)
        args = [self.compiler,
                file_arg, model_file,
                "-cfn", func_name,
                "-cmn", model_name,
                format_flag,
                "--target", target,
                "-od", output_dir,
                "--fuseLinearOps", str(fuse_linear_ops)
                ]
        if swig:
            args.append("--swig")
        if header:
            args.append("--header")
        args.append("--blas")
        hasBlas = bool(use_blas)
        if target == "host" and hasBlas and not self.blas:
            hasBlas = False
        args.append(str(hasBlas).lower())

        if not optimize:
            args += ["--optimize", "false"]
        else:
            args += ["--optimize", "true"]
        if debug:
            args += ["--debug", "true"]

        if profile:
            args.append("--profile")

        self.logger.info("compiling model...")
        self.run(args)
        return out_file

