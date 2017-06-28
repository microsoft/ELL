#
# SWIG interfaces
#
include (CommonInterfaces)

# Get include directories for LLVM
include (LLVMSetup)
if(LLVM_FOUND)
    include_directories(${LLVM_INCLUDE_DIRS})
    add_definitions(${LLVM_DEFINITIONS})
    add_compile_options(${LLVM_COMPILE_OPTIONS})
endif()

set (INTERFACE_SRC 
    ../common/src/ModelInterface.cpp)
set (INTERFACE_INCLUDE 
    ../common/include/CallbackInterface.h
    ../common/include/MathInterface.h
    ../common/include/ModelInterface.h
    ../common/include/NeuralLayersInterface.h
    ../common/include/NeuralNetworkPredictorInterface.h
)
set (INTERFACE_TCC
    ../common/tcc/CallbackInterface.tcc
    ../common/tcc/NeuralNetworkPredictorInterface.tcc)

set (INTERFACE_MAIN ../common/ELL.i)

set (INTERFACE_FILES ../common/common.i
                     ../common/callback.i
                     ../common/callback_javascript_post.i
                     ../common/callback_javascript_pre.i
                     ../common/callback_python_post.i
                     ../common/callback_python_pre.i
                     ../common/loadModelAsync.i
                     ../common/loadDatasetAsync.i
                     ../common/dataset.i
                     ../common/evaluators.i
                     ../common/functions.i
                     ../common/math.i
                     ../common/math_javascript_post.i
                     ../common/math_javascript_pre.i
                     ../common/math_python_post.i
                     ../common/math_python_pre.i
                     ../common/model.i
                     ../common/nodes.i
                     ../common/noncopyable.i
                     ../common/trainers.i
                     ../common/predictors.i
                     ../common/predictors_javascript_post.i
                     ../common/predictors_javascript_pre.i
                     ../common/predictors_python_post.i
                     ../common/predictors_python_pre.i
                     ../common/unique_ptr.i
                     ../common/utilities.i
                     ../common/vector.i
                     ../common/vector_javascript_post.i
                     ../common/vector_javascript_pre.i
                     ../common/vector_python_post.i
                     ../common/vector_python_pre.i)

set (INTERFACE_DEPENDENCIES common data emitters evaluators functions math trainers model nodes predictors trainers utilities)

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

add_subdirectory(common)
add_subdirectory(python)
# add_subdirectory(R) # currently doesn't compile
# add_subdirectory(csharp EXCLUDE_FROM_ALL)
add_subdirectory(javascript EXCLUDE_FROM_ALL)
add_subdirectory(xml EXCLUDE_FROM_ALL)
