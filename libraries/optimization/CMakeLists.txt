#
# cmake file
#

CMAKE_MINIMUM_REQUIRED(VERSION 2.8.5)

set (PROJ_NAME optimization)

PROJECT(${PROJ_NAME})

set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++1y")

set (SRC src/AsgdOptimizer.cpp)

set (INCLUDE include/AsgdOptimizer.h)

set (TCC tcc/AsgdOptimizer.tcc)

source_group("src" FILES ${SRC})
source_group("include" FILES ${INCLUDE})
source_group("tcc" FILES ${TCC})

include_directories(include)
include_directories(tcc)
include_directories(../linear/include)
include_directories(../layers/include)
include_directories(../predictors/include)
include_directories(../types/include)
include_directories(../utilities/include)

add_library(${PROJ_NAME} ${SRC} ${INCLUDE} ${TCC})

set_property(TARGET ${PROJ_NAME} PROPERTY FOLDER "libraries")

