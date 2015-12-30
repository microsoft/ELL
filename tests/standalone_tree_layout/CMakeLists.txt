#
# cmake file for standalone_tree_layout project
#

CMAKE_MINIMUM_REQUIRED(VERSION 2.8.5)
project(standalone_tree_layout)

set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++1y")

set (SRC standalone_tree_layout.cpp)

include_directories(../../libraries/common/include)
include_directories(../../libraries/tree_layout/include)

add_executable(standalone_tree_layout ${SRC})
target_link_libraries(standalone_tree_layout tree_layout)
