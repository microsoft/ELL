#!/usr/bin/env python3
####################################################################################################
##
##  Project:  Embedded Learning Library (ELL)
##  File:     remoterun.py
##  Authors:  Chuck Jacobs
##
##  Requires: Python 3.x
##
####################################################################################################

import argparse
import distutils.util
import os
import sys

import getpass

current_path = os.path.dirname(os.path.abspath(__file__))
sys.path += [os.path.join(current_path, '../pythonlibs')]
from remoterunner import RemoteRunner

if __name__ == "__main__":
    arg_parser = argparse.ArgumentParser(description="This script copies a project directory to a remote machine and runs a command in the copied directory (on the remote machine).")

    # options    
    arg_parser.add_argument("--cluster", help="optional http address of the cluster server that controls access to these machines")
    arg_parser.add_argument("--ipaddress", help="optional IP address of a specific target machine")
    arg_parser.add_argument("--source_dir", help="the directory to copy to the target device")
    arg_parser.add_argument("--target_dir", help="the directory on the target device to copy the files to")
    arg_parser.add_argument("--copyback_files", help="a list of files on the remote machine to copy back", nargs="*")
    arg_parser.add_argument("--copyback_dir", help="the directory on the host machine to copy files to (default: current directory)", default=".")
    arg_parser.add_argument("--username", help="the username for the target device", required=True)
    arg_parser.add_argument("--password", help="the password for the target device (optional: prompts if not present)")
    arg_parser.add_argument("--command", help="the command to run on the target device", nargs="*", default=[])
    arg_parser.add_argument("--logfile", help="filename to log output to")
    arg_parser.add_argument("--verbose", "-v", help="print output to the screen", action="store_true")
    arg_parser.add_argument("--cleanup", help="delete files from device after running", type=distutils.util.strtobool, default="true")
    arg_parser.add_argument("--timeout", type=int, help="maximum time in seconds for the job to run (default 300)", default=300)
    arg_parser.add_argument("--all", help="run the command on all machines in the cluster (default False)", action="store_true")
    
    args = arg_parser.parse_args()
    
    command = ' '.join(args.command)

    if args.password:
        password = args.password
    else:
        password = getpass.getpass()

    runner = RemoteRunner(cluster=args.cluster,
                          ipaddress=args.ipaddress, 
                          username=args.username, 
                          password=password,
                          source_dir=args.source_dir,
                          target_dir=args.target_dir,
                          copyback_files=args.copyback_files,
                          copyback_dir=args.copyback_dir,
                          command=command,
                          logfile=args.logfile,
                          verbose=args.verbose,
                          cleanup=args.cleanup,
                          timeout=args.timeout)
    if args.all:
        runner.run_all()
    else:
        runner.run_command()
    