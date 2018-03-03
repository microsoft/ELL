#!/usr/bin/env python3
####################################################################################################
##
##  Project:  Embedded Learning Library (ELL)
##  File:     drivetest.py
##  Authors:  Chris Lovett
##
##  Requires: Python 3.x
##
####################################################################################################

import logging
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

# this script may be called from a different location, so we need the path 
# relative to it
current_path = os.path.dirname(os.path.abspath(__file__))
sys.path.append(os.path.join(current_path, "../pythonlibs"))
import find_ell
import picluster
from download_helper import download_file, download_and_extract_model
from remoterunner import RemoteRunner
import logger

class DriveTest:
    def __init__(self, ipaddress=None, cluster=None, outdir=None, profile=False, 
            model=None, labels=None, target="pi3", target_dir="/home/pi/pi3", 
            username="pi", password="raspberry", iterations=1, expected=None, 
            blas=True, test=False, verbose=True, timeout=None):
        self.ipaddress = ipaddress
        self.build_root = find_ell.find_ell_build()
        self.ell_root = os.path.dirname(self.build_root)   
        self.output_dir = outdir
        self.target_dir = target_dir
        self.labels_file = labels
        self.ell_model = model
        self.username = username
        self.password = password
        self.target = target
        self.cluster = cluster
        self.blas = blas
        self.expected = expected
        self.profile = profile
        self.test = test
        self.verbose = verbose
        self.logger = logger.get()
        if timeout:
            self.timeout = int(timeout)
        else:
            self.timeout = None
        self.iterations = iterations
        # local state.
        self.model_name = None
        self.machine = None
        self.ell_json = None
        self.created_dirs = []
        self.gallery_url = "https://github.com/Microsoft/ELL-models/raw/master/models/ILSVRC2012/"

        # initialize state from the args
        if not self.output_dir:
            self.output_dir = "test"
        self.test_dir = os.path.abspath(self.output_dir)
        
        if os.path.isdir(self.test_dir):
            rmtree(self.test_dir)
        os.makedirs(self.test_dir)

        self.extract_model_info(self.ell_model, self.labels_file)
        self.output_dir = os.path.join(self.test_dir, self.target)

        self.resolve_address(self.ipaddress, self.cluster)
        if not os.path.isdir(self.output_dir):
            os.makedirs(self.output_dir)

    def __enter__(self):
        """Called when this object is instantiated with 'with'"""
        return self

    def __exit__(self, exc_type, exc_value, traceback):
        """Called on cleanup of this object that was instantiated with 'with'"""
        self.cleanup()

    def cleanup(self):
        """Unlocks the target device if it is part of a cluster"""
        if self.machine:
            self.logger.info("Unlocking machine: " + self.machine.ip_address)
            f = self.cluster.unlock(self.machine.ip_address)
            if f.current_user_name:
                self.logger.error("Failed to free the machine at " + self.machine.ip_address)
            else:
                self.logger.info("Freed machine at " + self.machine.ip_address)
        
    def resolve_address(self, ipaddress, cluster):
        """Resolves the ip address of the target device and locks it if it is
        part of a cluster"""
        if cluster:
            self.cluster = picluster.PiBoardTable(cluster)
            task = " ".join((current_script, self.model_name))

            if ipaddress:
                # A specific machine is requested, try to lock it
                self.machine = self.cluster.lock(ipaddress, task)
                self.logger.info("Locked requested machine at " + self.machine.ip_address)
            else:
                # No specific machine requested, find a free machine
                self.machine = self.cluster.wait_for_free_machine(task)
                self.logger.info("Locked machine at " + self.machine.ip_address)

            # if any of the above fails, this line should throw
            self.ipaddress = self.machine.ip_address
        else:
            if not ipaddress:
                raise Exception("Missing ipaddress or pi cluster address")
            self.ipaddress = ipaddress

    def extract_model_info(self, ell_model, labels_file):
        """Extracts information about a model"""
        if not ell_model:
            self.model_name = "d_I160x160x3CMCMCMCMCMCMC1AS"
            self.ell_model = self.model_name + ".ell"
        else:
            self.ell_model = ell_model
            name,ext = os.path.splitext(ell_model)
            if ext.lower() == ".zip":
                with zipfile.ZipFile(ell_model) as myzip:
                    filename = myzip.extract(myzip.filelist[0], self.test_dir)

                if filename != "":
                    self.logger.info("extracted: " + filename)
                    self.ell_model = filename
                else:
                    # not a zip archive
                    self.ell_model = ell_model

            self.model_name, ext = os.path.splitext(basename(self.ell_model))
            if ext.lower() == ".zip":
                self.model_name, ext = os.path.splitext(self.model_name)

        if not labels_file:
            self.labels_file = None
        else:
            self.labels_file = os.path.abspath(labels_file)


    def copy_files(self, filelist, folder):
        """Copies a list of files to a folder"""
        target_dir = os.path.join(self.test_dir, folder)
        if not os.path.isdir(target_dir):
            os.makedirs(target_dir)
        for path in filelist:
            if self.verbose:
                self.logger.info("Copying file: " + path + " to " + target_dir)
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

        # raspberry pi requires runtest to use 0xa for newlines, so fix autocrlf 
        # that happens on windows.
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
        self.copy_files( 
            [ os.path.join(self.ell_root, "tools/utilities/pitest/coffeemug.jpg"),
            os.path.join(self.ell_root, "tools/utilities/pythonlibs/vision/demo.py"),
            os.path.join(self.ell_root, "tools/utilities/pythonlibs/vision/demoHelper.py"),
            self.labels_file], self.output_dir)
        self.configure_runtest(self.output_dir)

        # avoid copying over bitcode files (as they are big)
        bitcode_files = self.find_files_with_extension(self.output_dir, "bc")
        for bitcode in bitcode_files:
            os.remove(os.path.join(self.output_dir, bitcode))

    def get_default_model(self):
        """Downloads the default model"""
        if (os.path.isfile(self.ell_model)):
            # a full path was already provided to a local model, no need for download.
            pass
        else:
            self.ell_model = os.path.join(self.test_dir, self.model_name + '.ell')
            if (not os.path.isfile(self.ell_model)) :
                self.logger.info("downloading default model...")
                download_and_extract_model(
                    self.gallery_url + self.model_name + "/" + self.model_name + ".ell.zip",
                    model_extension=".ell",
                    local_folder=self.test_dir)

    def get_default_labels(self):
        if not self.labels_file:
            self.labels_file = "categories.txt"
        if (not os.path.isfile(self.labels_file)):
            self.logger.info("downloading default categories.txt...")
            self.labels_file = download_file(self.gallery_url + "/categories.txt",
                local_folder=self.test_dir)

    def get_model(self):
        """Initializes the user-specified model or picks the default one"""
        self.get_default_model()
        self.get_default_labels()
        self.logger.info("using ELL model: " + self.model_name)

    def make_project(self):
        """Creates a project for the model and target"""
        if os.path.isdir(self.output_dir):
            rmtree(self.output_dir)
        sys.path.append(os.path.join(current_path, "../../wrap"))
        mpp = __import__("wrap")
        builder = mpp.ModuleBuilder()
        builder_args = [self.ell_model, "-target", self.target, "-outdir", 
            self.output_dir, "--blas", str(self.blas)]
        if self.verbose:
            builder_args.append("-v")
        if self.profile:
            builder_args.append("-profile")
        builder.parse_command_line(builder_args)
        builder.run()

    def verify_remote_test(self, output):
        """Verifies the remote test results and prints a pass or fail"""
        self.logger.info("==========================================================")
        found = False
        prediction_time = 0
        prompt = "Average prediction time:"
        previous = None
        prediction = "not found"
        for line in output:
            if prompt in line:
                prediction_time = float(line[len(prompt):])
                prediction = previous
            if "socket.timeout" in line:
                raise Exception("### Test failed due to timeout")
            previous = line

        if self.expected:
            found = (self.expected in prediction)
        else:
            found = True

        if found:
            self.logger.info("### Test passed")
            self.logger.info("Prediction=%s, time=%f" % (prediction, prediction_time))
        elif self.expected:
            msg = "### Test Failed, expecting %s, but found '%s' in time=%f" \
                % (self.expected, prediction, prediction_time)
            self.logger.error(msg)
            raise Exception(msg)
        else:
            self.logger.error("### Test Failed")
            raise Exception("### Test Failed")


    def run_test(self):
        """Runs the test"""
        try:
            if not self.test:
                self.get_model()
                self.make_project()
                self.get_bash_files()

            start_time = time.time()
            # do not pass cluster to remote runner because we've already locked the machine.
            runner = RemoteRunner(cluster=None,
                                  ipaddress=self.ipaddress,
                                  username=self.username,
                                  password=self.password,
                                  source_dir=self.output_dir,
                                  target_dir=self.target_dir,
                                  command="runtest.sh",
                                  verbose=self.verbose,
                                  start_clean=not self.test,
                                  timeout=self.timeout,
                                  cleanup=False)
            output = runner.run_command()
            self.verify_remote_test(output)
            end_time = time.time()
            self.logger.info("Remote test time: %f seconds" % (end_time - start_time))

        except:
            errorType, value, traceback = sys.exc_info()
            self.logger.error("### Exception: " + str(errorType) + ": " + str(value) + "\n" + str(traceback))
            raise Exception("### Test Failed")


if __name__ == "__main__":
        
    logging.basicConfig(level=logging.INFO, format="%(message)s")
    
    """Parses command line arguments"""
    arg_parser = argparse.ArgumentParser(
        "This script uses ELL to create a demo project for a model "
        "(default is d_I160x160x3CMCMCMCMCMCMC1AS from the ELL gallery)\n"
        "on a target device (default is Raspberry Pi 3), pushes it to the given\n"
        "device's ip address using ssh and scp, then executes the test.\n"
        "The test also measures the accuracy and performance of evaluating the model.\n")

    # options
    arg_parser.add_argument("--ipaddress", default=None, help="IP address of the target devices")
    arg_parser.add_argument("--cluster", default=None, help="http address of the cluster server that controls access to the target devices")
    arg_parser.add_argument("--outdir", default=".", help="where to store local working files as a staging area (default '.')")
    arg_parser.add_argument("--profile", help="enable profiling functions in the ELL module", action="store_true")

    model_group = arg_parser.add_argument_group("model", "options for loading a non-default model. All 3 must be specified for a non-default model to be used.")
    model_group.add_argument("--model", help="path to an ELL model file, the filename (without extension) will be used as the model name")
    model_group.add_argument("--labels", help="path to the labels file for evaluating the model")

    arg_parser.add_argument("--target", help="the target platform.\n"
        "Choices are pi3 (Raspberry Pi 3) and aarch64 (Dragonboard)", choices=["pi0", "pi3", "pi3_64", "aarch64"], default="pi3")
    arg_parser.add_argument("--target_dir", help="the directory on the target device for running the test", default="/home/pi/pi3")
    arg_parser.add_argument("--username", help="the username for the target device", default="pi")
    arg_parser.add_argument("--password", help="the password for the target device", default="raspberry")
    arg_parser.add_argument("--iterations", "-i", type=int, help="the number of iterations for each predict (default 1)", default=1)
    arg_parser.add_argument("--expected", "-e", help="the string to search for to verify test passed (default '')", default=None)
    arg_parser.add_argument("--blas", help="enable or disable the use of Blas on the target device (default 'True')", default="True")
    arg_parser.add_argument("--test", help="test only, assume the outdir has already been built (default 'False')", action="store_true")
    arg_parser.add_argument("--verbose", help="enable or disable verbose print output (default 'True')", default="True")
    arg_parser.add_argument("--timeout", help="set remote test run timeout in seconds (default '300')", default="300")

    argv = sys.argv
    argv.pop(0)
    args = arg_parser.parse_args(argv)

    def str2bool(v):
        """Converts a string to a bool"""
        return v.lower() in ("yes", "true", "t", "1")

    with DriveTest(args.ipaddress,  args.cluster, args.outdir, args.profile, 
        args.model, args.labels, args.target, args.target_dir, args.username, 
        args.password, args.iterations, args.expected, str2bool(args.blas), 
        args.test, str2bool(args.verbose), args.timeout) as tester:
        tester.run_test()
