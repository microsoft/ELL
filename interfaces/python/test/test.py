import sys

sys.path.append('.')
sys.path.append('./..')
sys.path.append('./../Release')
sys.path.append('./../Debug')

import lossfunctions_test
import model_test
tests = [lossfunctions_test.test, model_test.test]

def interface_test():
    rc = 0
    for test in tests:
        rc = rc | test()
    sys.exit(rc)

interface_test()
