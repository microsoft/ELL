#
# SWIG r interfaces
#

CMAKE_MINIMUM_REQUIRED(VERSION 2.8.11)

include (CommonInterfaces)

generate_interface(r ${CMAKE_CURRENT_SOURCE_DIR} "" "")
add_dependencies(ELL_r ELL_common)

