####################################################################################################
##
##  Project:  Embedded Learning Library (ELL)
##  File:     drivetest.py
##  Authors:  Chris Lovett
##
##  Requires: Python 3.x
##
####################################################################################################

import os
from os.path import basename
import sys
import argparse
import glob
import subprocess
import json
import operator
from shutil import copyfile
from shutil import rmtree
import zipfile
import socket
import time

import paramiko
import requests

current_script = os.path.basename(__file__)

# this script may be called from a different location, so we need the path relative to it
current_path = os.path.dirname(os.path.abspath(__file__))
sys.path.append(os.path.join(current_path, "../pythonlibs"))
import find_ell
import picluster
from download_helper import download_file, download_and_extract_model
from remoterunner import RemoteRunner

class DriveTest:
    def __init__(self):
        self.ipaddress = None
        self.build_root = find_ell.find_ell_build()
        self.ell_root = os.path.dirname(self.build_root)
        self.test_dir = os.path.join(self.build_root, "test", "pitest")        
        self.output_dir = None
        self.target_dir = "/home/pi/pi3"
        self.model_name = None
        self.labels_file = None
        self.ell_model = None
        self.ell_json = None
        self.username = "pi"
        self.password = "raspberry"
        self.target = "pi3"
        self.machine = None
        self.cluster = None
        self.blas = True
        self.expression = None
        self.created_dirs = []
        self.profile = False
        self.test = False
        if os.path.isdir(self.test_dir):
            rmtree(self.test_dir)
        os.makedirs(self.test_dir)

    def __enter__(self):
        """Called when this object is instantiated with 'with'"""
        return self

    def __exit__(self, exc_type, exc_value, traceback):
        """Called on cleanup of this object that was instantiated with 'with'"""
        self.cleanup()

    def parse_command_line(self, argv):
        """Parses command line arguments"""
        
        self.arg_parser = argparse.ArgumentParser(
            "This script uses ELL to create a demo project for a model (default is d_I160x160x3CMCMCMCMCMCMC1AS from the ELL gallery)\n"
            "on a target device (default is Raspberry Pi 3), pushes it to the given\n"
            "device's ip address using ssh and scp, then executes the test.\n"
            "The test also measures the accuracy and performance of evaluating the model.\n")

        # options
        self.arg_parser.add_argument("--ipaddress", default=None, help="IP address of the target devices")
        self.arg_parser.add_argument("--cluster", default=None, help="http address of the cluster server that controls access to the target devices")
        self.arg_parser.add_argument("--outdir", default=self.test_dir)
        self.arg_parser.add_argument("--profile", help="enable profiling functions in the ELL module", action="store_true")

        model_group = self.arg_parser.add_argument_group("model", "options for loading a non-default model. All 3 must be specified for a non-default model to be used.")
        model_group.add_argument("--model", help="path to an ELL model file, the filename (without extension) will be used as the model name")
        model_group.add_argument("--labels", help="path to the labels file for evaluating the model")

        self.arg_parser.add_argument("--target", help="the target platform.\n"
            "Choices are pi3 (Raspberry Pi 3) and aarch64 (Dragonboard)", choices=["pi3", "pi3_64", "aarch64"], default=self.target)
        self.arg_parser.add_argument("--target_dir", help="the directory on the target device for running the test", default=self.target_dir)
        self.arg_parser.add_argument("--username", help="the username for the target device", default=self.username)
        self.arg_parser.add_argument("--password", help="the password for the target device", default=self.password)
        self.arg_parser.add_argument("--iterations", "-i", type=int, help="the number of iterations for each predict (default 1)", default=1)
        self.arg_parser.add_argument("--expression", "-e", help="the string to search for to verify test passed (default 'coffee mug')", default="coffee mug")
        self.arg_parser.add_argument("--blas", help="enable or disable the use of Blas on the target device (default 'True')", default="True")
        self.arg_parser.add_argument("--test", help="test only, assume the outdir has already been built (default 'False')", action="store_true")

        args = self.arg_parser.parse_args(argv)

        self._init(args)

    def cleanup(self):
        """Unlocks the target device if it is part of a cluster"""
        if self.machine:
            print("Unlocking machine: " + self.machine.ip_address)
            f = self.cluster.unlock(self.machine.ip_address)
            if f.current_user_name:
                print("Failed to free the machine at " + self.machine.ip_address)
            else:
                print("Freed machine at " + self.machine.ip_address)

        print("Exiting DriveTest.cleanup")
        
    def str2bool(self, v):
        """Converts a string to a bool"""
        return v.lower() in ("yes", "true", "t", "1")

    def _init(self, args):
        self.test_dir = os.path.abspath(args.outdir)
        self.profile = args.profile
        self.target = args.target
        self.target_dir = args.target_dir
        self.username = args.username
        self.password = args.password
        self.iterations = args.iterations
        self.expression = args.expression
        self.blas = self.str2bool(args.blas)
        self.test = args.test
        self.extract_model_info(args.model, args.labels)
        self.output_dir = os.path.join(self.test_dir, self.target, self.model_name)
        self.resolve_address(args.ipaddress, args.cluster)
        if not os.path.isdir(self.output_dir):
            os.makedirs(self.output_dir)

    def resolve_address(self, ipaddress, cluster):
        """Resolves the ip address of the target device and locks it if it is part of a cluster"""
        if cluster:
            self.cluster = picluster.PiBoardTable(cluster)
            task = " ".join((current_script, self.model_name))

            if ipaddress:
                # A specific machine is requested, try to lock it
                self.machine = self.cluster.lock(ipaddress, task)
                print("Locked requested machine at " + self.machine.ip_address)
            else:
                # No specific machine requested, find a free machine
                self.machine = self.cluster.wait_for_free_machine(task)
                print("Locked machine at " + self.machine.ip_address)

            # if any of the above fails, this line should throw
            self.ipaddress = self.machine.ip_address
        else:
            if not ipaddress:
                raise Exception("Missing ipaddress or pi cluster address")
            self.ipaddress = ipaddress

    def extract_model_info(self, ell_model, labels_file):
        """Extracts information about a model"""
        if (ell_model is None or labels_file is None):
            self.model_name = "d_I160x160x3CMCMCMCMCMCMC1AS"
            self.labels_file = os.path.join(self.test_dir, "categories.txt")
        else:
            self.ell_model = ell_model
            name,ext = os.path.splitext(ell_model)
            if ext.lower() == ".zip":
                with zipfile.ZipFile(ell_model) as myzip:
                    filename = myzip.extract(myzip.filelist[0])

                if filename != "":
                    print("extracted: " + filename)
                    self.ell_model = filename
                else:
                    # not a zip archive
                    self.ell_model = ell_model

            self.model_name, ext = os.path.splitext(basename(self.ell_model))
            if ext.lower() == ".zip":
                self.model_name, ext = os.path.splitext(self.model_name)
            self.labels_file = os.path.abspath(labels_file)

    def copy_files(self, filelist, folder):
        """Copies a list of files to a folder"""
        target_dir = os.path.join(self.test_dir, folder)
        if not os.path.isdir(target_dir):
            os.makedirs(target_dir)
        for path in filelist:
            print("Copying file: " + path + " to " + target_dir)
            if not os.path.isfile(path):
                raise Exception("expected file not found: " + path)
            head, file_name = os.path.split(path)
            dest = os.path.join(target_dir, file_name)
            copyfile(path, dest)

    def configure_runtest(self, dest):
        """Creates the remote bash script"""
        with open(os.path.join(self.ell_root, "tools/utilities/pitest/runtest.sh.in"), "r") as f:
            template = f.read()
        template = template.replace("@LABELS@", basename(self.labels_file))
        template = template.replace("@COMPILED_MODEL@", basename(self.model_name))
        template = template.replace("@TARGET_DIR@", self.target_dir)
        template = template.replace("@ITERATIONS@", str(self.iterations))
        output_template = os.path.join(dest, "runtest.sh")

        # raspberry pi requires runtest to use 0xa for newlines, so fix autocrlf that happens on windows.
        with open(output_template, "w", newline="\n") as of:
            of.write(template)

    def find_files_with_extension(self, path, extension):
        """Searches for files with the given extension"""
        cwd = os.getcwd()
        os.chdir(path)
        files = glob.glob("*.{}".format(extension))
        os.chdir(cwd)
        return files

    def get_bash_files(self):
        """Copies demo files needed to run the test"""
        self.copy_files( [ os.path.join(self.ell_root, "tools/utilities/pitest/coffeemug.jpg"),
                           os.path.join(self.ell_root, "tools/utilities/pythonlibs/demo.py"),
                           os.path.join(self.ell_root, "tools/utilities/pythonlibs/demoHelper.py"),
                           self.labels_file], self.output_dir)
        self.configure_runtest(self.output_dir)

        # avoid copying over bitcode files (as they are big)
        bitcode_files = self.find_files_with_extension(self.output_dir, "bc")
        for bitcode in bitcode_files:
            os.remove(os.path.join(self.output_dir, bitcode))

    def get_default_model(self):
        """Downloads the default model"""
        self.ell_model = os.path.join(self.test_dir, self.model_name + '.ell')
        if (not os.path.isfile(self.ell_model)) or (not os.path.isfile(self.labels_file)) :
            print("downloading default model...")
            download_and_extract_model(
                "https://github.com/Microsoft/ELL-models/raw/master/models/ILSVRC2012/d_I160x160x3CMCMCMCMCMCMC1AS/d_I160x160x3CMCMCMCMCMCMC1AS.ell.zip",
                model_extension=".ell",
                local_folder=self.test_dir)
            print("downloading default categories.txt...")
            self.labels_file = download_file("https://github.com/Microsoft/ELL-models/raw/master/models/ILSVRC2012/categories.txt",
                local_folder=self.test_dir)

    def get_model(self):
        """Initializes the user-specified model or picks the default one"""
        if self.model_name == "d_I160x160x3CMCMCMCMCMCMC1AS":
            self.get_default_model()
        print("using ELL model: " + self.model_name)

    def make_project(self):
        """Creates a project for the model and target"""
        if os.path.isdir(self.output_dir):
            rmtree(self.output_dir)
        sys.path.append(os.path.join(current_path, "../../wrap"))
        mpp = __import__("wrap")
        builder = mpp.ModuleBuilder()
        builder_args = [self.ell_model, "-target", self.target, "-outdir", self.output_dir, "-v",
            "--blas", str(self.blas) ]
        if self.profile:
            builder_args.append("-profile")
        builder.parse_command_line(builder_args)
        builder.run()

    def verify_remote_test(self, output):
        """Verifies the remote test results and prints a pass or fail"""
        print("==========================================================")
        found = False
        for line in output:
            if self.expression in line:
                found = True

        if found:
            print("Test passed")
        else:
            raise Exception("### Test Failed")


    def run_test(self):
        """Runs the test"""
        try:
            if not self.test:
                self.get_model()
                self.make_project()
                self.get_bash_files()

            runner = RemoteRunner(cluster=self.cluster,
                                  ipaddress=self.ipaddress,
                                  username=self.username,
                                  password=self.password,
                                  source_dir=self.output_dir,
                                  target_dir=self.target_dir,
                                  command="runtest.sh",
                                  verbose=True,
                                  cleanup=False)
            output = runner.run_command()
            self.verify_remote_test(output)
        except:
            errorType, value, traceback = sys.exc_info()
            print("### Exception: " + str(errorType) + ": " + str(value))
            raise Exception("### Test Failed")


if __name__ == "__main__":
    with DriveTest() as tester:
        tester.parse_command_line(sys.argv[1:]) # drop the first argument (program name)
        tester.run_test()
