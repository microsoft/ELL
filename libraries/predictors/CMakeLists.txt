#
# cmake file for predictors library
#

CMAKE_MINIMUM_REQUIRED(VERSION 2.8.5)
PROJECT(predictors)

set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++1y")

set (SRC src/MutableDecisionTree.cpp
         src/SharedLinearBinaryPredictor.cpp)

set (INCLUDE include/MutableDecisionTree.h
             include/SharedLinearBinaryPredictor.h)

set (TCC tcc/SharedLinearBinaryPredictor.tcc)

source_group("src" FILES ${SRC})
source_group("include" FILES ${INCLUDE})
source_group("tcc" FILES ${TCC})

include_directories(include)
include_directories(tcc)
include_directories(../common/include)
include_directories(../layers/include)
include_directories(../linear/include)
include_directories(../utilities/include)

add_library(predictors ${SRC} ${INCLUDE} ${TCC})
