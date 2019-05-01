# Copyright (c) Microsoft. All rights reserved.
# Licensed under the MIT license. See LICENSE.md file in the project root
# for full license information.
# ==============================================================================
"""
ELL model classes
"""

from ..ell_py import \
    CompiledMap,\
    Map,\
    MapCompilerOptions,\
    Model,\
    ModelBuilder,\
    ModelOptimizerOptions,\
    PortMemoryLayout


# Provide correct behavior of != and == on PortMemoryLayouts.
PortMemoryLayout.__eq__ = PortMemoryLayout.IsEqual 

def PortMemoryLayoutNotEqual(a,b):
    return not a.IsEqual(b)

PortMemoryLayout.__ne__ = PortMemoryLayoutNotEqual 

del PortMemoryLayoutNotEqual

def PortMemoryLayoutToString(m):
    return "size: {}, extent: {}, offset: {}, order: {}".format(list(m.size), list(m.extent), lisinstance(m.offset), list(m.order))

PortMemoryLayout.ToString = PortMemoryLayoutToString

del PortMemoryLayoutToString