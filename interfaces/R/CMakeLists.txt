#
# SWIG r interfaces
#

CMAKE_MINIMUM_REQUIRED(VERSION 2.8.11)

include (CommonInterfaces)

generate_interface(r ${CMAKE_CURRENT_SOURCE_DIR} "" "")
add_dependencies(EMLL_r EMLL_common)

