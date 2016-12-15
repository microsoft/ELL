#
# SWIG csharp interfaces
#
include (CommonInterfaces)

generate_interface(xml ${CMAKE_CURRENT_SOURCE_DIR} "" "" "")
add_dependencies(ELL_xml ELL_common)
