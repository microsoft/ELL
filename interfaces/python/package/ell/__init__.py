# Copyright (c) Microsoft. All rights reserved.
# Licensed under the MIT license. See LICENSE.md file in the project root
# for full license information.
# ==============================================================================
 
# Make sure the binaries can be imported
import os
import sys
sys.path.append(os.path.join(os.path.dirname(__file__), 'Release'))

__this_file_directory = os.path.dirname(os.path.abspath(__file__))

def package_dir():
    """Return the path containing this ell module """
    import os
    return os.path.join(os.path.dirname(__file__))
    
try:
    from .rpi_magic import init_magics
    init_magics()
except ImportError:
    del package_dir
    pass # we're in regular Python, not Jupyter

# The SWIG generated wrappers are divided into pseudo-namespace sub packages.
from . import data
from . import math
from . import model
from . import nodes
from . import neural
from . import trainers

del os
del sys
