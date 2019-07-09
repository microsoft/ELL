####################################################################################################
#
#  Project:  Embedded Learning Library (ELL)
#  File:     buildtools.py
#  Authors:  Chris Lovett, Kern Handa
#
#  Requires: Python 3.x
#
####################################################################################################
import json
import os
import sys
import subprocess
from threading import Thread, Lock

sys.path += [os.path.dirname(os.path.abspath(__file__))]
import logger
import find_ell

ELL_TOOLS_JSON = "ell_build_tools.json"


def get_build_tool_locations():
    build_root = find_ell.find_ell_build()
    jsonPath = os.path.join(build_root, ELL_TOOLS_JSON)
    if not os.path.isfile(jsonPath):
        raise Exception("Could not find build output: " + jsonPath)

    with open(jsonPath) as f:
        return json.loads(f.read())


class EllBuildToolsRunException(Exception):
    def __init__(self, cmd, output=""):
        Exception.__init__(self, cmd)
        self.cmd = cmd
        self.output = output


class EllBuildTools:
    def __init__(self, ell_root):
        self.ell_root = ell_root
        self.build_root = None
        self.compiler = None
        self.swigexe = None
        self.llcexe = None
        self.optexe = None
        self.blas = None
        self.logger = logger.get()
        self.verbose = self.logger.getVerbose()
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

        self.tools = get_build_tool_locations()

        self.compiler = self.tools['compile']
        if self.compiler == "":
            raise Exception(ELL_TOOLS_JSON + " is missing compiler info")

        self.swigexe = self.tools['swig']
        if self.swigexe == "":
            raise Exception(ELL_TOOLS_JSON + " is missing swig info")

        self.llcexe = self.tools['llc']
        if self.llcexe == "":
            raise Exception(ELL_TOOLS_JSON + " is missing llc info")

        self.optexe = self.tools['opt']
        if self.optexe == "":
            raise Exception(ELL_TOOLS_JSON + " is missing opt info")

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
                        msg = out.rstrip('\r\n')
                        if self.verbose:
                            self.logger.info(msg)
                    finally:
                        self.lock.release()
                else:
                    break
        except:
            errorType, value, traceback = sys.exc_info()
            msg = "### Exception: %s: %s" % (str(errorType), str(value))
            if "closed file" not in msg:
                self.logger.info(msg)

    def run(self, command, print_output=True, shell=False):
        cmdstr = command if isinstance(command, str) else " ".join(command)
        if self.verbose:
            self.logger.info(cmdstr)
        try:
            with subprocess.Popen(command, stdout=subprocess.PIPE, stderr=subprocess.PIPE, bufsize=0,
                                  universal_newlines=True, shell=shell) as proc:
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
                'libraries/emitters/include']]

    def swig(self, output_dir, model_name, language):
        args = [self.swigexe,
                '-' + language,
                '-c++',
                '-Fmicrosoft']
        if language == "python":
            args += ["-py3"]
        if language == "javascript":
            args += ["-v8"]
        if os.name == 'nt':
            args += ["-DWIN32"]
        args = args + ['-outdir', output_dir] + ['-I' + d for d in self.swig_header_dirs()] + [
            '-o', os.path.join(output_dir, model_name + language.upper() + '_wrap.cxx'),
            os.path.join(output_dir, model_name + ".i")
        ]
        self.logger.info("generating " + language + " interfaces for " + model_name + " in " + output_dir)
        return self.run(args)

    def get_llc_options(self, target):
        common = ["-filetype=obj"]
        # arch processing
        if target == "pi3":  # Raspberry Pi 3
            return common + ["-mtriple=armv7-linux-gnueabihf", "-mcpu=cortex-a53", "-relocation-model=pic"]
        if target == "orangepi0":  # Orange Pi Zero
            return common + ["-mtriple=armv7-linux-gnueabihf", "-mcpu=cortex-a7", "-relocation-model=pic"]
        elif target == "pi0":  # Raspberry Pi Zero
            return common + ["-mtriple=arm-linux-gnueabihf", "-mcpu=arm1176jzf-s", "-relocation-model=pic"]
        elif target == "aarch64" or target == "pi3_64":  # arm64 Linux
            return common + ["-mtriple=aarch64-unknown-linux-gnu", "-relocation-model=pic"]
        else:  # host
            return common + ["-relocation-model=pic"]

    def log_command_arguments(self, args, log_dir):
        # Log the command executable and arguments. Filename is the name of the executable without
        # an extension + ".log".
        filename = os.path.join(log_dir, os.path.basename(os.path.splitext(args[0])[0]) + ".log")
        with open(filename, "w") as f:
            f.write(" ".join(args))

    def llc(self, output_dir, input_file, target, optimization_level="3", objext=".o"):
        model_name = os.path.splitext(os.path.basename(input_file))[0]
        if model_name.endswith('.opt'):
            model_name = model_name[:-4]
        out_file = os.path.join(output_dir, model_name + objext)
        args = [self.llcexe,
                input_file,
                "-o", out_file,
                "-O" + optimization_level
                ]
        args = args + self.get_llc_options(target)
        # Save the parameters passed to llc. This is used for archiving purposes.
        self.log_command_arguments(args, log_dir=output_dir)

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
                "-O" + optimization_level]
        # Save the parameters passed to opt. This is used for archiving purposes.
        self.log_command_arguments(args, log_dir=output_dir)

        self.logger.info("running opt ...")
        self.run(args)
        return out_file

    def compile(self, model_file, func_name, model_name, target, output_dir,
                use_blas=False, fuse_linear_ops=True, optimize_reorder_data_nodes=True, profile=False, llvm_format="bc",
                optimize=True, parallelize=True, vectorize=True, debug=False, is_model_file=False, swig=True,
                header=False, objext=".o", extra_options=[]):
        file_arg = "-imf" if is_model_file else "-imap"
        format_flag = {
            "bc": "--bitcode",
            "ir": "--ir",
            "asm": "--assembly",
            "obj": "--objectCode"
        }[llvm_format]
        output_ext = {
            "bc": ".bc",
            "ir": ".ll",
            "asm": ".s",
            "obj": objext
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
                "--fuseLinearOps", str(fuse_linear_ops),
                "--optimizeReorderDataNodes", str(optimize_reorder_data_nodes)
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
            if parallelize:
                args += ["--parallelize", "true"]
            if vectorize:
                args += ["--vectorize"]
        if debug:
            args += ["--debug", "true"]

        if profile:
            args.append("--profile")

        args += extra_options

        # Save the parameters passed to compile. This is used for archiving purposes.
        self.log_command_arguments(args, log_dir=output_dir)

        self.logger.info("compiling model...")
        self.run(args)
        return out_file
