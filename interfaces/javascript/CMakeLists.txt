#
# SWIG javascript-on-node interfaces
#

CMAKE_MINIMUM_REQUIRED(VERSION 2.8.11)

include (CommonInterfaces)

generate_interface(javascript ${CMAKE_CURRENT_SOURCE_DIR} "" "")
add_dependencies(EMLL_javascript EMLL_common)

add_subdirectory(emll_module)
add_subdirectory(Swig2Typescript)
