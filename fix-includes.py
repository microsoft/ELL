#!/usr/bin/env python3

import os
import re
import sys
 
PROJ_NAME_REGEX = re.compile(r'(.*)/(include|src|tcc)')
INCLUDE_REGEX = re.compile(r'#include ([<"].*[">])')
INCLUDE_GROUP_REGEX = re.compile(r'// ?(\w+(?:/\w+)*)$')

# TODO: reformat include blocks. There's this in TrainerInterface.cpp:
#
#    #include "TrainerInterface.h"
#    #include <libraries/data/include/AutoDataVector.h>
#    #include <libraries/data/include/Dataset.h>
#    #include "DatasetInterfaceImpl.h"
#    #include <libraries/utilities/include/Exception.h>
#    #include <libraries/common/include/LoadModel.h>
#    #include <libraries/common/include/MakeTrainer.h>
#    #include <libraries/model/include/Map.h>
#    #include <libraries/common/include/MapLoadArguments.h>
#    #include "ModelInterface.h"
#    #include <libraries/nodes/include/ProtoNNPredictorNode.h>
#    #include <libraries/trainers/include/ProtoNNTrainer.h>

# returns a dict mapping header filenames to a set include paths (of the form "libraryname/include") that contain a file with that name
def get_headers(root_paths: str):
    result = {}
    for root_path in root_paths:
        for root, _, files in os.walk(root_path):
            libname = root
            for file in files:
                if file.endswith(".h"):
                    if file not in result:
                        result[file] = set()
                    result[file].add(libname)
    return result

def fix_files(root_path: str, headers: dict):
    for root, _, files in os.walk(root_path):
        for file in files:
            if file.endswith(".h") or file.endswith(".cpp") or file.endswith(".tcc"):
                fix_file(root, file, headers)
 
def fix_file(dir: str, file: str, headers: dict):
    filepath = os.path.join(dir, file)
    with open(filepath, mode='r') as code_file:
        code_contents = code_file.readlines()
 
    this_dir = get_project_header_dir(dir)
    new_code_contents = fix_include_lines(code_contents, file, this_dir, headers)
    with open(filepath, mode='w') as code_file:
        code_file.writelines(new_code_contents)
 
def get_project_header_dir(dirname):
    m = PROJ_NAME_REGEX.match(dirname)
    if m:
        return m.groups()[0] + "/include"
    return ""
 
def fix_include_lines(code_contents, this_file, this_dir, headers):
    result = []
    for line in code_contents:
        result_lines = fix_include_line(line, this_file, this_dir, headers)
        # remove block comment if necessary
        if len(result) > 0 and has_includes(result_lines):
            if is_include_group_header(result[-1]):
                result.pop()
        result += result_lines
    return result

def has_includes(lines):
    return len(lines) > 0 and "#include" in lines[-1] # -1 because sometimes the first entry in lines is a "#error" directive

def is_include_group_header(line):
    if not INCLUDE_GROUP_REGEX.match(line):
        return False
    # check for exceptions
    exceptions = ["// solutions", "// regularizers"]
    return line.strip() not in exceptions

def fix_include_line(line, this_file, this_dir, headers):
    m = INCLUDE_REGEX.match(line)
    if m:
        rest = line[m.end():]
        include_directive_arg = str(m[1])
        include_file: str = include_directive_arg[1:-1]
        if include_file in headers:
            result = []
            header_dirs = headers[include_file]

            # Don't emit an error if one of the possibilities is local
            if this_dir in header_dirs:
                header_dirs = [this_dir]

            if len(header_dirs) > 1:
                result.append('#error  Ambiguous include file\n')

            for header_dir in header_dirs:
                if header_dir == this_dir:
                    if this_file.endswith(".tcc"):
                        line_template = "#include \"../include/{1}\""
                    else:
                        line_template = "#include \"{1}\""
                else:
                    header_dir = header_dir.split('/', 1)[-1] # remove first component from path
                    line_template = "#include <{0}/{1}>"
                result.append(line_template.format(header_dir, include_file) + rest)
            return result
    
    return [line]
 
if __name__ == "__main__":
    if len(sys.argv) > 1:
        raise RuntimeError("Run this script from the ELL root directory")
    header_dirs = ["libraries", "tools", "interfaces"]
    headers = get_headers(header_dirs)

    code_dirs = ["libraries", "tools", "interfaces"]
    for dir in code_dirs:
        fix_files(dir, headers)
 