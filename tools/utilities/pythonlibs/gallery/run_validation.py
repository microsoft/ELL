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
from time import sleep

class RunValidation:
    def __init__(self):
        self.arg_parser = argparse.ArgumentParser(
            "This script performs a validation pass on a given model\n"
            "on a target device (such as a Raspberry Pi) using scp, and retrieves the validation result\n"
            "This requires the model to have been deployed to /home/pi/pi3 (or similar) and\n"
            "the validation set to be copied to /home/pi/validation (or similar)\n")
        self.ipaddress = None
        self.username = "pi"
        self.password = "raspberry"
        self.model = None
        self.labels = "categories.txt"
        self.maxfiles = 200
        self.ssh = None
        self.target_dir = "/home/pi/pi3"
        self.target = "pi3"
        self.truth = "/home/pi/validation/val_map.txt"
        self.images = "/home/pi/validation"

    def parse_command_line(self, argv):
        # required arguments
        self.arg_parser.add_argument("model", help="name of the model")
        self.arg_parser.add_argument("ipaddress", help="ip address of the target device to copy to")

        # options
        self.arg_parser.add_argument("--labels", default=self.labels, help="name of the labels file")
        self.arg_parser.add_argument("--maxfiles", type=int, default=self.maxfiles, help="max number of files to copy (up to the size of the validation set)")
        self.arg_parser.add_argument("--target_dir", default=self.target_dir, help="destination directory on the target device")
        self.arg_parser.add_argument("--username", default=self.username, help="username for the target device")
        self.arg_parser.add_argument("--password", default=self.password, help="password for the target device")
        self.arg_parser.add_argument("--target", default=self.target, choices=['pi3', 'pi3_64', 'aarch64'], help="type of target device")
        self.arg_parser.add_argument("--images", default=self.images, help="path to the validation images on the target device")
        self.arg_parser.add_argument("--truth", default=self.truth, help="path to a tsv file on the target device, each line contains two values, the file name of the image and the integer classification value")

        argv.pop(0) # when passed directly into parse_args, the first argument (program name) is not skipped
        args = self.arg_parser.parse_args(argv)

        self.init(args)

    def init(self, args):
        self.ipaddress = args.ipaddress
        self.model = args.model
        self.labels = args.labels
        self.maxfiles = args.maxfiles
        self.username = args.username
        self.password = args.password
        self.target = args.target
        self.target_dir = args.target_dir
        self.truth = args.truth
        self.images = args.images

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
                dest_file = self.linux_join(self.target_dir, basename(src_file))
                sftp.put(src_file, dest_file)

        sftp.close()

    def receive(self, src, dest):
        sftp = paramiko.SFTPClient.from_transport(self.ssh.get_transport())
        print("receiving:" + src)
        src_file = self.linux_join(self.target_dir, src)

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
        template = template.replace("@TARGET_DIR@", self.target_dir)
        template = template.replace("@TRUTH@", self.truth)
        template = template.replace("@IMAGES@", self.images)

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

        # kill zombie python processes on the device
        self.exec_remote_command("pkill -9 python")

        # run validation
        self.exec_remote_command("chmod u+x {}/validate.sh".format(self.target_dir))
        output = self.exec_remote_command("{}/validate.sh".format(self.target_dir))
        self.receive("validation.out", "{}_validation_{}.out".format(self.model, self.target))
        self.receive("validation.json", "{}_validation_{}.json".format(self.model, self.target))

        # wait 2 seconds for procmon to exit
        sleep(2)
        self.receive("procmon.json", "{}_procmon_{}.json".format(self.model, self.target))

if __name__ == "__main__":
    args = sys.argv
    program = RunValidation()
    program.parse_command_line(args)
    program.run()