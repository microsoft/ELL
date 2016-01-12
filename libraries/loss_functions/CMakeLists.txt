#
# cmake file
#

CMAKE_MINIMUM_REQUIRED(VERSION 2.8.5)

set (PROJ_NAME loss_functions)

PROJECT(${PROJ_NAME})

set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++1y")

set (SRC src/HingeLoss.cpp
         src/LogLoss.cpp
         src/SquaredLoss.cpp)

set (INCLUDE include/HingeLoss.h
             include/LogLoss.h
             include/SquaredLoss.h)


source_group("src" FILES ${SRC})
source_group("include" FILES ${INCLUDE})

include_directories(include)
include_directories(../types/include)

add_library(${PROJ_NAME} ${SRC} ${INCLUDE})

set_property(TARGET ${PROJ_NAME} PROPERTY FOLDER "libraries")
