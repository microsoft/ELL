####################################################################################################
##
##  Project:  Embedded Learning Library (ELL)
##  File:     generate_index.py (gallery)
##  Authors:  Lisa Ong
##
##  Requires: Python 3.x
##
####################################################################################################
import os
import sys
import argparse
import glob
import re

class GenerateIndex:
    def __init__(self):
        self.arg_parser = argparse.ArgumentParser(
            "This script takes a model gallery directory and generates a simple markdown listing,\n"
            "handy for updating index.md")
        self.gallerydir = None
        self.outfile = 'index.md'
        self.inputfiles = []

    def parse_command_line(self, argv):
        # required arguments
        self.arg_parser.add_argument("gallerydir", help="the directory containing the model gallery files")

        # optional arguments
        self.arg_parser.add_argument("--outfile", help="path to the output filename", default=self.outfile)

        args = self.arg_parser.parse_args(argv)

        self.gallerydir = args.gallerydir
        self.outfile = args.outfile

    def get_gallery_files(self):
        os.chdir(self.gallerydir)
        self.inputfiles = glob.glob("*.md")

    def find_first_match(self, line, regex, entry, key):
        if key not in entry:
            matches = re.search(regex, line)
            if matches and matches.group(1):
                entry[key] = matches.group(1)
        return entry

    def write_index(self):
        index = []

        for inputfile in self.inputfiles:
            entry = dict()

            with open(inputfile, 'r') as f:
                for line in f:
                    # perform the searches in sequential order of where they appear in the file
                    # get the permalink
                    entry = self.find_first_match(line, r'^permalink: (.+)', entry, 'link')

                    # get the title
                    entry = self.find_first_match(line, r'^title: (.+)', entry, 'title')

                    # get the subtitle
                    if 'title' in entry:
                        entry = self.find_first_match(line, r'{} (.+)'.format(entry['title']), entry, 'subtitle')
                    
                    # last entry
                    if 'subtitle' in entry:
                        index.append(entry)
                        break # go to the next file

        if len(index) > 0:
            with open(self.outfile, 'w') as o:
                for entry in index:
                    print('* [{} {}](/ELL{})'.format(entry['title'], entry['subtitle'], entry['link']), file=o)

    def run(self):
        self.get_gallery_files()
        self.write_index()

if __name__ == "__main__":
    program = GenerateIndex()

    argv = sys.argv
    argv.pop(0) # when passed directly into parse_args, the first argument (program name) is not skipped
    program.parse_command_line(argv)

    program.run()