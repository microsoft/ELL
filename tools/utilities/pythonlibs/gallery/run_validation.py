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
import paramiko
from itertools import islice
from os.path import basename

class RunValidation:
    def __init__(self):
        self.arg_parser = argparse.ArgumentParser(
            "This script performs a validation pass on a given model\n"
            "on a Raspberry Pi using scp, and retrieves the validation result\n"
            "This requires the model to have been deployed to /home/pi/pi3 and\n"
            "the validation set to be copied to /home/pi/validation\n")
        self.ipaddress = None
        self.username = "pi"
        self.password = "raspberry"
        self.model = None
        self.labels = "cntkVgg16ImageNetLabels.txt"
        self.maxfiles = 200
        self.ssh = None
        self.dest_dir = "/home/pi/pi3"

    def parse_command_line(self, argv):
        # required arguments
        self.arg_parser.add_argument("model", help="name of the model")
        self.arg_parser.add_argument("ipaddress", help="ip address of the Raspberry Pi to copy to")

        # options
        self.arg_parser.add_argument("--labels", default=self.labels, help="name of the labels file")
        self.arg_parser.add_argument("--maxfiles", type=int, default=self.maxfiles, help="max number of files to copy (up to the size of the validation set)")

        argv.pop(0) # when passed directly into parse_args, the first argument (program name) is not skipped
        args = self.arg_parser.parse_args(argv)

        self.ipaddress = args.ipaddress
        self.model = args.model
        self.labels = args.labels
        self.maxfiles = args.maxfiles

    def linux_join(self, path, name):
        # on windows os.path.join uses backslashes which doesn't work on the pi!
        return os.path.join(path, name).replace("\\","/")

    def connect(self):
        self.ssh = paramiko.SSHClient()
        self.ssh.set_missing_host_key_policy(paramiko.AutoAddPolicy())
        print("connecting to " + self.ipaddress)
        self.ssh.connect(self.ipaddress, username=self.username, password=self.password)

    def publish(self, files):
        sftp = paramiko.SFTPClient.from_transport(self.ssh.get_transport())

        for src_file in files:
            if (os.path.isfile(src_file)):
                print("copying:" + src_file)
                dest_file = self.linux_join(self.dest_dir, basename(src_file))
                sftp.put(src_file, dest_file)

        sftp.close()

    def receive(self, src, dest):
        sftp = paramiko.SFTPClient.from_transport(self.ssh.get_transport())
        print("receiving:" + src)
        src_file = self.linux_join(self.dest_dir, src)

        try:
            sftp.get(src_file, dest)
        except IOError:
            print("file not found: " + src_file)
        finally:
            sftp.close()

    def exec_remote_command(self, cmd):
        print("remote:" + cmd)
        output = []
        stdin, stdout, stderr = self.ssh.exec_command(cmd)
        for line in stdout:
            output.append(line.strip('\n'))
            print('... ' + line.strip('\n'))
        for line in stderr:
            output.append(line.strip('\n'))
            print('... ' + line.strip('\n'))
        return output

    def delete_if_exists(self, filename):
        try:
            os.remove(filename)
        except:
            pass

    def configure_sh_script(self, output):
        with open("validate.sh.in", 'r') as f:
            template = f.read()
        template = template.replace("@LABELS@", self.labels)
        template = template.replace("@COMPILED_MODEL@", self.model)
        template = template.replace("@MAXFILES@", str(self.maxfiles))

        self.delete_if_exists(output)
        # Raspberry pi requires runtest to use 0xa for newlines, so fix autocrlf that happens on windows.
        with open(output, 'w', newline='\n') as of:
            of.write(template)

    def run(self):
        self.connect()
        self.configure_sh_script("validate.sh")

        # publish files to the device
        bash_files = ["validate.sh", "validate.py", "../procmon.py"]
        self.publish(bash_files)

        # run validation
        self.exec_remote_command("chmod u+x /home/pi/pi3/validate.sh")
        output = self.exec_remote_command("/home/pi/pi3/validate.sh")
        self.receive("validation.json", self.model + "_validation.json")
        self.receive("procmon.json", self.model + "_procmon.json")

if __name__ == "__main__":
    args = sys.argv
    program = RunValidation()
    program.parse_command_line(args)
    program.run()