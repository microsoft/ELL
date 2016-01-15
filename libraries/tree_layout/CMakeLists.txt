#
# cmake file
#

CMAKE_MINIMUM_REQUIRED(VERSION 2.8.11)

set (PROJ_NAME tree_layout)

PROJECT(${PROJ_NAME})

if(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES Clang)
  add_compile_options(-std=c++1y)
endif()

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
include_directories(../types/include)

add_library(${PROJ_NAME} ${SRC} ${INCLUDE} ${TCC})

set_property(TARGET ${PROJ_NAME} PROPERTY FOLDER "libraries")

