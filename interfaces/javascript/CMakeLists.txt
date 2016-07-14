#
# SWIG javascript-on-node interfaces
#

CMAKE_MINIMUM_REQUIRED(VERSION 2.8.11)

## Problem: compilation fails due to node.h, etc. missing
## Solutions... ??

include (CommonInterfaces)

generate_interface(javascript ${CMAKE_CURRENT_SOURCE_DIR} "" "")
add_dependencies(EMLL_javascript EMLL_common)
