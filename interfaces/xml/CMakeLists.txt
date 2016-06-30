#
# SWIG csharp interfaces
#
include (CommonInterfaces)

generate_interface(xml ${CMAKE_CURRENT_SOURCE_DIR} "" "")
add_dependencies(EMLL_xml EMLL_common)
