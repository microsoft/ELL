####################################################################################################
##
##  Project:  Embedded Learning Library (ELL)
##  File:     remoterunner.py
##  Authors:  Chris Lovett, Chuck Jacobs
##
##  Requires: Python 3.x
##
####################################################################################################

import os
import sys
import argparse

import paramiko

sys.path += ["../pythonlibs"]
import picluster

# 
# If no source_dir or source_files, don't copy
# If no target_dir, don't copy
# If no command, copy only

class RemoteRunner:
    def __init__(self, cluster=None, ipaddress=None, username=None, password=None,
        source_dir=None, target_dir=None, copyback_files=None, copyback_dir=None,
        command=None, logfile=None, verbose=True, start_clean=True, cleanup=True, timeout=None,
        all=None, source_files=None):

        if cluster:
            self.cluster = picluster.PiBoardTable(cluster)
        self.ipaddress = ipaddress
        self.username = username
        self.password = password
        self.source_dir = source_dir
        self.source_files = source_files
        self.target_dir = target_dir
        self.copyback_files = copyback_files
        self.copyback_dir = copyback_dir
        self.command = command
        self.verbose = verbose
        self.start_clean = start_clean
        self.cleanup = cleanup
        self.logfile = logfile
        self.timeout = timeout
        self.cluster = None
        if not cluster and not ipaddress:
            raise Exception("Error: required ipaddress or cluster or both")

        self.all = all
        self.machine = None
        self.ssh = None

        # Sanity-check parameters
        if os.path.pathsep in self.target_dir:
            raise Exception("Error: multilevel target directories not supported")

    def lock_machine(self):
        if not self.ipaddress:
            # then this is a pi cluster server, so find a free machine
            self.machine = self.cluster.wait_for_free_machine(self.command)
            self.print("Using machine at " + self.machine.ip_address)
            self.ipaddress = self.machine.ip_address
        elif self.cluster:
            self.machine = self.cluster.lock(self.ipaddress, self.command)
        return self.ipaddress

    def free_machine(self):
        if self.machine != None:
            f = self.cluster.unlock(self.machine.ip_address)
            if f.current_user_name:
                self.print("Failed to free the machine at " + self.machine.ip_address)

    def connect_ssh(self):
        self.ssh = paramiko.SSHClient()
        self.ssh.set_missing_host_key_policy(paramiko.AutoAddPolicy())
        self.print("connecting to target device at " + self.ipaddress)
        self.ssh.connect(self.ipaddress, username=self.username, password=self.password)

    def close_ssh(self):
        self.ssh.close()

    def exec_remote_command(self, cmd):
        self.print("remote: " + cmd)
        output = []
        stdin, stdout, stderr = self.ssh.exec_command(cmd, timeout=self.timeout)
        for line in stdout:
            output.append(line.rstrip("\n"))
            self.print("... " + line.rstrip("\n"))
        for line in stderr:
            output.append(line.rstrip("\n"))
            self.print("... " + line.rstrip("\n"))
        return output

    def clean_target(self):
        self.exec_remote_command("rm -rf {}".format(os.path.basename(self.target_dir)))

    def linux_join(self, path, name):
        # on windows os.path.join uses backslashes which doesn't work on the pi!
        return os.path.join(path, name).replace("\\","/")

    # TODO: make this recursive to deal with multi-level directories
    def safe_mkdir(self, sftp, newdir):
        try:
            sftp.mkdir(newdir)
            self.print("mkdir: {}".format(newdir))
        except IOError as error:
            import errno
            if error.errno == errno.EEXIST:
                self.print("Could not mkdir {}, directory already exists".format(newdir))

    def sftp_copy_dir(self, sftp, src, dest):
        self.safe_mkdir(sftp, dest + "/")
        for dirname, _, filenames in os.walk(src):
            src_relative_dirname = dirname[len(src) + 1:]
            target_dir = self.linux_join(dest, src_relative_dirname)
            self.safe_mkdir(sftp, target_dir)
            self.sftp_copy_files(sftp, src, dirname, filenames, dest)

    def sftp_copy_files(self, sftp, src=None, dirname=None, filenames=[], dest=""):
        for filename in filenames:
            if dirname and src:
                src_file = os.path.join(dirname, filename)
                dest_relative_src_file = src_file[len(src) + 1:]
            else:
                src_file = filename # assume absolute path
                dest_relative_src_file = os.path.basename(filename)

            if os.path.isfile(src_file):
                dest_file = self.linux_join(dest, dest_relative_src_file)
                self.print("copying {} to {}".format(src_file, dest_file))
                sftp.put(src_file, dest_file)

    def publish_bits(self):  
        if self.source_dir:
            with paramiko.SFTPClient.from_transport(self.ssh.get_transport()) as sftp:
                self.sftp_copy_dir(sftp, self.source_dir, self.target_dir)
        elif self.source_files:
            with paramiko.SFTPClient.from_transport(self.ssh.get_transport()) as sftp:
                self.safe_mkdir(sftp, self.target_dir)
                self.sftp_copy_files(sftp, src=None, dirname=None,
                    filenames=self.source_files, dest=self.target_dir)

    def copy_files(self):  
        if self.target_dir and self.copyback_files:
            os.makedirs(self.copyback_dir, exist_ok=True)
            with paramiko.SFTPClient.from_transport(self.ssh.get_transport()) as sftp:
                for src_filename in self.copyback_files:
                    src_file = self.linux_join(self.target_dir, src_filename)
                    dest_file = os.path.join(self.copyback_dir, src_filename)
                    print("Copying remote file from {} to {}".format(src_file, dest_file))
                    sftp.get(src_file, dest_file)
    
    def log_output(self, output):
        if self.logfile:
            with open(self.logfile, "w", encoding = "utf-8") as f:
                f.writelines([line+"\n" for line in output])

    def print(self, output):
        if self.verbose:
            print(output)

    def run_command(self):
        output = ""
        try:
            self.lock_machine()
            self.connect_ssh()
            if self.start_clean:
                self.clean_target()
            self.publish_bits()
            if self.command:
                self.exec_remote_command("cd {} && chmod u+x ./{}".format(self.target_dir, self.command.split(" ")[0]))
                output = self.exec_remote_command("cd {} && ./{}".format(self.target_dir, self.command))
            self.copy_files()
            if self.cleanup:
                self.clean_target()
            self.close_ssh()
            self.log_output(output)
        except:
            errorType, value, traceback = sys.exc_info()
            self.print("### Exception: {}: {}".format(errorType, value))
        finally:
            self.free_machine()
        return output

    def run_all(self):
        for machine in self.cluster.get_all():
            try:       
                self.ipaddress = machine.ip_address
                self.run_command()
            except:
                errorType, value, traceback = sys.exc_info()
                print("### cannot lock machine " + str(machine.ip_address))
                print("### Exception: " + str(errorType) + ": " + str(value))
