#
# cmake file
#

CMAKE_MINIMUM_REQUIRED(VERSION 2.8.11)

set (PROJ_NAME evaluators)

PROJECT(${PROJ_NAME})

if(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES Clang)
  add_compile_options(-std=c++1y)
endif()

set (SRC src/AUCAggregator.cpp
         src/BinaryErrorAggregator.cpp)

set (INCLUDE include/AUCAggregator.h
             include/BinaryErrorAggregator.h
             include/Evaluator.h
             include/IncrementalEvaluator.h
             include/LossAggregator.h)

set (TCC tcc/Evaluator.tcc
         tcc/IncrementalEvaluator.tcc
         tcc/LossAggregator.tcc)

source_group("src" FILES ${SRC})
source_group("include" FILES ${INCLUDE})
source_group("tcc" FILES ${TCC})

add_library(${PROJ_NAME} ${SRC} ${INCLUDE} ${TCC})
target_include_directories(${PROJ_NAME} PUBLIC include)
target_link_libraries(${TEST_NAME} dataset)
set_property(TARGET ${PROJ_NAME} PROPERTY FOLDER "libraries")

#
# test project
#

set (TEST_NAME ${PROJ_NAME}_test)

PROJECT(${TEST_NAME})

set (TEST_SRC test/src/main.cpp)
set (TEST_INCLUDE )

source_group("src" FILES ${TEST_SRC})
source_group("include" FILES ${TEST_INCLUDE})

include_directories(../testing/include)
include_directories(test/include)

add_executable(${TEST_NAME} ${TEST_SRC} ${TEST_INCLUDE} ${INCLUDE})

target_link_libraries(${TEST_NAME} dataset linear)

set_property(TARGET ${TEST_NAME} PROPERTY FOLDER "tests")

add_test(NAME ${TEST_NAME} COMMAND ${TEST_NAME})
