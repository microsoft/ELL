#
# cmake file for apply project
#

CMAKE_MINIMUM_REQUIRED(VERSION 2.8.5)

set (PROJ_NAME apply)

project(${PROJ_NAME})

set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++1y")

set (SRC src/main.cpp)

include_directories(../libraries/layers/include)
include_directories(../libraries/types/include)
include_directories(../libraries/utilities/include)

add_executable(${PROJ_NAME} ${SRC})

target_link_libraries(${PROJ_NAME} utilities layers)
