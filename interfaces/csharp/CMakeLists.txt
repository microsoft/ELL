#
# SWIG csharp interfaces
#
include (CommonInterfaces)

generate_interface(csharp ${CMAKE_CURRENT_SOURCE_DIR} "" "")
add_dependencies(Rockmill_csharp Rockmill_common)
