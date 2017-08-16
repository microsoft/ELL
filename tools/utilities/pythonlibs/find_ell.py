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

# find the build directory relative to this file
def find_ell_build():
    current_path = os.path.dirname(os.path.abspath(__file__))
    build_dir = ""
    head,tail = os.path.split(current_path)
    while (tail != ""):
        test = os.path.join(head,"build")
        if (os.path.isdir(test)):
            build_dir = test
            break
        head,tail = os.path.split(head)
    return build_dir

def setup_ell_paths():
    build_dir = find_ell_build()
    if (build_dir == ""):
        print("Could not find 'build' directory, did you follow the ELL Python Binding build instructions?")
        sys.exit(1)    

    build_dir = os.path.abspath(build_dir)
    print("Using ELL build from: " + build_dir)

    ell_py_path = os.path.join(build_dir, "interfaces/python/ELL.py")

    if (not os.path.exists(ell_py_path)):
        print("Could not find '" + ell_py_path + "', did you follow the ELL Python Binding build instructions?")
        sys.exit(1)    

    if sys.platform == 'win32':
        release_path = os.path.join(build_dir, "interfaces/python/Release")
        if (not os.path.exists(release_path)):
            print("Could not find '" + release_path + "', did you build the Release configuration as per the README instructions?")

    sys.path.append(os.path.join(build_dir, 'interfaces/python'))
    sys.path.append(os.path.join(build_dir, 'interfaces/python/utilities'))
    sys.path.append(os.path.join(build_dir, 'interfaces/python/Release'))
    sys.path.append(os.path.join(build_dir, 'tools/importers/CNTK'))
    sys.path.append(os.path.join(build_dir, 'tools/importers/darknet'))

try:
    setup_ell_paths()
    __import__("ELL")
    __import__("ell_utilities")
except:    
    errorType, value, traceback = sys.exc_info()
    print("### Exception: " + str(errorType))
    print(str(value))
    print("====================================================================")
    print("ELL module is not loading")
    print("It is possible that you need to add LibOpenBLAS to your system path (See Install-*.md) from root of this repo")
    sys.exit(1)