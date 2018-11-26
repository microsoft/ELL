#!/usr/bin/env python3

from pathlib import Path
import re
import sys

INCLUDE_REGEX = re.compile(r'#include ([<"].*[">])$')
INCLUDE_GROUP_REGEX = re.compile(r'// (\w+)$')
NAMESPACE_REGEX = re.compile('namespace')


def fix_includes(filepath: str):
    with open(filepath, mode='r') as code_file:
        code_contents = code_file.readlines()
    include_group = ''
    new_code_contents = []
    for idx, line in enumerate(code_contents):
        if NAMESPACE_REGEX.match(line):
            new_code_contents += code_contents[idx:]
            break
        m = INCLUDE_REGEX.match(line)
        if not m:
            m = INCLUDE_GROUP_REGEX.match(line)
            if m:
                include_group = m[1]
                if not INCLUDE_REGEX.match(code_contents[idx + 1]):
                    new_code_contents.append(line)
            else:
                new_code_contents.append(line)
            continue

        include_directive_arg = str(m[1])
        include_file: str = include_directive_arg[1:-1]
        if include_directive_arg[0] == '<' or include_file.endswith('.h') == False:
            new_code_contents.append(line)
            continue

        basepath = Path(filepath).parent
        if (basepath/include_file).exists() or (basepath/'../include'/include_file).exists():
            new_code_contents.append(line)
            continue

        new_code_contents.append(
            '#include <{0}/include/{1}>\n'.format(include_group, include_file))

    with open(filepath, mode='w') as code_file:
        code_file.writelines(new_code_contents)


if __name__ == "__main__":
    if len(sys.argv) <= 1:
        raise RuntimeError("Must provide a path to a file")
    fix_includes(sys.argv[1])
