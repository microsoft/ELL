#
# cmake file for linear_test project
#

CMAKE_MINIMUM_REQUIRED(VERSION 2.8.5)
project(linear_test)

if(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)
  add_compile_options(-std=c++1y)
endif()

set (SRC linear_test.cpp)

include_directories(../../libraries/dataset/include)
include_directories(../../libraries/dataset/tcc)
include_directories(../../libraries/linear/include)
include_directories(../../libraries/types/include)

add_executable(linear_test ${SRC})
target_link_libraries(linear_test dataset linear)
