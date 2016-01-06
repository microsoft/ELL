#
# cmake file for linear library
#

CMAKE_MINIMUM_REQUIRED(VERSION 2.8.5)
PROJECT(linear)

set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++1y")

set (SRC src/Base.cpp
         src/DoubleMatrix.cpp
         src/DoubleSubvectorCref.cpp
         src/DoubleVector.cpp
         src/IMatrix.cpp
         src/IVector.cpp)

set (INCLUDE include/Base.h
             include/BiMatrix.h
             include/DoubleMatrix.h
             include/DoubleSubvectorCref.h
             include/DoubleVector.h
             include/IMatrix.h
             include/IVector.h
             include/RowMatrix.h
             include/operators.h)

set (TCC tcc/BiMatrix.tcc
         tcc/DoubleVector.tcc
         tcc/RowMatrix.tcc)

source_group("src" FILES ${SRC})
source_group("include" FILES ${INCLUDE})
source_group("tcc" FILES ${TCC})

include_directories(include)
include_directories(tcc)
include_directories(../types/include)

add_library(linear ${SRC} ${INCLUDE} ${TCC})
