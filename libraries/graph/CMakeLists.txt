#
# cmake file
#

CMAKE_MINIMUM_REQUIRED(VERSION 2.8.11)

set (PROJ_NAME graph)

PROJECT(${PROJ_NAME})

if(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES Clang)
  add_compile_options(-std=c++1y)
endif()

set (SRC src/DirectedGraph.cpp
         src/Node.cpp
         src/Port.cpp
         src/UniqueId.cpp
)

set (INCLUDE include/DirectedGraph.h
             include/Node.h
             include/Port.h
             include/InputPort.h
             include/OutputPort.h
             include/ConstantNode.h
             include/InputNode.h
             include/ExtremalValueNode.h
             include/UniqueId.h
             include/ValueSelectorNode.h
)

set (TCC tcc/Node.tcc
         tcc/InputPort.tcc
         tcc/DirectedGraph.tcc
         tcc/ConstantNode.tcc
         tcc/InputNode.tcc
         tcc/ExtremalValueNode.tcc
         tcc/ValueSelectorNode.tcc
)

source_group("src" FILES ${SRC})
source_group("include" FILES ${INCLUDE})
source_group("tcc" FILES ${TCC})

include_directories(include)

add_library(${PROJ_NAME} ${SRC} ${INCLUDE} ${TCC})

set_property(TARGET ${PROJ_NAME} PROPERTY FOLDER "libraries")

#
# test project
#

set (TEST_NAME ${PROJ_NAME}_test)

PROJECT(${TEST_NAME})

set (TEST_SRC test/src/main.cpp
              test/src/Graph_test.cpp)
set (TEST_INCLUDE test/include/Graph_test.h)

source_group("src" FILES ${TEST_SRC})
source_group("include" FILES ${TEST_INCLUDE})

include_directories(../testing/include)
include_directories(test/include)

add_executable(${TEST_NAME} ${TEST_SRC} ${TEST_INCLUDE} ${INCLUDE})

target_link_libraries(${TEST_NAME} graph testing)

set_property(TARGET ${TEST_NAME} PROPERTY FOLDER "tests")

add_test(NAME ${TEST_NAME} COMMAND ${TEST_NAME})
