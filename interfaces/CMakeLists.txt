#
# SWIG interfaces
#
include (CommonInterfaces)


###
if(WIN32)
  include (LLVMSetup)

  if(LLVM_LIBROOT)
    set(LLVM_ENABLED ON)
  else()
    set(LLVM_ENABLED OFF)
    message("LLVM Unavailable. Compilers disabled.")
  endif()

else()
  find_package(LLVM REQUIRED CONFIG)
  if(LLVM_FOUND)
    message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
    message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")
    message(STATUS "LLVM definitions: ${LLVM_DEFINITIONS}")
    include_directories(${LLVM_INCLUDE_DIRS})
    add_definitions(${LLVM_DEFINITIONS})
    # Find the libraries that correspond to the LLVM components
    # that we wish to use
    llvm_map_components_to_libnames(LLVM_LIBS all) #support core irreader)
#    set(LLVM_LIBS LLVMCore LLVMAnalysis LLVMAsmParser LLVMSupport LLVMBitWriter LLVMInstCombine LLVMTransformUtils LLVMScalarOpts LLVMExecutionEngine LLVMRuntimeDyld LLVMObject LLVMMC LLVMTarget LLVMMCParser LLVMBitReader LLVMCodeGen LLVMSelectionDAG LLVMAsmPrinter LLVMX86CodeGen LLVMX86Info LLVMX86Desc LLVMX86Utils LLVMX86AsmPrinter LLVMMCDisassembler LLVMDebugInfoCodeView LLVMMCJIT)

    message(STATUS "Libs: ${LLVM_LIBS}")
    set(LLVM_ENABLED ON)
  else()
    set(LLVM_ENABLED OFF)
  endif()

endif()

###

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
#include_directories(../libraries/dataset/include)
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

