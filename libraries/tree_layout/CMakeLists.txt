#
# cmake file for tree_layout library
#

CMAKE_MINIMUM_REQUIRED(VERSION 2.8.5)
PROJECT(tree_layout)

set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++1y")

set (SRC src/Layout.cpp
         src/QpLayoutGenerator.cpp)

set (INCLUDE include/Layout.h
             include/QpLayoutGenerator.h)

set (TCC tcc/QpLayoutGenerator.tcc)

source_group("src" FILES ${SRC})
source_group("include" FILES ${INCLUDE})
source_group("tcc" FILES ${TCC})

include_directories(include)
include_directories(tcc)
include_directories(../common/include)

add_library(tree_layout ${SRC} ${INCLUDE} ${TCC})
