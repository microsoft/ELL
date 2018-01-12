#!/usr/bin/env python3
####################################################################################################
##
##  Project:  Embedded Learning Library (ELL)
##  File:     run_validation.py (gallery)
##  Authors:  Lisa Ong
##
##  Requires: Python 3.x
##
####################################################################################################

import os
import sys
import argparse
import logging
from shutil import copyfile

_current_script = os.path.basename(__file__)
_current_script_directory = os.path.dirname(os.path.abspath(__file__))

sys.path += ["../"] # pythonlibs
import picluster
from remoterunner import RemoteRunner

class RunValidation:
    def __init__(self):
        self.arg_parser = argparse.ArgumentParser(
            "This script performs a validation pass on a given model\n"
            "on a target device (such as a Raspberry Pi) using scp, and retrieves the validation result\n"
            "Pre-requisites:\n"
            "    1) the model files, deployed to /home/pi/pi3 (or similar) using drivetest.py\n"
            "    2) the validation set, copied to /home/pi/validation (or similar) using copy_validation_set.py\n")
        self.ipaddress = None
        self.cluster = None
        self.username = "pi"
        self.password = "raspberry"
        self.model_name = None
        self.labels = "categories.txt"
        self.maxfiles = 200
        self.ssh = None
        self.target_dir = "/home/pi/pi3"
        self.target = "pi3"
        self.truth = "/home/pi/validation/val_map.txt"
        self.images = "/home/pi/validation"
        self.test_dir = None
        self.output_dir = None
        self.machine = None
        self.logger = logging.getLogger(__name__)

    def __enter__(self):
        """Called when this object is instantiated with 'with'"""
        return self

    def __exit__(self, exc_type, exc_value, traceback):
        """Called on cleanup of this object that was instantiated with 'with'"""
        self._cleanup()

    def parse_command_line(self, argv):
        """Parses command line arguments"""
        # required arguments
        self.arg_parser.add_argument("model", help="name of the model")
        self.arg_parser.add_argument("ipaddress", help="ip address of a target device")

        # options
        self.arg_parser.add_argument("--cluster", default=self.cluster, help="http address of the cluster server that provides access to the target device")
        self.arg_parser.add_argument("--labels", default=self.labels, help="name of the labels file")
        self.arg_parser.add_argument("--maxfiles", type=int, default=self.maxfiles, help="max number of files to copy (up to the size of the validation set)")
        self.arg_parser.add_argument("--target_dir", default=self.target_dir, help="destination directory on the target device")
        self.arg_parser.add_argument("--username", default=self.username, help="username for the target device")
        self.arg_parser.add_argument("--password", default=self.password, help="password for the target device")
        self.arg_parser.add_argument("--target", default=self.target, choices=['pi3', 'pi3_64', 'aarch64'], help="type of target device")
        self.arg_parser.add_argument("--images", default=self.images, help="path to the validation images on the target device")
        self.arg_parser.add_argument("--truth", default=self.truth, help="path to a tsv file on the target device, each line contains two values, the file name of the image and the integer classification value")
        self.arg_parser.add_argument("--test_dir", help="the folder on the host to collect model files", default="test")

        args = self.arg_parser.parse_args(argv)

        self._init(args)

    def _init(self, args):
        self.model_name = args.model
        self.labels = args.labels
        self.maxfiles = args.maxfiles
        self.username = args.username
        self.password = args.password
        self.target = args.target
        self.target_dir = args.target_dir
        self.truth = args.truth
        self.images = args.images
        self.test_dir = args.test_dir

        self.output_dir = os.path.join(self.test_dir, self.target, self.model_name)
        if not os.path.isdir(self.output_dir):
            os.makedirs(self.output_dir)

        self._resolve_address(args.ipaddress, args.cluster)

    def _cleanup(self):
        "Unlocks the target device if it is part of a cluster"
        if self.machine:
            f = self.cluster.unlock(self.machine.ip_address)
            if f.current_user_name:
                self.logger.error("Failed to free the machine at " + self.machine.ip_address)
            else:
                self.logger.info("Freed machine at " + self.machine.ip_address)

    def _resolve_address(self, ipaddress, cluster):
        "Resolves the ip address of the target device and locks it if it is part of a cluster"
        if cluster:
            # lock the machine in the cluster
            task = " ".join((_current_script, self.model_name))
            self.cluster = picluster.PiBoardTable(cluster)
            self.machine = self.cluster.lock(ipaddress, task)
            self.logger.info("Locked machine at " + self.machine.ip_address)
            self.ipaddress = self.machine.ip_address
        else:
            self.ipaddress = ipaddress

    def _configure_script(self, output):
        "Creates the remote bash script"
        with open(os.path.join(_current_script_directory, "validate.sh.in"), 'r') as f:
            template = f.read()
        template = template.replace("@LABELS@", self.labels)
        template = template.replace("@COMPILED_MODEL@", self.model_name)
        template = template.replace("@MAXFILES@", str(self.maxfiles))
        template = template.replace("@TARGET_DIR@", self.target_dir)
        template = template.replace("@TRUTH@", self.truth)
        template = template.replace("@IMAGES@", self.images)

        # Linux-based target devices use \n for newlines instead of \r\n
        with open(output, 'w', newline='\n') as of:
            of.write(template)

    def run(self):
        """Main run method"""
        self._configure_script(os.path.join(self.output_dir, "validate.sh"))

        # Other files are copied by drivetest.py
        for f in [os.path.join(_current_script_directory, "..", "procmon.py"),
                  os.path.join(_current_script_directory, "..", "dependency_installer.py"),
                  os.path.join(_current_script_directory, "validate.py")]:
            copyfile(f, os.path.join(self.output_dir, os.path.basename(f)))

        runner = RemoteRunner(cluster=self.cluster,
                              ipaddress=self.ipaddress,
                              username=self.username,
                              password=self.password,
                              source_dir=self.output_dir,
                              target_dir=self.target_dir,
                              command="validate.sh",
                              verbose=True,
                              copyback_files=['validation.json', 'validation.out', 'procmon.json'],
                              copyback_dir=self.output_dir,
                              start_clean=False, # reuse what drivetest.py already setup
                              cleanup=False)
        output = runner.run_command()
        #self.logger.info(output) this has already been logged by remote runner.

if __name__ == "__main__":
    logging.basicConfig(level=logging.INFO, format="%(message)s")
    with RunValidation() as program:
        program.parse_command_line(sys.argv[1:]) # drop the first argument (program name)
        program.run()
