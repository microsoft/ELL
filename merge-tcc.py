#!/usr/bin/env python3

import sys
import re
import os

INCLUDE_REGEX = re.compile(r'^#include "(.*\.tcc)"$')


def consolidate_header(filepath: str):
    with open(filepath, mode='r') as header_file:
        header_contents = header_file.readlines()
    new_header_contents = []
    for line in header_contents:
        m = INCLUDE_REGEX.match(line)
        if not m:
            new_header_contents.append(line)
            continue
        basepath, _ = os.path.split(filepath)
        tcc_filepath = os.path.join(basepath, m[1])
        with open(tcc_filepath, mode='r') as tcc_file:
            tcc_contents = tcc_file.readlines()
        while tcc_contents[0].startswith("//"):
            tcc_contents.pop(0)
        new_header_contents += ["\n\n#pragma region implementation\n\n"] + \
            tcc_contents + ["\n\n#pragma endregion implementation\n"]
        os.remove(tcc_filepath)
    with open(filepath, mode='w') as header_file:
        header_file.writelines(new_header_contents)


if __name__ == "__main__":
    if len(sys.argv) != 2:
        raise RuntimeError("Must provide a path to a header")
    consolidate_header(sys.argv[1])
