#
# cmake file
#

CMAKE_MINIMUM_REQUIRED(VERSION 2.8.5)

set (PROJ_NAME linear)

PROJECT(${PROJ_NAME})

if(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)
  add_compile_options(-std=c++1y)
endif()

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
         tcc/RowMatrix.tcc)

source_group("src" FILES ${SRC})
source_group("include" FILES ${INCLUDE})
source_group("tcc" FILES ${TCC})

include_directories(include)
include_directories(tcc)
include_directories(../types/include)

add_library(${PROJ_NAME} ${SRC} ${INCLUDE} ${TCC})

set_property(TARGET ${PROJ_NAME} PROPERTY FOLDER "libraries")