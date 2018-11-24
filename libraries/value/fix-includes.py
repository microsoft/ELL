#!/usr/bin/env python3

import os
import re
import sys

INCLUDE_REGEX = re.compile(r'#include ([<"].*[">])')

def fix_includes(filepath: str):
    with open(filepath, mode='r') as code_file:
        code_contents = code_file.readlines()
    new_code_contents = []
    current_library = ""
    for line in in code_contents:
        m = INCLUDE_REGEX.match(line)
        if not m:
            new_code_contents.append(line)
            continue
        basepath, _ os.path.split(filepath)
        include_file_string = m[1]
        if include_file_string[0] == '<':
            new_code_contents.append(line)
            continue
        include_file = include_file_string[1:-1]
        if any(map(lambda ))



if __name__ == "__main__":
    if len(sys.argv) != 2:
        raise RuntimeError("Must provide a path to a C++ file")
    fix_includes(sys.argv[1])
