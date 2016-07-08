#
# cmake file
#

CMAKE_MINIMUM_REQUIRED(VERSION 2.8.11)

set (PROJ_NAME predictors)

PROJECT(${PROJ_NAME})

if(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES Clang)
  add_compile_options(-std=c++1y)
endif()

set (SRC src/ConstantPredictor.cpp
         src/DecisionTreePredictor.cpp
         src/LinearPredictor.cpp
         src/SingleInputThresholdRule.cpp)

set (INCLUDE include/ConstantPredictor.h
             include/DecisionTreePredictor.h
             include/EnsemblePredictor.h
             include/LinearPredictor.h
             include/SingleInputThresholdRule.h
             include/ForestPredictor.h)

set (TCC tcc/EnsemblePredictor.tcc
         tcc/SingleInputThresholdRule.tcc
         tcc/ForestPredictor.tcc)

source_group("src" FILES ${SRC})
source_group("include" FILES ${INCLUDE})
source_group("tcc" FILES ${TCC})

add_library(${PROJ_NAME} ${SRC} ${INCLUDE} ${TCC})
target_include_directories(${PROJ_NAME} PUBLIC include)
target_link_libraries(${PROJ_NAME} dataset layers)

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

add_executable(${TEST_NAME} ${TEST_SRC} ${TEST_INCLUDE} ${INCLUDE} ${TCC})
target_include_directories(${TEST_NAME} PRIVATE test/include)

target_link_libraries(${TEST_NAME} testing predictors dataset linear)

set_property(TARGET ${TEST_NAME} PROPERTY FOLDER "tests")

add_test(NAME ${TEST_NAME} COMMAND ${TEST_NAME})

