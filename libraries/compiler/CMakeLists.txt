#
# cmake file
#

CMAKE_MINIMUM_REQUIRED(VERSION 2.8.11)

set (PROJ_NAME compiler)

PROJECT(${PROJ_NAME})

add_compile_options(-w14062 /Zc:inline /Zc:rvalueCast /Zc:sizedDealloc- /D_SCL_SECURE_NO_DEPRECATE /D_SCL_SECURE_NO_WARNINGS)
add_compile_options( /wd4141 /wd4146 /wd4180 /wd4244 /wd4258 /wd4267 /wd4291 /wd4345 /wd4351 /wd4355 /wd4456 /wd4457 /wd4458 /wd4459 /wd4503 /wd4624 /wd4722 /wd4800 /wd4100 /wd4127 /wd4512 /wd4505 /wd4610 /wd4510 /wd4702 /wd4245 /wd4706 /wd4310 /wd4701 /wd4703 /wd4389 /wd4611 /wd4805 /wd4204 /wd4577 /wd4091 /wd4592 /wd4319 /wd4324)

if(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES Clang)
  add_compile_options(-std=c++1y)
endif()

set (SRC src/Types.cpp
		 src/IREmitter.cpp
         src/IRModuleEmitter.cpp
         src/IRLoopEmitter.cpp
		 src/IRIfEmitter.cpp
         src/IRFunctionEmitter.cpp
		 src/ModelEx.cpp
		 src/Variable.cpp
		 src/Compiler.cpp
		 src/IRRuntime.cpp
		 src/IRCompiler.cpp
		 src/IndentedTextWriter.cpp
		 src/CppEmitter.cpp
		 src/CppBlock.cpp
		 src/CppModuleEmitter.cpp
		 src/CppFunctionEmitter.cpp
		 src/CppCompiler.cpp
)

set (INCLUDE include/LLVMInclude.h
		include/CompilerException.h
		include/Types.h
		include/SymbolTable.h
		include/IREmitter.h
		include/IRLoopEmitter.h
		include/IRIfEmitter.h
		include/IRFunctionEmitter.h
		include/IRModuleEmitter.h
		include/IRInclude.h
		include/ModelEx.h
		include/Variable.h
		include/ScalarVar.h
		include/VectorVar.h
		include/Compiler.h
		include/IRRuntime.h
		include/IRCompiler.h
		include/IndentedTextWriter.h
		include/CppEmitter.h
		include/CppBlock.h
		include/CppModuleEmitter.h
		include/CppFunctionEmitter.h
		include/CppCompiler.h
)

set (TCC tcc/SymbolTable.tcc
		 tcc/Variable.tcc
		 tcc/ScalarVar.tcc
		 tcc/VectorVar.tcc
		 tcc/IRModuleEmitter.tcc
		 tcc/IRFunctionEmitter.tcc
		 tcc/IRRuntime.tcc
		 tcc/Compiler.tcc
		 tcc/IRCompiler.tcc
		 tcc/CppEmitter.tcc
		 tcc/CppModuleEmitter.tcc
		 tcc/CppFunctionEmitter.tcc
		 tcc/CppCompiler.tcc
)

source_group("src" FILES ${SRC})
source_group("include" FILES ${INCLUDE})
source_group("tcc" FILES ${TCC})

include_directories(include)
include_directories(${CMAKE_SOURCE_DIR}/external/llvm/include)
include_directories(${CMAKE_SOURCE_DIR}/external/llvm/include/llvm)
include_directories(${CMAKE_SOURCE_DIR}/external/llvm/include/Support)
include_directories(${CMAKE_SOURCE_DIR}/external/llvm/include/machine)
include_directories(${CMAKE_SOURCE_DIR}/external/llvm/include/IR)

add_library(${PROJ_NAME} ${SRC} ${INCLUDE} ${TCC})
target_include_directories(${PROJ_NAME} PUBLIC include)
target_link_libraries(${PROJ_NAME} utilities model nodes)

set_property(TARGET ${PROJ_NAME} PROPERTY FOLDER "libraries")

#
# test project
#

set (TEST_NAME ${PROJ_NAME}_test)

PROJECT(${TEST_NAME})

if(MSVC)
	set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /ignore:4099")
endif()

set (TEST_SRC test/src/main.cpp
              test/src/CompilerTest.cpp
			  test/src/CppCompilerTest.cpp)

set (TEST_INCLUDE test/include/CompilerTest.h
				  test/include/CppCompilerTest.h)

source_group("src" FILES ${TEST_SRC})
source_group("include" FILES ${TEST_INCLUDE})

include_directories(../testing/include)
include_directories(test/include)

add_executable(${TEST_NAME} ${TEST_SRC} ${TEST_INCLUDE} ${INCLUDE})

# LLVM Dependencies

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

target_link_libraries(${TEST_NAME} nodes model testing utilities predictors compiler LLVMCore LLVMAnalysis LLVMAsmParser LLVMSupport LLVMBitWriter)

set_property(TARGET ${TEST_NAME} PROPERTY FOLDER "tests")

add_test(NAME ${TEST_NAME} COMMAND ${TEST_NAME})



