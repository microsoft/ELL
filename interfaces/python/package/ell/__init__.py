# Copyright (c) Microsoft. All rights reserved.
# Licensed under the MIT license. See LICENSE.md file in the project root
# for full license information.
# ==============================================================================

# Make sure the binaries can be imported

import os
import sys

saved = list(sys.path)
sys.path += [os.path.join(os.path.dirname(__file__)),
             os.path.join(os.path.dirname(__file__), 'Release'),
             os.path.join(os.path.dirname(__file__), 'RelWithDebInfo'),
             os.path.join(os.path.dirname(__file__), 'Debug')]

__this_file_directory = os.path.dirname(os.path.abspath(__file__))


def package_dir():
    """Return the path containing this ell module """
    return os.path.dirname(__file__)


def ensure_openblas():
    if os.name == "nt":
        path = os.getenv("PATH")
        if "OpenBLASLibs" in path:
            return

        head,tail = os.path.split(__this_file_directory)
        while tail != "":
            # find the build tools info
            tools = os.path.join(head,"ell_build_tools.json")
            if (os.path.isfile(tools)):
                import json
                blas_libs = json.load(open(tools,"r"))["blas"] # ~/lib/libopenblas.dll.a
                blas_bin = os.path.join(os.path.dirname(os.path.dirname(blas_libs)), "bin")
                path += ";" + blas_bin
                os.putenv("PATH", path)
                return
            head,tail = os.path.split(head)

ensure_openblas()

# The SWIG generated wrappers are divided into pseudo-namespace sub packages.
from . import data
from . import math
from . import model
from . import nodes
from . import neural
from . import trainers

from . import platform

try:
    from .rpi_magic import init_magics
    init_magics()
except ImportError:
    pass # we're in regular Python, not Jupyter

# must come after we initialize rpi_magic.
from . import util

# restore path now that all ELL modules have been loaded.
sys.path = saved
del os
del sys
