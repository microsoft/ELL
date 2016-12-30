#
# SWIG javascript-on-node interfaces
#

CMAKE_MINIMUM_REQUIRED(VERSION 2.8.11)

include (CommonInterfaces)

generate_interface(javascript ${CMAKE_CURRENT_SOURCE_DIR} "" "")
add_dependencies(ELL_javascript ELL_common)

add_subdirectory(ell_module)
add_subdirectory(swigToTypescript)
