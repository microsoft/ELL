#
# cmake file
#

CMAKE_MINIMUM_REQUIRED(VERSION 2.8.11)

set (PROJ_NAME nodes)

PROJECT(${PROJ_NAME})

if(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES Clang)
  add_compile_options(-std=c++1y)
endif()

set (INCLUDE include/AccumulatorNode.h
             include/BinaryOperationNode.h
             include/ConstantNode.h
             include/DelayNode.h
             include/DotProductNode.h
             include/LinearPredictorNode.h
             include/MagnitudeNode.h
             include/MovingAverageNode.h
             include/SumNode.h
             include/UnaryOperationNode.h
             include/MovingVarianceNode.h)

set (SRC src/LinearPredictorNode.cpp)

set (TCC tcc/AccumulatorNode.tcc
         tcc/BinaryOperationNode.tcc
         tcc/ConstantNode.tcc
         tcc/DelayNode.tcc
         tcc/DotProductNode.tcc
         tcc/MagnitudeNode.tcc
         tcc/MovingAverageNode.tcc
         tcc/SumNode.tcc
         tcc/UnaryOperationNode.tcc
         tcc/MovingVarianceNode.tcc)

source_group("include" FILES ${INCLUDE})
source_group("src" FILES ${SRC})
source_group("tcc" FILES ${TCC})

add_library(${PROJ_NAME} ${SRC} ${INCLUDE} ${TCC})
target_include_directories(${PROJ_NAME} PUBLIC include)
target_link_libraries(${PROJ_NAME} model predictors utilities)

set_property(TARGET ${PROJ_NAME} PROPERTY FOLDER "libraries")

#
# test project
#

set (TEST_NAME ${PROJ_NAME}_test)

PROJECT(${TEST_NAME})

set (TEST_SRC test/src/main.cpp 
              test/src/Nodes_test.cpp)
set (TEST_INCLUDE test/include/Nodes_test.h)

source_group("src" FILES ${TEST_SRC})
source_group("include" FILES ${TEST_INCLUDE})

add_executable(${TEST_NAME} ${TEST_SRC} ${TEST_INCLUDE} ${INCLUDE})
target_include_directories(${TEST_NAME} PRIVATE test/include)
target_link_libraries(${TEST_NAME} testing linear model nodes predictors utilities)

set_property(TARGET ${TEST_NAME} PROPERTY FOLDER "tests")

add_test(NAME ${TEST_NAME} COMMAND ${TEST_NAME})
