#
# cmake file for optimization library
#

CMAKE_MINIMUM_REQUIRED(VERSION 2.8.5)
PROJECT(optimization)

set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++1y")

set (SRC src/AsgdOptimizer.cpp)

set (INCLUDE include/AsgdOptimizer.h)

set (TCC tcc/AsgdOptimizer.tcc)

source_group("src" FILES ${SRC})
source_group("include" FILES ${INCLUDE})
source_group("tcc" FILES ${TCC})

include_directories(include)
include_directories(tcc)
include_directories(../common/include)
include_directories(../linear/include)
include_directories(../layers/include)
include_directories(../predictors/include)
include_directories(../utilities/include)

add_library(optimization ${SRC} ${INCLUDE} ${TCC})
