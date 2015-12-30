#
# cmake file for layers library
#

CMAKE_MINIMUM_REQUIRED(VERSION 2.8.5)
PROJECT(layers)

set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++1y")


set (SRC src/Coordinate.cpp
         src/Coordinatewise.cpp
         src/DecisionTreePath.cpp
         src/Io.cpp
         src/Layer.cpp
         src/Map.cpp
         src/Scale.cpp
         src/Shift.cpp
         src/Sum.cpp)

set (INCLUDE include/Coordinate.h
             include/Coordinatewise.h
             include/DecisionTreePath.h
             include/Input.cpp
             include/Input.h
             include/Layer.h
             include/Map.h
             include/Scale.h
             include/Shift.h
             include/Sum.h
             include/layers.h)

set (TCC tcc/DecisionTreePath.tcc
         tcc/Map.tcc)

source_group("src" FILES ${SRC})
source_group("include" FILES ${INCLUDE})
source_group("tcc" FILES ${TCC})

include_directories(include)
include_directories(tcc)
include_directories(../common/include)
include_directories(../utilities/include)

add_library(layers ${SRC} ${INCLUDE} ${TCC})
