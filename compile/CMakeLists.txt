#
# cmake file for compile project
#

CMAKE_MINIMUM_REQUIRED(VERSION 2.8.11)

# define project
set (PROJ_NAME compile)
PROJECT(${PROJ_NAME})

#enable C++11 in GCC, etc.
if(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES Clang)
  add_compile_options(-std=c++1y)
endif()

set (SRC src/main.cpp
         src/AddToAction.cpp
         src/CompilableCoordinatewise.cpp
         src/CompilableInput.cpp
         src/CompilableLayer.cpp
         src/CompilableMap.cpp
         src/CompilableSum.cpp
         src/ConstructLayer.cpp
         src/DataFlowGraph.cpp
         src/DataFlowNode.cpp
         src/LinearOperation.cpp)

set (INCLUDE include/AddToAction.h
             include/CompileArguments.h
             include/CompilableCoordinatewise.h
             include/CompilableInput.h
             include/CompilableLayer.h
             include/CompilableMap.h
             include/CompilableSum.h
             include/DataFlowGraph.h
             include/DataFlowNode.h
             include/LinearOperation.h)

set (TCC tcc/LinearOperation.tcc)

source_group("src" FILES ${SRC})
source_group("include" FILES ${INCLUDE})
source_group("tcc" FILES ${TCC})

include_directories(include)
include_directories(../libraries/dataset/include)
include_directories(../libraries/layers/include)
include_directories(../libraries/linear/include)
include_directories(../libraries/utilities/include)
include_directories(../libraries/common/include)

# create executable in build\bin
set (GLOBAL_BIN_DIR ${CMAKE_BINARY_DIR}/bin)
set (EXECUTABLE_OUTPUT_PATH ${GLOBAL_BIN_DIR}) 
add_executable(${PROJ_NAME} ${SRC} ${INCLUDE} ${TCC})

target_link_libraries(${PROJ_NAME} utilities layers linear common)

set_property(TARGET ${PROJ_NAME} PROPERTY FOLDER "tools")
