#
# cmake file for test_exe project
#

CMAKE_MINIMUM_REQUIRED(VERSION 2.8.5)
project(test_exe)

set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++1y")

set (SRC test.cpp)

include_directories(../../libraries/utilities/include)

add_executable(test_exe ${SRC})

target_link_libraries(test_exe utilities)

