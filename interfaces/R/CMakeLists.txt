#
# SWIG r interfaces
#

include (CommonInterfaces)

generate_interface(r ${CMAKE_CURRENT_SOURCE_DIR} "" "")
add_dependencies(ELL_r ELL_common)

