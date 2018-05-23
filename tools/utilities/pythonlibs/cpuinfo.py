#!/usr/bin/env python3
###################################################################################################
##
##  Project:  Embedded Learning Library (ELL)
##  File:     cpuinfo.py
##  Authors:  Chris Lovett
##
##  Requires: Python 3.x
##
###################################################################################################
import subprocess
import os
import platform

class CpuInfo:
    def __init__(self):
        if os.name == 'nt':
            self.processors = 0
            self.cores = None
            self.model_name = platform.processor() 
            self.platform = self.model_name
        else:
            proc = subprocess.Popen(
                ["cat", "/proc/cpuinfo"],
                stdout=subprocess.PIPE, stderr=subprocess.PIPE,
                universal_newlines = True)
            try:
                output, errors = proc.communicate()
            except subprocess.TimeoutExpired:
                proc.kill()
                output, errors = proc.communicate()

            if proc.returncode:
                raise Exception("Failed to list cpuinfo: {}".format(proc.returncode))

            self.processors = 0
            self.cores = None
            self.model_name = ""
            self.hardware = ""
            for line in output.split('\n'):
                if line.startswith("processor"):
                    self.processors += 1
                if line.startswith("cpu cores"):
                    self.cores = int(line.split(':')[1])
                if line.startswith("model name"):
                    self.model_name = line.split(':')[1].strip()
                if line.startswith("Hardware"):
                    self.hardware = line.split(':')[1].strip()

            if self.cores is None:
                self.cores = self.processors
            
            self.platform = "{} ({} cores) {}".format(self.model_name, self.cores, self.hardware)
            self.platform = self.platform.strip()


if __name__ == "__main__":
    ci = CpuInfo()
    print(ci.platform)


