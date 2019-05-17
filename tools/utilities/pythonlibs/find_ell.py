####################################################################################################
#
#  Project:  Embedded Learning Library (ELL)
#  File:     find_ell.py
#  Authors:  Chris Lovett
#
#  Requires: Python 3.x
#
####################################################################################################
import sys
import os

__this_file_directory = os.path.dirname(os.path.abspath(__file__))


def get_ell_root():
    """ find the root of the ELL git repo """
    expected_dir = "external"
    root_dir = __this_file_directory
    while not os.path.isdir(os.path.join(root_dir, expected_dir)):
        parent = os.path.dirname(root_dir)
        if parent == root_dir:
            break
        root_dir = parent
    return root_dir


def find_ell_build():
    """ find the ELL build directory relative to this file """
    build_dir = ""
    for potential_dir in [os.getcwd(), __this_file_directory]:
        head = potential_dir
        tail = ""
        while True:
            # find a file that is unique to the ELL build folder
            # (this works if our search started somewhere in the build folder)
            test = os.path.join(head, "ell_build_tools.json")
            if os.path.isfile(test):
                return head

            # find a file that is unique to the ELL repo root.
            test = os.path.join(head, "StyleGuide.md")
            if os.path.isfile(test):
                # this happens if we are searching from outside the build folder, so in
                # order to support different named build folders, like 'build_gcc' or 'build_clang'
                # we match by "build" prefix.
                for d in os.listdir(head):
                    dd = os.path.join(head, d)
                    if d.startswith("build") and \
                        os.path.isdir(dd) and \
                            os.path.isfile(os.path.join(dd, "ell_build_tools.json")):  # noqa: E501
                        return dd
                print("ELL build folder not found in {}\nFound:{}".format(head, os.listdir(head)))

            newhead, tail = os.path.split(head)
            if newhead == head:
                break
            else:
                head = newhead
    return build_dir


def add_ell_root_args(arg_parser):
    default_ell_root = get_ell_root()
    default_ell_build_dir = find_ell_build()
    arg_parser.add_argument("--ell_root", help="Path to ELL root.", default=default_ell_root)
    arg_parser.add_argument("--ell_build_root", help="Path to ELL build directory.", default=default_ell_build_dir)


ell_build_dir = find_ell_build()
if not ell_build_dir:
    ell_root = os.getenv("ELL_ROOT")
    if not ell_root:
        raise ImportError("Could not determine ELL build directory and ELL_ROOT environment variable isn't set")
    ell_build_dir = os.path.join(ell_root, "build")
ell_py_dir = os.path.join(ell_build_dir, "interfaces", "python", "package")
if ell_py_dir is None:
    raise ImportError("Could not find ell package, did you follow the ELL Python Binding build instructions?")

sys.path.append(ell_py_dir)
