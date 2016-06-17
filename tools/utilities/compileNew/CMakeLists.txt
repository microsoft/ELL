#
# cmake file for compile project
#

CMAKE_MINIMUM_REQUIRED(VERSION 2.8.11)

# define project
set (PROJ_NAME compileNew)
PROJECT(${PROJ_NAME})

add_compile_options(-w14062 /Zc:inline /Zc:strictStrings /Zc:rvalueCast /Zc:sizedDealloc- /D_SCL_SECURE_NO_DEPRECATE /D_SCL_SECURE_NO_WARNINGS)
add_compile_options( /wd4141 /wd4146 /wd4180 /wd4244 /wd4258 /wd4267 /wd4291 /wd4345 /wd4351 /wd4355 /wd4456 /wd4457 /wd4458 /wd4459 /wd4503 /wd4624 /wd4722 /wd4800 /wd4100 /wd4127 /wd4512 /wd4505 /wd4610 /wd4510 /wd4702 /wd4245 /wd4706 /wd4310 /wd4701 /wd4703 /wd4389 /wd4611 /wd4805 /wd4204 /wd4577 /wd4091 /wd4592 /wd4319 /wd4324)

if(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES Clang)
  add_compile_options(-std=c++1y)
endif()

#enable C++11 in GCC, etc.
if(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES Clang)
  add_compile_options(-std=c++1y)
endif()

set (SRC src/main.cpp
         src/AddToAction.cpp
         src/CompilableCoordinatewise.cpp
         src/CompilableLayer.cpp
         src/CompilableMap.cpp
         src/CompilableSum.cpp
         src/DataFlowGraph.cpp
         src/DataFlowNode.cpp
         src/LinearOperation.cpp
		 src/CodeEmitter.cpp)

set (INCLUDE include/AddToAction.h
             include/CompileArguments.h
             include/CompilableCoordinatewise.h
             include/CompilableLayer.h
             include/CompilableMap.h
             include/CompilableSum.h
             include/DataFlowGraph.h
             include/DataFlowNode.h
             include/LinearOperation.h
			 include/CodeEmitter.h)

set (TCC tcc/LinearOperation.tcc)

source_group("src" FILES ${SRC})
source_group("include" FILES ${INCLUDE})
source_group("tcc" FILES ${TCC})

include_directories(include)
include_directories(${CMAKE_SOURCE_DIR}/libraries/dataset/include)
include_directories(${CMAKE_SOURCE_DIR}/libraries/layers/include)
include_directories(${CMAKE_SOURCE_DIR}/libraries/linear/include)
include_directories(${CMAKE_SOURCE_DIR}/libraries/utilities/include)
include_directories(${CMAKE_SOURCE_DIR}/libraries/common/include)
include_directories(${CMAKE_SOURCE_DIR}/libraries/compiler/include)
include_directories(${CMAKE_SOURCE_DIR}/external/llvm/include)
include_directories(${CMAKE_SOURCE_DIR}/external/llvm/include/llvm)
include_directories(${CMAKE_SOURCE_DIR}/external/llvm/include/Support)
include_directories(${CMAKE_SOURCE_DIR}/external/llvm/include/machine)
include_directories(${CMAKE_SOURCE_DIR}/external/llvm/include/IR)

# create executable in build\bin
set (GLOBAL_BIN_DIR ${CMAKE_BINARY_DIR}/bin)
set (EXECUTABLE_OUTPUT_PATH ${GLOBAL_BIN_DIR}) 
add_executable(${PROJ_NAME} ${SRC} ${INCLUDE} ${TCC})

link_directories(${CMAKE_SOURCE_DIR}/external/llvm/lib)

add_library(LLVMAnalysis STATIC IMPORTED)
set_property(TARGET LLVMAnalysis PROPERTY IMPORTED_LOCATION ${CMAKE_SOURCE_DIR}/external/llvm/lib/LLVMAnalysis.lib)
add_library(LLVMAsmParser STATIC IMPORTED)
set_property(TARGET LLVMAsmParser PROPERTY IMPORTED_LOCATION ${CMAKE_SOURCE_DIR}/external/llvm/lib/LLVMAsmParser.lib)
add_library(LLVMCore STATIC IMPORTED)
set_property(TARGET LLVMCore PROPERTY IMPORTED_LOCATION ${CMAKE_SOURCE_DIR}/external/llvm/lib/LLVMCore.lib)
add_library(LLVMSupport STATIC IMPORTED)
set_property(TARGET LLVMSupport PROPERTY IMPORTED_LOCATION ${CMAKE_SOURCE_DIR}/external/llvm/lib/LLVMSupport.lib)
add_library(LLVMBitWriter STATIC IMPORTED)
set_property(TARGET LLVMBitWriter PROPERTY IMPORTED_LOCATION ${CMAKE_SOURCE_DIR}/external/llvm/lib/LLVMBitWriter.lib)

target_link_libraries(${PROJ_NAME} utilities layers linear common compiler LLVMCore LLVMAnalysis LLVMAsmParser LLVMSupport LLVMBitWriter)

set_property(TARGET ${PROJ_NAME} PROPERTY FOLDER "tools/utilities")
