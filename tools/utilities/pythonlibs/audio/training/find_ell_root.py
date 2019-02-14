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

__this_file_directory = os.path.dirname(os.path.abspath(__file__))
__python_libs_directory = os.path.join(__this_file_directory, "..", "..")
if os.path.exists(os.path.join(__python_libs_directory, "find_ell.py")):
    sys.path += [__python_libs_directory]
    import find_ell
    ell_root = find_ell.get_ell_root()
else:
    ell_root = os.getenv("ELL_ROOT")
    if not ell_root:
        raise Exception("Please set your ELL_ROOT environment, as we will be using python scripts from there")

sys.path += [os.path.join(ell_root, "build", "interfaces", "python", "package")]
sys.path += [os.path.join(ell_root, "tools", "utilities", "pythonlibs")]
sys.path += [os.path.join(ell_root, "tools", "utilities", "pythonlibs", "audio")]

print("using scripts from ell root: " + ell_root)
