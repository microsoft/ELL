import sys

sys.path.append('.')
sys.path.append('../../interfaces/python')
sys.path.append('../../interfaces/python/Release')
sys.path.append('../../interfaces/python/Debug')

import lossfunctiontest
import graphtest

tests = [lossfunctiontest.test, graphtest.test]

def swig_test():
    rc = 0
    for test in tests:
        rc = rc | test()
    sys.exit(rc)

swig_test()
