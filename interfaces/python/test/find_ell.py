# this script is only here to enable debugging of the unit tests outside of unit test running

import os
import sys

_script_path = os.path.dirname(os.path.abspath(__file__))

# look for a directory that is in the root, but not in the build folder.
expected_dir = "external" 
_root_dir = _script_path
while not os.path.isdir(os.path.join(_root_dir, expected_dir)):
    parent = os.path.dirname(_root_dir)
    if parent == _root_dir:
        break
    _root_dir = parent

if not os.path.isdir(os.path.join(_root_dir, expected_dir)):
    raise Exception("Could not find '%s' dir, please run this script in the ELL repo." % (expected_dir))

sys.path.append(os.path.join(_root_dir, "build/interfaces/python/package"))

def get_ell_root():
    global _root_dir
    return _root_dir