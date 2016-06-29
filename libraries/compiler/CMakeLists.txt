#
# cmake file
#

CMAKE_MINIMUM_REQUIRED(VERSION 2.8.11)

set (PROJ_NAME compiler)

PROJECT(${PROJ_NAME})

add_compile_options(-w14062 /Zc:inline /Zc:strictStrings /Zc:rvalueCast /Zc:sizedDealloc- /D_SCL_SECURE_NO_DEPRECATE /D_SCL_SECURE_NO_WARNINGS)
add_compile_options( /wd4141 /wd4146 /wd4180 /wd4244 /wd4258 /wd4267 /wd4291 /wd4345 /wd4351 /wd4355 /wd4456 /wd4457 /wd4458 /wd4459 /wd4503 /wd4624 /wd4722 /wd4800 /wd4100 /wd4127 /wd4512 /wd4505 /wd4610 /wd4510 /wd4702 /wd4245 /wd4706 /wd4310 /wd4701 /wd4703 /wd4389 /wd4611 /wd4805 /wd4204 /wd4577 /wd4091 /wd4592 /wd4319 /wd4324)

if(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES Clang)
  add_compile_options(-std=c++1y)
endif()

set (SRC src/IREmitter.cpp
         src/IRModuleEmitter.cpp
         src/IRLoopEmitter.cpp
		 src/IRIfEmitter.cpp
         src/IRFunctionEmitter.cpp
)

set (INCLUDE include/LLVMInclude.h
		include/EmitterException.h
		include/CompilerException.h
		include/Types.h
		include/SymbolTable.h
		include/IREmitter.h
		include/IRLoopEmitter.h
		include/IRIfEmitter.h
		include/IRFunctionEmitter.h
		include/IRModuleEmitter.h
		include/IRCompiler.h
)

set (TCC tcc/SymbolTable.tcc
)

source_group("src" FILES ${SRC})
source_group("include" FILES ${INCLUDE})
source_group("tcc" FILES ${TCC})

include_directories(include)
include_directories(../utilities/include)
include_directories(${CMAKE_SOURCE_DIR}/external/llvm/include)
include_directories(${CMAKE_SOURCE_DIR}/external/llvm/include/llvm)
include_directories(${CMAKE_SOURCE_DIR}/external/llvm/include/Support)
include_directories(${CMAKE_SOURCE_DIR}/external/llvm/include/machine)
include_directories(${CMAKE_SOURCE_DIR}/external/llvm/include/IR)

add_library(${PROJ_NAME} ${SRC} ${INCLUDE} ${TCC})

set_property(TARGET ${PROJ_NAME} PROPERTY FOLDER "libraries")

