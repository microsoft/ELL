#!/usr/bin/env python3
####################################################################################################
##
##  Project:  Embedded Learning Library (ELL)
##  File:     copy_validation_set.py (gallery)
##  Authors:  Lisa Ong
##
##  Requires: Python 3.x
##
####################################################################################################
import argparse
import os
import sys
from itertools import islice

_current_script = os.path.basename(__file__)
sys.path += [".."] # pythonlibs
import logger
import logging 
import picluster
from remoterunner import RemoteRunner

class CopyValidationSet:
    def __init__(self):
        self.arg_parser = argparse.ArgumentParser(
            "This script takes a CNTK val.map.txt and a path to the validation set, and copies maxfiles files\n"
            "to a target device (e.g. Raspberry Pi 3) using scp\n")
        self.ipaddress = None
        self.cluster = None
        self.username = "pi"
        self.password = "raspberry"
        self.validation_map = None
        self.validation_path = None
        self.labels = None
        self.maxfiles = 50
        self.created_dirs = []
        self.target_dir = "/home/pi/validation"
        self.machine = None
        self.logger = logger.get()

    def __enter__(self):
        """Called when this object is instantiated with 'with'"""
        return self

    def __exit__(self, exc_type, exc_value, traceback):
        """Called on cleanup of this object that was instantiated with 'with'"""
        self._cleanup()

    def parse_command_line(self, argv):
        """Parses command line arguments"""
        # required arguments
        self.arg_parser.add_argument("validation_map", help="val.map.txt file containing the filenames and classes")
        self.arg_parser.add_argument("validation_path", help="path to the validation set")
        self.arg_parser.add_argument("ipaddress", help="ip address of the target device to copy to")

        # options
        self.arg_parser.add_argument("--cluster", default=None, help="http address of the cluster server that controls access to the target devices")
        self.arg_parser.add_argument("--maxfiles", type=int, default=self.maxfiles, help="max number of files to copy (up to the size of the validation set)")
        self.arg_parser.add_argument("--target_dir", default=self.target_dir, help="destination directory on the target device")
        self.arg_parser.add_argument("--labels", default=None, help="path to the labels to optionally copy")
        self.arg_parser.add_argument("--username", default=self.username, help="username for the target device")
        self.arg_parser.add_argument("--password", default=self.password, help="password for the target device")

        args = self.arg_parser.parse_args(argv)

        self._init(args)

    def _init(self, args):
        self.ipaddress = args.ipaddress
        self.validation_map = args.validation_map
        self.validation_path = args.validation_path
        self.maxfiles = args.maxfiles
        self.labels = args.labels
        self.username = args.username
        self.password = args.password
        self.target_dir = args.target_dir

        if args.cluster:
            # try to lock the machine in the cluster
            self.cluster = picluster.PiBoardTable(args.cluster)
            self.machine = self.cluster.lock(self.ipaddress, _current_script)
            self.logger.info("Locked machine at " + self.machine.ip_address)

    def _cleanup(self):
        "Unlocks the target device if it is part of a cluster"
        if self.machine:
            f = self.cluster.unlock(self.machine.ip_address)
            if f.current_user_name:
                self.logger.error("Failed to free the machine at " + self.machine.ip_address)
            else:
                self.logger.info("Freed machine at " + self.machine.ip_address)

    def _get_validation_set(self):
        "Gets the validation set based on the input requirements like maxfiles"
        results = []

        with open(self.validation_map, 'r') as vm:
            subset = list(islice(vm, self.maxfiles))

            # subset contains the list to copy
            for entry in subset:
                path, _ = entry.split()
                filename = os.path.join(self.validation_path, os.path.basename(path))
                if not os.path.isfile(filename):
                    raise Exception("File not found: " + filename)
                results.append(filename)

            # copy the validation map as is
            results.append(self.validation_map)

            if self.labels:
                results.append(self.labels)

        return results

    def _publish(self, files):
        "Publishes files to the target device"
        runner = RemoteRunner(cluster=self.cluster,
                              ipaddress=self.ipaddress,
                              username=self.username,
                              password=self.password,
                              source_files=files,
                              target_dir=self.target_dir,
                              verbose=True,
                              cleanup=False)
        runner.run_command()

    def run(self):
        """Main run method"""
        files = self._get_validation_set()
        self._publish(files)

if __name__ == "__main__":
    logging.basicConfig(level=logging.INFO, format="%(message)s")
    with CopyValidationSet() as program:
        program.parse_command_line(sys.argv[1:]) # drop the first argument (program name)
        program.run()
