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
current_path = os.path.dirname(os.path.abspath(__file__))
sys.path.append(os.path.join(current_path, '../pythonlibs'))
import find_ell
import argparse
import subprocess
import json
import operator
from shutil import copyfile
from shutil import rmtree
from urllib import request
import paramiko
import ziptools

class DriveTest:
    def __init__(self):
        self.arg_parser = argparse.ArgumentParser(
            "This script uses ELL to create a demo project for a model (default is darknet)\n"
            "on a target device (default is Raspberry Pi 3), pushes it to the given\n"
            "device's ip address using ssh and scp, then executes the test.\n"
            "The test also measures the accuracy and performance of evaluating the model.\n")
        self.ipaddress = None
        self.build_root = find_ell.find_ell_build()
        self.ell_root = os.path.dirname(self.build_root)
        self.test_dir = os.path.join(self.build_root, "test", "pitest")
        self.output_dir = None
        self.target_dir = "/home/pi/pi3"
        self.config_file = None
        self.weights_file = None
        self.model_name = None
        self.labels_file = None
        self.ell_model = None
        self.ell_json = None
        self.username = "pi"
        self.password = "raspberry"
        self.target = "pi3"
        self.ssh = None
        self.created_dirs = []
        self.profile = False
        if (not os.path.isdir(self.test_dir)):
            os.makedirs(self.test_dir)

    def parse_command_line(self, argv):
        # required arguments
        self.arg_parser.add_argument("ipaddress")

        # options
        self.arg_parser.add_argument("--outdir", default=self.test_dir)
        self.arg_parser.add_argument("--profile", help="enable profiling functions in the ELL module", action="store_true")

        model_group = self.arg_parser.add_argument_group('model', 'options for loading a non-default model. All 3 must be specified for a non-default model to be used.')
        model_group.add_argument("--model", help="path to an ELL model file, the filename (without extension) will be used as the model name")
        model_group.add_argument("--labels", help="path to the labels file for evaluating the model")

        self.arg_parser.add_argument("--target", help="the target platform.\n"
            "Choices are pi3 (Raspberry Pi 3) and aarch64 (Dragonboard)", choices=["pi3", "pi3_64", "aarch64"], default=self.target)
        self.arg_parser.add_argument("--target_dir", help="the directory on the target device for running the test", default=self.target_dir)
        self.arg_parser.add_argument("--username", help="the username for the target device", default=self.username)
        self.arg_parser.add_argument("--password", help="the password for the target device", default=self.password)

        argv.pop(0) # when passed directly into parse_args, the first argument (program name) is not skipped
        args = self.arg_parser.parse_args(argv)

        self.init(args)

    def init(self, args):
        self.ipaddress = args.ipaddress
        self.test_dir = args.outdir
        self.profile = args.profile
        self.target = args.target
        self.target_dir = args.target_dir
        self.username = args.username
        self.password = args.password

        self.extract_model_info(args.model, args.labels)

        self.output_dir = os.path.join(self.test_dir, self.target, self.model_name)
        if not os.path.isdir(self.output_dir):
            os.makedirs(self.output_dir)

    def extract_model_info(self, ell_model, labels_file):
        if (ell_model is None or labels_file is None):
            self.model_name = "darknet"
            self.labels_file = os.path.join(self.test_dir, "darknetImageNetLabels.txt")
        else:
            # extract the model if it's in an archive
            unzip = ziptools.Extractor(ell_model)
            success, filename = unzip.extract_file(".ell")
            if success:
                print("extracted: " + filename)
                self.ell_model = filename
            else:
                # not a zip archive
                self.ell_model = ell_model

            self.model_name, ext = os.path.splitext(basename(self.ell_model))
            if ext.lower() == ".zip":
                self.model_name, ext = os.path.splitext(self.model_name)
            self.labels_file = labels_file

    def download(self, url, localpath):
        req = request.URLopener()
        req.retrieve(url, localpath)

    def copy_files(self, list, folder):
        target_dir = os.path.join(self.test_dir, folder)
        if not os.path.isdir(target_dir):
            os.makedirs(target_dir)
        for path  in list:
            if not os.path.isfile(path):
                raise Exception("expected file not found: " + path)
            head,file_name = os.path.split(path)
            dest = os.path.join(target_dir, file_name)
            copyfile(path, dest)

    def configure_runtest(self, dest):
        with open(os.path.join(self.ell_root, "tools/utilities/pitest/runtest.sh.in"), 'r') as f:
            template = f.read()
        template = template.replace("@LABELS@", basename(self.labels_file))
        template = template.replace("@COMPILED_MODEL@", basename(self.model_name))
        template = template.replace("@TARGET_DIR@", self.target_dir)
        output_template = os.path.join(dest, "runtest.sh")

        # raspberry pi requires runtest to use 0xa for newlines, so fix autocrlf that happens on windows.
        with open(output_template, 'w', newline='\n') as of:
            of.write(template)

    def get_bash_files(self):
        # copy demo files needed to run the test
        self.copy_files( [ os.path.join(self.ell_root, "tools/utilities/pitest/coffeemug.jpg"),
                           os.path.join(self.ell_root, "docs/tutorials/shared/demo.py"),
                           os.path.join(self.ell_root, "tools/utilities/pythonlibs/demoHelper.py") ], self.output_dir) 
        self.configure_runtest(self.output_dir)
    
        bitcode = os.path.join(self.output_dir, self.model_name + ".bc")
        if os.path.isfile(bitcode):
            os.remove(bitcode) # don't need to copy this one over and it is big!

    def get_darknet(self):
        self.config_file = os.path.join(self.test_dir, "darknet.cfg")
        self.weights_file = os.path.join(self.test_dir, "darknet.weights")
        if (not os.path.isfile(self.config_file) or not os.path.isfile(self.weights_file)):
            print("downloading darknet model...")
            self.download("https://raw.githubusercontent.com/pjreddie/darknet/master/cfg/darknet.cfg", self.config_file)
            self.download("https://pjreddie.com/media/files/darknet.weights", self.weights_file)
        
        self.copy_files( [ os.path.join(self.ell_root, "docs/tutorials/Importing-new-models/darknetImageNetLabels.txt") ], "")

    def import_darknet(self):
        self.ell_model = os.path.join(self.test_dir, self.model_name + ".ell")
        sys.path.append(os.path.join(current_path, '../../importers/darknet'))
        darknet_importer = __import__("darknet_import")
        args = {}
        args['config_file'] = self.config_file
        args['weights_file'] = self.weights_file
        args['output_directory'] = None
        importer = darknet_importer.DarknetImporter(args)
        importer.run()

    def get_model(self):
        if self.model_name == "darknet":
            self.get_darknet()
            self.import_darknet()
        print("using ELL model: " + self.model_name)

    def make_project(self):
        labels_file = os.path.join(self.test_dir, self.labels_file)
        if os.path.isdir(self.output_dir):
            rmtree(self.output_dir)
        sys.path.append(os.path.join(current_path, '../../wrap'))
        mpp = __import__("wrap")
        builder = mpp.ModuleBuilder()
        builder_args = [labels_file, self.ell_model, "-target", self.target, "-outdir", self.output_dir]
        if self.profile:
            builder_args.append("-profile")
        builder.parse_command_line(builder_args)
        builder.run()

    def connect_ssh(self):
        self.ssh = paramiko.SSHClient()
        self.ssh.set_missing_host_key_policy(paramiko.AutoAddPolicy())
        print("connecting to target device at " + self.ipaddress)
        self.ssh.connect(self.ipaddress, username=self.username, password=self.password)

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

    def clean_target(self):
        self.exec_remote_command('rm -rf {}'.format(basename(self.target_dir)))

    def linux_join(self, path, name):
        # on windows os.path.join uses backslashes which doesn't work on the pi!
        return os.path.join(path, name).replace("\\","/")

    def safe_mkdir(self, sftp,  dir):
        if not dir in self.created_dirs:
            print("mkdir: " + dir)
            sftp.mkdir(dir)
            self.created_dirs.append(dir)

    def sftp_copy_dir(self, sftp, src, dest):
        self.safe_mkdir(sftp, dest + "/")
        for dirname, dirnames, filenames in os.walk(src):
            target_dir = self.linux_join(dest, dirname[len(src) + 1:])
            self.safe_mkdir(sftp, target_dir)
            for filename in filenames:
                src_file = os.path.join(dirname, filename)
                print("copying:" + src_file)
                if os.path.isfile(src_file):
                    dest_file = self.linux_join(dest, src_file[len(src) + 1:])
                    sftp.put(src_file, dest_file)
        
    def publish_bits(self):  
        sftp = paramiko.SFTPClient.from_transport(self.ssh.get_transport())
        self.sftp_copy_dir(sftp, self.output_dir, self.target_dir)
        sftp.close()     

    def execute_remote_test(self):   
        self.exec_remote_command("chmod u+x {}/runtest.sh".format(self.target_dir))
        output = self.exec_remote_command("{}/runtest.sh".format(self.target_dir))
        print("==========================================================")
        found = False
        for line in output:
            if "coffee mug" in line:
                found = True
        
        if found:
            print("Test passed")
        else:
            print("Test failed")
            sys.exit(1)
        
    def run_test(self):
        self.get_model()
        self.make_project()
        self.get_bash_files()
        self.connect_ssh()
        self.clean_target()
        self.publish_bits()
        self.execute_remote_test()

if __name__ == "__main__":
    args = sys.argv
    tester = DriveTest()
    tester.parse_command_line(args)
    tester.run_test()