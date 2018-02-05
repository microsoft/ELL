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

# find the root of the ELL git repo
def get_ell_root():
    expected_dir = "external"
    root_dir = __this_file_directory
    while not os.path.isdir(os.path.join(root_dir, expected_dir)):
        parent = os.path.dirname(root_dir)
        if parent == root_dir:
            break
        root_dir = parent
    return root_dir

# find the ELL build directory relative to this file
def find_ell_build():
    build_dir = ""
    head,tail = os.path.split(__this_file_directory)
    while (tail != ""):
        # find a file that is unique to the ELL build folder
        test = os.path.join(head,"ell_build_tools.json")
        if (os.path.isfile(test)):
            return head

        # find a file that is unique to the ELL repo root.
        test = os.path.join(head,"StyleGuide.md")
        if (os.path.isfile(test)):
            for d in os.listdir(head):
                dd = os.path.join(head, d)
                # support different named build folders, like 'build_gcc' or 'build_clang'
                if d.startswith("build") and os.path.isdir(dd):
                    return dd
            raise Exception("ELL build folder not found in {}\nFound:{}".format(head,os.listdir(head)))

        head,tail = os.path.split(head)
    return build_dir

ell_py_dir = os.path.join(find_ell_build(), "interfaces", "python", "package")
if ell_py_dir is None:
    raise ImportError("Could not find ell package, did you follow the ELL Python Binding build instructions?")

sys.path.append(ell_py_dir)
