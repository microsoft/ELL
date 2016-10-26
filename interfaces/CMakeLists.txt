#
# SWIG interfaces
#
include (CommonInterfaces)

# Get include directories for LLVM
if(WIN32)
  include (LLVMSetup)
else()
  find_package(LLVM OPTIONAL CONFIG)
  if(LLVM_FOUND)
    include_directories(${LLVM_INCLUDE_DIRS})
    add_definitions(${LLVM_DEFINITIONS})
  endif()
endif()

# set (INTERFACE_SRC ../common/src/DataLoadersInterface.cpp)
# set (INTERFACE_INCLUDE ../common/include/DataLoadersInterface.h)

set (INTERFACE_MAIN ../common/EMLL.i)

set (INTERFACE_FILES ../common/common.i
                     ../common/callback.i
                     ../common/loadModelAsync.i
                     ../common/loadDatasetAsync.i
                     ../common/dataset.i
                     ../common/evaluators.i
                     ../common/lossFunctions.i
                     ../common/model.i
                     ../common/nodes.i
                     ../common/noncopyable.i
                     ../common/trainers.i
                     ../common/predictors.i
                     ../common/unique_ptr.i
                     ../common/utilities.i)

set (INTERFACE_DEPENDENCIES common evaluators lossFunctions trainers predictors testing treeLayout utilities)

# Add EMLL library include directories
include_directories(../libraries/common/include)
include_directories(../libraries/data/include)
include_directories(../libraries/evaluators/include)
include_directories(../libraries/lossFunctions/include)
include_directories(../libraries/model/include)
include_directories(../libraries/nodes/include)
include_directories(../libraries/predictors/include)
include_directories(../libraries/trainers/include)
include_directories(../libraries/utilities/include)
include_directories(common/include)

add_subdirectory(common)
add_subdirectory(python)
# add_subdirectory(R) # currently doesn't compile
# add_subdirectory(csharp)
add_subdirectory(javascript)
add_subdirectory(xml) 

