#
# cmake file for standalone_tree_layout project
#

CMAKE_MINIMUM_REQUIRED(VERSION 2.8.5)

set (PROJ_NAME standalone_tree_layout)

project(${PROJ_NAME})

set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++1y")

set (SRC standalone_tree_layout.cpp)

include_directories(../../libraries/tree_layout/include)
include_directories(../../libraries/types/include)

add_executable(${PROJ_NAME} ${SRC})

set_property(TARGET ${PROJ_NAME} PROPERTY FOLDER "tests")

target_link_libraries(${PROJ_NAME} tree_layout)
