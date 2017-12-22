#
# SWIG interfaces
#
include (CommonInterfaces)

# Get include directories for LLVM
include (LLVMSetup)
if(LLVM_FOUND)
    include_directories(SYSTEM ${LLVM_INCLUDE_DIRS})
    add_definitions(${LLVM_DEFINITIONS})
    add_compile_options(${LLVM_COMPILE_OPTIONS})
endif()

set(_sources
    common/src/DatasetInterface.cpp
    common/src/ModelInterface.cpp
    common/src/TrainerInterface.cpp
)
foreach(src ${_sources})
    list(APPEND INTERFACE_SRC "${CMAKE_CURRENT_LIST_DIR}/${src}")
endforeach()

set(_sources
    common/include/CallbackInterface.h
    common/include/DatasetInterface.h
    common/include/DatasetInterfaceImpl.h
    common/include/MathInterface.h
    common/include/ModelInterface.h
    common/include/NeuralLayersInterface.h
    common/include/NeuralNetworkPredictorInterface.h
    common/include/TrainerInterface.h
)
foreach(src ${_sources})
    list(APPEND INTERFACE_INCLUDE "${CMAKE_CURRENT_LIST_DIR}/${src}")
endforeach()

set(_sources
    common/tcc/CallbackInterface.tcc
    common/tcc/ModelInterface.tcc
    common/tcc/NeuralNetworkPredictorInterface.tcc
)
foreach(src ${_sources})
    list(APPEND INTERFACE_TCC "${CMAKE_CURRENT_LIST_DIR}/${src}")
endforeach()

set(INTERFACE_MAIN "${CMAKE_CURRENT_LIST_DIR}/common/ell.i")

set(_sources
    common/common.i
    common/callback.i
    common/callback_javascript_post.i
    common/callback_javascript_pre.i
    common/callback_python_post.i
    common/callback_python_pre.i
    common/loadModelAsync.i
    common/loadDatasetAsync.i
    common/dataset.i
    common/ELL_javascript_post.i
    common/ELL_javascript_pre.i
    common/ELL_python_post.i
    common/ELL_python_pre.i
    common/evaluators.i
    common/functions.i
    common/math.i
    common/math_javascript_post.i
    common/math_javascript_pre.i
    common/math_python_post.i
    common/math_python_pre.i
    common/macros.i
    common/model.i
    common/model_javascript_post.i
    common/model_python_post.i
    common/nodes.i
    common/noncopyable.i
    common/shape.i
    common/trainers.i
    common/predictors.i
    common/predictors_javascript_post.i
    common/predictors_javascript_pre.i
    common/predictors_python_post.i
    common/predictors_python_pre.i
    common/unique_ptr.i
    common/utilities.i
    common/vector.i
    common/vector_javascript_post.i
    common/vector_javascript_pre.i
    common/vector_python_post.i
    common/vector_python_pre.i
)
foreach(src ${_sources})
    list(APPEND INTERFACE_FILES "${CMAKE_CURRENT_LIST_DIR}/${src}")
endforeach()

# We're done using the temporary _sources variable
unset(_sources)

set(INTERFACE_DEPENDENCIES
    common
    data
    emitters
    evaluators
    functions
    math
    trainers
    model
    nodes
    predictors
    trainers
    utilities
)

# Add ELL library include directories
include_directories(common/include)
include_directories(../libraries/common/include)
include_directories(../libraries/data/include)
include_directories(../libraries/evaluators/include)
include_directories(../libraries/functions/include)
include_directories(../libraries/model/include)
include_directories(../libraries/nodes/include)
include_directories(../libraries/predictors/include)
include_directories(../libraries/predictors/neural/include)
include_directories(../libraries/trainers/include)
include_directories(../libraries/utilities/include)
include_directories(../libraries/math/include)

add_subdirectory(common)
add_subdirectory(python)
# add_subdirectory(R) # currently doesn't compile
# add_subdirectory(csharp EXCLUDE_FROM_ALL)
add_subdirectory(javascript)
add_subdirectory(xml)
