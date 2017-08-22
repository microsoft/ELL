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
import sys
current_path = os.path.dirname(os.path.abspath(__file__))
sys.path.append(os.path.join(current_path, '../pythonlibs'))
import find_ell
import subprocess
import json
import operator
from shutil import copyfile
from shutil import rmtree
from urllib import request
import paramiko

class DriveTest:
    def __init__(self):
        self.ipaddress = None
        self.build_root = find_ell.find_ell_build()
        self.ell_root = os.path.dirname(self.build_root)
        self.pitest_dir = os.path.join(self.build_root, "test", "pitest")
        self.output_dir = None
        self.config_file = None
        self.weights_file = None
        self.ell_model = None
        self.username = "pi"
        self.password = "raspberry"
        self.ssh = None
        self.created_dirs = []
        if (not os.path.isdir(self.pitest_dir)):
            os.makedirs(self.pitest_dir)

    def print_usage(self):
        print("Usage: drivetest.py ipaddress [outdir]")
        print("This script uses ELL to create a raspberry pi demo project for darknet model, pushes it to the given")
        print("raspberry pi ip address using ssh and scp, then executes the test.  The test measures the accuracy")
        print("and performance of the model and it writes these results to a test_results.json data file")
        
    def parse_command_line(self, argv):
        if (len(argv) == 1):
            return False
        if (len(argv) > 1):
            self.ipaddress = argv[1]
        if (len(argv) > 2):
            self.pitest_dir = argv[2]
        if (len(argv) > 3):
            return False
        self.output_dir = os.path.join(self.pitest_dir, "pi3")
        return True

    def download(self, url, localpath):
        req = request.URLopener()
        req.retrieve(url, localpath)

    def copy_files(self, list, folder):
        target_dir = os.path.join(self.pitest_dir, folder)
        if (not os.path.isdir(target_dir)):
            os.makedirs(target_dir)
        for path  in list:
            if (not os.path.isfile(path)):
                raise Exception("expected file not found: " + path)
            head,file_name = os.path.split(path)
            dest = os.path.join(target_dir, file_name)
            copyfile(path, dest)

    def normalize_newlines(self, src, dest):
        # raspberry pi requires runtest to use 0xa for newlines, so fix autocrlf that happens on windows.
        with open(src, 'r') as infile, open(dest, 'w', newline='\n') as outfile:
            outfile.writelines(infile.readlines())

    def get_bash_files(self):
        # copy demo files needed to run the test
        self.copy_files( [ os.path.join(self.ell_root, "tools/utilities/pitest/schoolbus.png"),        
                           os.path.join(self.ell_root, "tools/utilities/pythonlibs/demo.py"),
                           os.path.join(self.ell_root, "tools/utilities/pythonlibs/demoHelper.py") ], "pi3") 
        self.normalize_newlines(os.path.join(self.ell_root, "tools/utilities/pitest/runtest.sh"), 
                            os.path.join(self.pitest_dir, "pi3", "runtest.sh"))
        bitcode = os.path.join(self.output_dir, "darknet.bc")
        if (os.path.isfile(bitcode)):
            os.remove(bitcode) # don't need to copy this one over and it is big!

    def get_darknet(self):
        self.config_file = os.path.join(self.pitest_dir, "darknet.cfg")
        self.weights_file = os.path.join(self.pitest_dir, "darknet.weights")
        if (not os.path.isfile(self.config_file) or not os.path.isfile(self.weights_file)):
            print("downloading darknet model...")
            self.download("https://raw.githubusercontent.com/pjreddie/darknet/master/cfg/darknet.cfg", self.config_file)
            self.download("https://pjreddie.com/media/files/darknet.weights", self.weights_file)
        
        self.copy_files( [ os.path.join(self.ell_root, "tutorials/vision/gettingStarted/darknetImageNetLabels.txt") ], "")

    def import_darknet(self):
        self.ell_model = os.path.join(self.pitest_dir, "darknet.ellmodel")
        if (not os.path.isfile(self.ell_model)):
            sys.path.append(os.path.join(current_path, '../../importers/darknet'))
            darknet_importer = __import__("darknet_import")
            importer = darknet_importer.DarknetImporter()
            importer.parse_command_line(["", self.config_file, self.weights_file])
            importer.run()

            
    def make_project(self):
        labels_file = os.path.join(self.pitest_dir, "darknetImageNetLabels.txt")
        json_file = os.path.join(self.pitest_dir, "darknet_config.json")
        if (os.path.isdir(self.output_dir)):
            rmtree(self.output_dir)
        sys.path.append(os.path.join(current_path, '../../wrap'))
        mpp = __import__("wrap")
        builder = mpp.ModuleBuilder()
        builder.parse_command_line(["", json_file, labels_file, self.ell_model, "-target", "pi3", "-outdir", self.output_dir])
        builder.run()

    def connect_ssh(self):
        self.ssh = paramiko.SSHClient()
        self.ssh.set_missing_host_key_policy(paramiko.AutoAddPolicy())
        print("connecting to raspberry pi at " + self.ipaddress)
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

    def clean_pi(self):
        self.exec_remote_command('rm -rf pi3')

    def linux_join(self, path, name):
        # on windows os.path.join uses backslashes which doesn't work on the pi!
        return os.path.join(path, name).replace("\\","/")

    def safe_mkdir(self, sftp,  dir):
        if (not dir in self.created_dirs):
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
                if (os.path.isfile(src_file)):
                    dest_file = self.linux_join(dest, src_file[len(src) + 1:])
                    sftp.put(src_file, dest_file)
        
    def publish_bits(self):  
        sftp = paramiko.SFTPClient.from_transport(self.ssh.get_transport())
        self.sftp_copy_dir(sftp, self.output_dir, "/home/pi/pi3")
        sftp.close()     

    def execute_remote_test(self):   
        self.exec_remote_command("chmod u+x /home/pi/pi3/runtest.sh")
        output = self.exec_remote_command("/home/pi/pi3/runtest.sh")
        print("==========================================================")
        found = False
        for line in output:
            if (line.startswith("school bus")):                
                found = True
        
        if (found):
            print("Test passed")
        else:
            print("Test failed")
            sys.exit(1)
        
    def run_test(self):
        self.get_darknet()
        self.import_darknet()
        self.make_project()
        self.get_bash_files()
        self.connect_ssh()
        self.clean_pi()
        self.publish_bits()
        self.execute_remote_test()

if __name__ == "__main__":
    args = sys.argv
    tester = DriveTest()
    if (not tester.parse_command_line(args)):
        tester.print_usage()
    else:
        tester.run_test()