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
                 src/CompilableInput.cpp
                 src/CompilableScale.cpp
                 src/CompilableShift.cpp
                 src/CompilableSum.cpp
                 src/Compiler.cpp)


set (INCLUDE include/Action.h
             include/CommandLineArguments.h
             include/CompilableInput.h
             include/CompilableScale.h
             include/CompilableShift.h
             include/CompilableSum.h
             include/Compiler.h
             include/Compilable.h)

source_group("src" FILES ${SRC})
source_group("include" FILES ${INCLUDE})

include_directories(include)
include_directories(../libraries/layers/include)
include_directories(../libraries/types/include)
include_directories(../libraries/utilities/include)

# create executable in build\bin
set (GLOBAL_BIN_DIR ${CMAKE_BINARY_DIR}/bin)
set (EXECUTABLE_OUTPUT_PATH ${GLOBAL_BIN_DIR}) 
add_executable(${PROJ_NAME} ${SRC} ${INCLUDE})

target_link_libraries(${PROJ_NAME} utilities layers)
