#
# LLVMSetup
#

# Centralized place to define LLVM variables that we can leverage in components with dependencies on the emittersLib
# Sets the following variables:
#
# General information:
# LLVM_FOUND
# LLVM_PACKAGE_VERSION
#
# Settings for compiling against LLVM libraries:
# LLVM_DEFINITIONS
# LLVM_COMPILE_OPTIONS
# LLVM_INCLUDE_DIRS
# LLVM_LIBRARY_DIRS
# LLVM_LIBS
#
# Info about how LLVM was built:
# LLVM_ENABLE_ASSERTIONS
# LLVM_ENABLE_EH
# LLVM_ENABLE_RTTI
#
# Location of the executable tools:
# LLVM_TOOLS_BINARY_DIR
#
# Misc:
# LLVM_CMAKE_DIR

# Include guard so we don't try to find or download LLVM more than once
if(LLVMSetup_included)
    return()
endif()
set(LLVMSetup_included true)

if(WIN32)
    set(LLVM_PACKAGE_NAME LLVMNativeWindowsLibs.x64)
    set(LLVM_PACKAGE_VERSION 6.0.1)
    set(LLVM_PACKAGE_DIR ${PACKAGE_ROOT}/${LLVM_PACKAGE_NAME}.${LLVM_PACKAGE_VERSION})

    set(LLVM_COMPILE_OPTIONS /wd4141 /wd4146 /wd4180 /wd4244 /wd4258 /wd4267 /wd4291 /wd4345 /wd4351 /wd4355 /wd4456 /wd4457 /wd4458 /wd4459 /wd4503 /wd4624 /wd4722 /wd4800 /wd4100 /wd4127 /wd4512 /wd4505 /wd4610 /wd4510 /wd4702 /wd4245 /wd4706 /wd4310 /wd4701 /wd4703 /wd4389 /wd4611 /wd4805 /wd4204 /wd4577 /wd4091 /wd4592 /wd4319 /wd4324 /wd4996)

    set(LLVM_PATH ${LLVM_PACKAGE_DIR})
endif()

find_package(LLVM 6 REQUIRED CONFIG PATHS ${LLVM_PATH})

message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")

include_directories(${LLVM_INCLUDE_DIRS})
add_definitions(${LLVM_DEFINITIONS})

# llvm_map_components_to_libnames(LLVM_LIBS support core irreader AsmPrinter X86AsmPrinter ExecutionEngine)

set(LLVM_LIBS ${LLVM_AVAILABLE_LIBS})
list(FILTER LLVM_LIBS INCLUDE REGEX "LLVM.+")
