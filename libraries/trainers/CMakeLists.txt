#
# cmake file
#

CMAKE_MINIMUM_REQUIRED(VERSION 2.8.11)

set (PROJ_NAME trainers)

PROJECT(${PROJ_NAME})

if(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES Clang)
  add_compile_options(-std=c++1y)
endif()

set (SRC src/Nothing.cpp)

set (INCLUDE include/BaggingIncrementalTrainer.h
             include/IIncrementalTrainer.h
             include/ITrainer.h
             include/MultiEpochIncrementalTrainer.h
             include/SortingTreeTrainer.h
             include/SweepingIncrementalTrainer.h
             include/SGDIncrementalTrainer.h)

set (TCC tcc/BaggingIncrementalTrainer.tcc
         tcc/MultiEpochIncrementalTrainer.tcc
         tcc/SortingTreeTrainer.tcc
         tcc/SweepingIncrementalTrainer.tcc
         tcc/SGDIncrementalTrainer.tcc)

source_group("src" FILES ${SRC})
source_group("include" FILES ${INCLUDE})
source_group("tcc" FILES ${TCC})

include_directories(include)
include_directories(../linear/include)
include_directories(../dataset/include)
include_directories(../layers/include)
include_directories(../predictors/include)
include_directories(../evaluators/include)
include_directories(../utilities/include)

add_library(${PROJ_NAME} ${SRC} ${INCLUDE} ${TCC})

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
