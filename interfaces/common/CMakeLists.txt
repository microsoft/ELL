#
# SWIG interface code common to all languages ("common" here doesn't mean the "common" library)
#
include (CommonInterfaces)

set (INTERFACE_SRC src/ModelInterface.cpp)

generate_interface(common ${CMAKE_CURRENT_SOURCE_DIR} "" "" "")
