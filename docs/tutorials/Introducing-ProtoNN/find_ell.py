###############################################################################
#
#  Project:  Embedded Learning Library (ELL)
#  File:     find_ell.py
#  Authors:  Chris Lovett
#  Requires: Python 3.x
#
###############################################################################
import os
import sys


def find_ell():
    """ Find the location of the ELL repo and the build directory """
    root = os.environ.get("ELL_ROOT")
    if not root:
        print("'ELL_ROOT' environment variable is not set.")
        print("Please set the ELL_ROOT environment variable to point to your ELL git repo")
        sys.exit(1)

    if not os.path.isdir(root):
        print("'ELL_ROOT' environment variable points to invalid directory.")
        print("Please set the ELL_ROOT environment variable to point to your ELL git repo")
        sys.exit(1)

    build_dir = os.path.join(root, "build")
    if not os.path.isdir(build_dir):
        print("Please create 'build' for the ELL repo at '" + root + "'")
        sys.exit(1)

    py_build = os.path.join(build_dir, "interfaces/python")
    sys.path.append(py_build)

    build_dir = os.path.join(root, "build")
    if not os.path.isdir(build_dir):
        print("Please create 'build' for the ELL repo at '" + root + "'")
        sys.exit(1)

    sys.path.append(os.path.join(build_dir, "interfaces/python/Release"))
    try:
        import ell
    except:
        print("import ell failed")
        print("Please check that you built the ELL repo at '" + root + "'")
        print("And remember to build the _ELL_python target")
    return root


find_ell()
