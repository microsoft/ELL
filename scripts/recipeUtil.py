#
# recipeUtil -- various utility functions to make recipes easier to write
#
import sys
import os
import subprocess

class CommandlineArgs(object):
    def __init__(self, args):
        self.executable = args[0]
        self.args = args[1:]

    def __getitem__(self, index):
        if index >= 0 and index < len(self.args):
            return self.args[index]
        else:
            return None

    def __str__(self):
        return str(self.args)

def findDirWithFile(dirs, filenames):
    '''Of the supplied list of directories, returns the first one that contains a file in the list filenames'''
    for dir in dirs:
        for filename in filenames:
            if os.path.isfile(os.path.join(dir, filename)):
                return dir
    return None

def runBinary(binaryFilename, args=None):
    binDir = findDirWithFile(['../bin/Debug', '../bin/Release', '../bin'], [binaryFilename, binaryFilename+'.exe'])
    if not binDir:
        return
    binPath = os.path.join(binDir, binaryFilename)

    if args:
        subprocess.check_call([binPath]+args)
    else:
        subprocess.check_call([binPath])

def exit(status=None):
    sys.exit(status)

# Predefined objects
args = CommandlineArgs(sys.argv)
