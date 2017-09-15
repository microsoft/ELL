# Make sure the binaries can be imported
import os
import sys
sys.path.append(os.path.join(os.path.dirname(__file__), 'Release'))

def package_dir():
    return os.path.join(os.path.dirname(__file__))

try:
    from .rpi_magic import init_magics
    init_magics()
except ImportError:
    pass # we're in regular Python, not Jupyter

# Make all the definitions in ELL.py be toplevel in this package
from .ELL import *
