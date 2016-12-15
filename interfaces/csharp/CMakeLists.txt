#
# SWIG csharp interfaces
#
include (CommonInterfaces)

generate_interface(csharp ${CMAKE_CURRENT_SOURCE_DIR} "" "" "")
add_dependencies(ELL_csharp ELL_common)
