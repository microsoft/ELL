import sys

sys.path.append('.')
sys.path.append('../../interfaces/python')
sys.path.append('../../interfaces/python/Release')
sys.path.append('../../interfaces/python/Debug')

import EMLL
from EMLL import *
import lossfunctiontest

def swig_test():
    if lossfunctiontest.test() == 0:
        sys.exit(0)
    else:
        sys.exit(1)

swig_test()
