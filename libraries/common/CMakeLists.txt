#
# cmake file for common library
#

CMAKE_MINIMUM_REQUIRED(VERSION 2.8.5)
PROJECT(common)

set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++1y")


set (SRC src/DatasetMapLoader.cpp)

set (INCLUDE include/DatasetLoader.h
             include/DatasetMapLoader.h)

set (TCC tcc/DatasetLoader.tcc)

source_group("src" FILES ${SRC})
source_group("include" FILES ${INCLUDE})
source_group("tcc" FILES ${TCC})

include_directories(include)
include_directories(tcc)
include_directories(../dataset/include)
include_directories(../layers/include)
include_directories(../linear/include)
include_directories(../types/include)
include_directories(../utilities/include)

add_library(common ${SRC} ${INCLUDE} ${TCC})
