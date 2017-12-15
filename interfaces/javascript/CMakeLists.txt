#
# SWIG javascript-on-node interfaces
#

include (CommonInterfaces)

generate_interface(javascript ell ${CMAKE_CURRENT_SOURCE_DIR} "" "")
add_dependencies(ELL_javascript ELL_common)

add_subdirectory(ell_module)
add_subdirectory(swigToTypescript)
