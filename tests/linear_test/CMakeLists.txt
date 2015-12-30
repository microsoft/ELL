#
# cmake file for linear_test project
#

CMAKE_MINIMUM_REQUIRED(VERSION 2.8.5)
project(linear_test)

set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++1y")

set (SRC linear_test.cpp)

include_directories(../../libraries/common/include)
include_directories(../../libraries/dataset/include)
include_directories(../../libraries/dataset/tcc)
include_directories(../../libraries/linear/include)

add_executable(linear_test ${SRC})
target_link_libraries(linear_test dataset linear)
