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

def __is_ell_py_dir(path):
    ell_py_path = os.path.join(path, "ELL.py")
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

try:
    ell_py_dir = __get_ell_py_dir()
    if ell_py_dir is None:
        print("Could not find '" + ell_py_dir + "', did you follow the ELL Python Binding build instructions?")
        sys.exit(1)
    sys.path.append(ell_py_dir)
    import ELL
    import ell_utilities
except:
    errorType, value, traceback = sys.exc_info()
    print("### Exception: " + str(errorType))
    print(str(value))
    print("====================================================================")
    print("ELL module is not loading")
    print("It is possible that you need to add LibOpenBLAS to your system path (See Install-*.md) from root of this repo.")
    sys.exit(1)
