###################################################################################################
#
#  Project:  Embedded Learning Library (ELL)
#  File:     find_ell_root.py
#  Authors:  Chris Lovett
#
#  Requires: Python 3.x
#
###################################################################################################
import os
import sys

ell_root = os.getenv("ELL_ROOT")
if not ell_root:
    raise Exception("Please set your ELL_ROOT environment, as we will be using python scripts from there")

sys.path += [os.path.join(ell_root, "build", "interfaces", "python", "package")]
sys.path += [os.path.join(ell_root, "tools", "utilities", "pythonlibs", "audio")]

print("using scripts from ell root: " + ell_root)
