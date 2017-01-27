#
# cmake file
#

CMAKE_MINIMUM_REQUIRED(VERSION 2.8.11)

set (PROJ_NAME trainers)

PROJECT(${PROJ_NAME})

if(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES Clang)
  add_compile_options(-std=c++1y)
endif()

set (SRC src/ForestTrainer.cpp
         src/LogitBooster.cpp
         src/ThresholdFinder.cpp)

set (INCLUDE include/EvaluatingIncrementalTrainer.h
             include/ForestTrainer.h
             include/HistogramForestTrainer.h
             include/ITrainer.h
             include/LogitBooster.h
             include/MultiEpochIncrementalTrainer.h
             include/SortingForestTrainer.h
             include/SweepingIncrementalTrainer.h
             include/SGDLinearTrainer.h
             include/SDSGDLinearTrainer.h
             include/ThresholdFinder.h)

set (TCC tcc/EvaluatingIncrementalTrainer.tcc
         tcc/ForestTrainer.tcc
         tcc/HistogramForestTrainer.tcc
         tcc/MultiEpochIncrementalTrainer.tcc
         tcc/SortingForestTrainer.tcc
         tcc/SweepingIncrementalTrainer.tcc
         tcc/SGDLinearTrainer.tcc
         tcc/SDSGDLinearTrainer.tcc
         tcc/ThresholdFinder.tcc)

set (DOC doc/README.md)

source_group("src" FILES ${SRC})
source_group("include" FILES ${INCLUDE})
source_group("tcc" FILES ${TCC})
source_group("doc" FILES ${DOC})

add_library(${PROJ_NAME} ${SRC} ${INCLUDE} ${TCC} ${DOC})
target_include_directories(${PROJ_NAME} PUBLIC include)
if(CMAKE_COMPILER_IS_GNUCXX)
  target_compile_options(${PROJ_NAME} PRIVATE -fPIC)
endif()
target_link_libraries(${PROJ_NAME} evaluators predictors)
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

add_executable(${TEST_NAME} ${TEST_SRC} ${TEST_INCLUDE} ${INCLUDE})
target_include_directories(${TEST_NAME} PRIVATE test/include)
target_link_libraries(${TEST_NAME} trainers)

set_property(TARGET ${TEST_NAME} PROPERTY FOLDER "tests")

add_test(NAME ${TEST_NAME} COMMAND ${TEST_NAME})
