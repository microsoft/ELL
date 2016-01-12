#
# cmake file for apply project
#

CMAKE_MINIMUM_REQUIRED(VERSION 2.8.5)
project(apply)

if(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)
  add_compile_options(-std=c++1y)
endif()

set (SRC src/main.cpp)

include_directories(../libraries/layers/include)
include_directories(../libraries/types/include)
include_directories(../libraries/utilities/include)

add_executable(apply ${SRC})
target_link_libraries(apply utilities layers)
