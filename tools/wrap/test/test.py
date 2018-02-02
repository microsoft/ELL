#!/usr/bin/env python3
####################################################################################################
##
##  Project:  Embedded Learning Library (ELL)
##  File:     test.py
##  Authors:  Chris Lovett
##
##  Requires: Python 3.x
##
####################################################################################################
tests = []

import os
import sys

SkipTests = False
try:
    script_path = os.path.dirname(os.path.abspath(__file__))
    sys.path += [
        os.path.join(script_path, '..', '..', '..', 'tools', 'utilities',
            'pythonlibs'),
        '.',
        '..']
    import find_ell
    import ell
    import wrap_test

    tests = [        
        (wrap_test.test, "wrap_test") 
    ]
except ImportError as err:
    if "Could not find ell package" in str(err):
        print("Python was not built, so skipping test")
        SkipTests = True
    else:
        raise err

def run_test():
    rc = 0
    for (test, name) in tests:
        try:
            ans = test()
            if ans == 0:
                print(name, "passed")
            else:
                print(name, "failed")
            rc |= ans
        except Exception as e:
            print(name, "failed")
            print("exception:", e)
            rc = 1
    sys.exit(rc)

if not SkipTests:
    run_test()

