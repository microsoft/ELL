#
# cmake file
#

CMAKE_MINIMUM_REQUIRED(VERSION 2.8.11)

set (PROJ_NAME model)

PROJECT(${PROJ_NAME})

if(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES Clang)
  add_compile_options(-std=c++1y)
endif()

set (SRC src/ModelGraph.cpp
         src/ModelTransformer.cpp
         src/Node.cpp
         src/OutputPort.cpp
         src/OutputPortElements.cpp
         src/Port.cpp
)

set (INCLUDE include/ExtremalValueNode.h
             include/InputNode.h
             include/InputPort.h
             include/ModelGraph.h
             include/ModelTransformer.h
             include/Node.h
             include/OutputNode.h
             include/OutputPort.h
             include/OutputPortElements.h
             include/Port.h
             include/ValueSelectorNode.h
)

set (TCC tcc/ExtremalValueNode.tcc
         tcc/InputNode.tcc
         tcc/InputPort.tcc
         tcc/ModelGraph.tcc
         tcc/ModelTransformer.tcc
         tcc/OutputNode.tcc
         tcc/OutputPort.tcc
         tcc/OutputPortElements.tcc
         tcc/ValueSelectorNode.tcc
)

source_group("src" FILES ${SRC})
source_group("include" FILES ${INCLUDE})
source_group("tcc" FILES ${TCC})

add_library(${PROJ_NAME} ${SRC} ${INCLUDE} ${TCC})
target_include_directories(${PROJ_NAME} PUBLIC include)
target_link_libraries(${PROJ_NAME} utilities)

set_property(TARGET ${PROJ_NAME} PROPERTY FOLDER "libraries")

#
# test project
#

set (TEST_NAME ${PROJ_NAME}_test)

PROJECT(${TEST_NAME})

set (TEST_SRC test/src/main.cpp
              test/src/Graph_test.cpp
              test/src/OutputPortElements_test.cpp)
set (TEST_INCLUDE test/include/OutputPortElements_test.h
                  test/include/Graph_test.h)

source_group("src" FILES ${TEST_SRC})
source_group("include" FILES ${TEST_INCLUDE})

add_executable(${TEST_NAME} ${TEST_SRC} ${TEST_INCLUDE} ${INCLUDE})
target_include_directories(${TEST_NAME} PRIVATE test/include)
target_link_libraries(${TEST_NAME} common model nodes testing utilities)

set_property(TARGET ${TEST_NAME} PROPERTY FOLDER "tests")

add_test(NAME ${TEST_NAME} COMMAND ${TEST_NAME})
