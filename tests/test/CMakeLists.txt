#
# cmake file for test_exe project
#

CMAKE_MINIMUM_REQUIRED(VERSION 2.8.5)
project(test_exe)

if(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)
  add_compile_options(-std=c++1y)
endif()

set (SRC test.cpp)

include_directories(../../libraries/utilities/include)

add_executable(test_exe ${SRC})

target_link_libraries(test_exe utilities)

