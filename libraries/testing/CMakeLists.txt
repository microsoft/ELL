#
# cmake file
#

CMAKE_MINIMUM_REQUIRED(VERSION 2.8.11)

set (PROJ_NAME testing)

PROJECT(${PROJ_NAME})

if(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)
  add_compile_options(-std=c++1y)
endif()

set (SRC src/testing.cpp)
set (INCLUDE include/testing.h)
set (TCC )

source_group("src" FILES ${SRC})
source_group("include" FILES ${INCLUDE})
source_group("tcc" FILES ${TCC})

add_library(${PROJ_NAME} ${SRC} ${INCLUDE} ${TCC})
target_include_directories(${PROJ_NAME} PUBLIC include)
target_link_libraries(${PROJ_NAME} linear)

set_property(TARGET ${PROJ_NAME} PROPERTY FOLDER "libraries")
