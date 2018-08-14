#
# SWIG interface code common to all languages ("common" here doesn't mean the "common" library)
#
include (CommonInterfaces)

set (INTERFACE_SRC src/ModelBuilderInterface.cpp
                   src/ModelInterface.cpp
                   src/NeuralNetworkPredictorInterface.cpp
                   src/TrainerInterface.cpp
                   src/DatasetInterface.cpp)
set (INTERFACE_TCC tcc/CallbackInterface.tcc
                   tcc/ModelInterface.tcc)

generate_interface(common ell ${CMAKE_CURRENT_SOURCE_DIR} "" "" "")
