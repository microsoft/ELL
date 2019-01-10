# Copyright (c) Microsoft. All rights reserved.
# Licensed under the MIT license. See LICENSE.md file in the project root
# for full license information.
# ==============================================================================
"""
ELL nodes
"""

from ..ell_py import BinaryOperationType,\
InputNode,\
InputPort,\
InputPortIterator, \
Node,\
NodeIterator,\
OutputNode,\
OutputPort,\
OutputPortIterator,\
Port,\
PortElement,\
PortElements,\
PortElementsList,\
PortType,\
UnaryOperationType


def IteratorHelper(iter):
    while iter.IsValid():
        yield iter.Get()
        iter.Next()

NodeIterator.__iter__ = IteratorHelper
InputPortIterator.__iter__ = IteratorHelper
OutputPortIterator.__iter__ = IteratorHelper

del IteratorHelper