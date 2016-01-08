#
# cmake file for compile project
#

CMAKE_MINIMUM_REQUIRED(VERSION 2.8.5)
PROJECT(compile)

set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++1y")

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

add_executable(compile ${SRC} ${INCLUDE})
target_link_libraries(compile utilities layers)
