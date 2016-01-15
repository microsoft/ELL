#
# cmake file for apply project
#

CMAKE_MINIMUM_REQUIRED(VERSION 2.8.11)

set (PROJ_NAME apply)

project(${PROJ_NAME})

if(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)
  add_compile_options(-std=c++1y)
endif()

set (SRC src/main.cpp)

include_directories(../libraries/linear/include)
include_directories(../libraries/layers/include)
include_directories(../libraries/dataset/include)
include_directories(../libraries/common/include)
include_directories(../libraries/types/include)
include_directories(../libraries/utilities/include)

add_executable(${PROJ_NAME} ${SRC})

target_link_libraries(${PROJ_NAME} utilities layers linear dataset common)
