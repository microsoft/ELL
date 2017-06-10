#
# SWIG interface code common to all languages ("common" here doesn't mean the "common" library)
#
include (CommonInterfaces)

set (INTERFACE_SRC src/ModelInterface.cpp)
set (INTERFACE_TCC tcc/NeuralNetworkPredictorInterface.tcc)

generate_interface(common ${CMAKE_CURRENT_SOURCE_DIR} "" "" "")
