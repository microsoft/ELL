####################################################################################################
##
##  Project:  Embedded Learning Library (ELL)
##  File:     find_ell.py
##  Authors:  Chris Lovett
##
##  Requires: Python 3.x
##
####################################################################################################
import sys
import os

__this_file_directory = os.path.dirname(os.path.abspath(__file__))

# find the ELL build directory relative to this file
def find_ell_build():
    build_dir = ""
    head,tail = os.path.split(__this_file_directory)
    while (tail != ""):
        test = os.path.join(head,"build")
        if (os.path.isdir(test)):
            build_dir = test
            break
        head,tail = os.path.split(head)
    return build_dir

def __is_ell_py_dir(path):
    ell_py_path = os.path.join(path, "ell.py")
    return os.path.isfile(ell_py_path)

def __get_ell_py_dir():
    # First check if we're in the source tree and have a build directory called 'build', then check if we're in a valid build tree
    candidate_dirs = [
        os.path.abspath(os.path.join(__this_file_directory, '..', '..', '..', 'build', 'interfaces', 'python')),
        os.path.abspath(os.path.join(__this_file_directory, '..', '..', '..', 'interfaces', 'python'))
    ]

    for d in candidate_dirs:
        if __is_ell_py_dir(d):
            return d

    return None


ell_py_dir = __get_ell_py_dir()
if ell_py_dir is None:
    print("Could not find ell.py, did you follow the ELL Python Binding build instructions?")
    sys.exit(1)
sys.path.append(ell_py_dir)
import ell
import ell_utilities