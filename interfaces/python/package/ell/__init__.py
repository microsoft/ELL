# Make sure the binaries can be imported
import os
import sys
sys.path.append(os.path.join(os.path.dirname(__file__), 'Release'))

from .ellmagics import init_magics
init_magics()

# Make all the definitions in ELL.py be toplevel in this package
from .ELL import *
