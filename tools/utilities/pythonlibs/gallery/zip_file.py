####################################################################################################
##
##  Project:  Embedded Learning Library (ELL)
##  File:     zip_file.py (gallery)
##  Authors:  Lisa Ong
##
##  Requires: Python 3.x
##
####################################################################################################

import sys
import os
import argparse
sys.path.append(os.path.join(os.path.dirname(os.path.abspath(__file__)), '../'))
import ziptools

def main(argv):
    arg_parser = argparse.ArgumentParser(
        "Creates a zip file from a given input file\n"
        "Example:\n"
        "    zip_file.py mymodel.cntk\n"
        "This outputs 'mymodel.cntk.zip'\n")

    arg_parser.add_argument("file", help="path to a file to be zipped")
    args = arg_parser.parse_args(argv)

    output_file = args.file + ".zip"
    zipper = ziptools.Zipper()
    zipper.zip_file(args.file, output_file)
    print("Created file: '" + output_file)

if __name__ == "__main__":
    argv = sys.argv
    argv.pop(0) # when passed directly into parse_args, the first argument (program name) is not skipped
    main(argv)
