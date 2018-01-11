#!/usr/bin/env python3
####################################################################################################
##
##  Project:  Embedded Learning Library (ELL)
##  File:     dependency_installer.py
##  Authors:  Lisa Ong
##
##  Requires: Python 3.x
##
####################################################################################################

import argparse
import json
import pip
import subprocess

class PackageInstaller:
    def __init__(self, requested_packages, verbose=False):
        self.requested_packages = dict.fromkeys(requested_packages)
        self.missing_packages = {}
        self.verbose = verbose

    def print_missing_packages(self):
        """Prints the list of packages that are not yet installed"""
        if self.verbose:
            if self.missing_packages:
                print("Found missing packages to be installed:")
                print(self.missing_packages)
            else:
                print("All packages are already installed")

class PipPackageInstaller(PackageInstaller):

    def find_missing_packages(self):
        """Finds the list of packages that are not yet installed"""
        listed = pip.get_installed_distributions()
        installed = dict.fromkeys([package for package in listed \
            if package.project_name in self.requested_packages])
        self.missing_packages = [package for package in self.requested_packages.keys() \
            if package not in installed]
        self.print_missing_packages()

    def install_missing_packages(self):
        """Installs the missing packages"""
        if self.missing_packages:
            if self.verbose:
                print("Installing missing packages....")
        for package in self.missing_packages:
            pip.main(["install", "--upgrade", package])
            if self.verbose:
                print("{}", package)

class CondaPackageInstaller(PackageInstaller):
    def __init__(self, requested_packages, verbose=False, environment="py34"):
        PackageInstaller.__init__(requested_packages, verbose)
        self.environment = environment

    def find_missing_packages(self):
        """Finds the list of packages that are not yet installed"""
        proc = subprocess.Popen(
            ["conda", "list", "--name", self.environment, "--json", "--canonical"],
            stdout=subprocess.PIPE, stderr=subprocess.PIPE,
            bufsize=0, universal_newlines=True)
        try:
            outs, errors = proc.communicate()
        except subprocess.TimeoutExpired:
            proc.kill()
            outs, errors = proc.communicate()

        if self.verbose:
            print(outs)
            print(errors)

        if proc.returncode:
            raise Exception("Failed to list installed packages: {}".format(outs))

        listed = json.loads(outs)

        # TODO: check package versions (currently we just check names)
        listed_names = [package.split("-", 1)[0] for package in listed]
        installed = dict.fromkeys([package for package in listed_names \
            if package in self.requested_packages])
        self.missing_packages = [package for package in self.requested_packages.keys() \
            if package not in installed]
        self.print_missing_packages()

    def install_missing_packages(self):
        """Installs the missing packages"""
        if self.missing_packages:
            if self.verbose:
                print("Installing missing packages....")

            proc = subprocess.Popen(
                ["conda", "install", "--name", self.environment, "--json", "--yes"] + \
                    self.missing_packages,
                stdout=subprocess.PIPE, stderr=subprocess.PIPE,
                bufsize=0, universal_newlines=True)
            try:
                outs, errors = proc.communicate()
            except subprocess.TimeoutExpired:
                proc.kill()
                outs, errors = proc.communicate()

            if self.verbose:
                print(outs)
                print(errors)

            if proc.returncode != 0:
                raise Exception("Failed to install one or more packages: {}".format(outs))

class DependencyInstaller:
    """Installs requested Anaconda and Pip packages"""
    def __init__(self, environment="py36", pip_packages=None, conda_packages=None, verbose=False):
        self.installers = []

        if pip_packages:
            self.installers.append(PipPackageInstaller(pip_packages, verbose))

        if conda_packages:
            self.installers.append(CondaPackageInstaller(conda_packages, verbose, environment))

    def _find_missing_packages(self):
        "Finds the list of packages that are not yet installed"
        for installer in self.installers:
            installer.find_missing_packages()

    def _install_missing_packages(self):
        "Installs the missing packages"
        for installer in self.installers:
            installer.install_missing_packages()

    def run(self):
        """Main run method"""
        self._find_missing_packages()
        self._install_missing_packages()

if __name__ == "__main__":

    arg_parser = argparse.ArgumentParser("Installs dependencies through pip or Anaconda\n"
        "Example: python dependency_installer.py --pip_packages 'psutil>=5'\n"
        "Example: python dependency_installer.py --conda_packages paramiko\n")

    arg_parser.add_argument("--pip_packages", "-pp", help="list of packages to pip install", nargs='+')
    arg_parser.add_argument("--conda_packages", "-cp", help="list of packages to conda install", nargs='+')
    arg_parser.add_argument("--environment", "-e", help="name of the Anaconda environment", default="py34")
    arg_parser.add_argument("--verbose", "-v", help="print verbose output", action="store_true")

    args = arg_parser.parse_args()

    installer = DependencyInstaller(args.environment, args.pip_packages,
        args.conda_packages, verbose=args.verbose)
    installer.run()
