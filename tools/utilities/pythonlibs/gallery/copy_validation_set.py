####################################################################################################
##
##  Project:  Embedded Learning Library (ELL)
##  File:     copy_validation_set.py (gallery)
##  Authors:  Lisa Ong
##
##  Requires: Python 3.x
##
####################################################################################################
import os
import sys
import argparse
import paramiko
from itertools import islice
from os.path import basename

class CopyValidationSet:
    def __init__(self):
        self.arg_parser = argparse.ArgumentParser(
            "This script takes a CNTK val.map.txt and a path to the validation set, and copies maxfiles files\n"
            "to a Raspberry Pi using scp\n")
        self.ipaddress = None
        self.username = "pi"
        self.password = "raspberry"
        self.validation_map = None
        self.validation_path = None
        self.labels = None
        self.maxfiles = 200
        self.created_dirs = []
        self.dest_dir = "/home/pi/validation"

    def parse_command_line(self, argv):
        # required arguments
        self.arg_parser.add_argument("validation_map", help="val.map.txt file containing the filenames and classes")
        self.arg_parser.add_argument("validation_path", help="path to the validation set")
        self.arg_parser.add_argument("ipaddress", help="ip address of the Raspberry Pi to copy to")

        # options
        self.arg_parser.add_argument("--maxfiles", type=int, default=self.maxfiles, help="max number of files to copy (up to the size of the validation set)")
        self.arg_parser.add_argument("--destdir", default=self.dest_dir, help="destination directory on the Raspberry Pi")
        self.arg_parser.add_argument("--labels", default=None, help="path to the labels to optionally copy")

        argv.pop(0) # when passed directly into parse_args, the first argument (program name) is not skipped
        args = self.arg_parser.parse_args(argv)

        self.ipaddress = args.ipaddress
        self.validation_map = args.validation_map
        self.validation_path = args.validation_path
        self.maxfiles = args.maxfiles
        self.labels = args.labels

    def safe_mkdir(self, ssh, sftp, dir):
        if (not dir in self.created_dirs):
            print("mkdir: " + dir)
            self.exec_remote_command(ssh, "rm -rf " + dir)
            sftp.mkdir(dir)
            self.created_dirs.append(dir)

    def linux_join(self, path, name):
        # on windows os.path.join uses backslashes which doesn't work on the pi!
        return os.path.join(path, name).replace("\\","/")

    def publish(self, files):
        ssh = paramiko.SSHClient()
        ssh.set_missing_host_key_policy(paramiko.AutoAddPolicy())
        print("connecting to " + self.ipaddress)
        ssh.connect(self.ipaddress, username=self.username, password=self.password)
        sftp = paramiko.SFTPClient.from_transport(ssh.get_transport())

        self.safe_mkdir(ssh, sftp, self.dest_dir + "/")

        for src_file in files:
            if (os.path.isfile(src_file)):
                print("copying:" + src_file)
                dest_file = self.linux_join(self.dest_dir, basename(src_file))
                sftp.put(src_file, dest_file)

        sftp.close()

    def exec_remote_command(self, ssh, cmd):
        print("remote:" + cmd)
        output = []
        stdin, stdout, stderr = ssh.exec_command(cmd)
        for line in stdout:
            output.append(line.strip('\n'))
            print('... ' + line.strip('\n'))
        for line in stderr:
            output.append(line.strip('\n'))
            print('... ' + line.strip('\n'))
        return output

    def run(self):
        with open(self.validation_map, 'r') as vm:
            subset = list(islice(vm, self.maxfiles))
        
        # subset contains the list to copy
        files = []
        for entry in subset:
            f, c = entry.split()
            filename = os.path.join(self.validation_path, basename(f))
            if (not os.path.isfile(filename)):
                raise Exception("File not found: " + filename)
            files.append(filename)

        # copy the validation map as is
        files.append(self.validation_map)

        if (self.labels):
            files.append(self.labels)

        # publish files to the device
        self.publish(files)

if __name__ == "__main__":
    args = sys.argv
    program = CopyValidationSet()
    program.parse_command_line(args)
    program.run()