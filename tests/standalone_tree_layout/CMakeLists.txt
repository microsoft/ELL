#
# cmake file for standalone_tree_layout project
#

CMAKE_MINIMUM_REQUIRED(VERSION 2.8.5)
project(standalone_tree_layout)

if(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)
  add_compile_options(-std=c++1y)
endif()

set (SRC standalone_tree_layout.cpp)

include_directories(../../libraries/tree_layout/include)
include_directories(../../libraries/types/include)

add_executable(standalone_tree_layout ${SRC})
target_link_libraries(standalone_tree_layout tree_layout)
