#
# cmake file for test_exe project
#

CMAKE_MINIMUM_REQUIRED(VERSION 2.8.5)

set (PROJ_NAME test_exe)

project(${PROJ_NAME})

set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++1y")

set (SRC test.cpp)

include_directories(../../libraries/utilities/include)

add_executable(${PROJ_NAME} ${SRC})

set_property(TARGET ${PROJ_NAME} PROPERTY FOLDER "tests")

target_link_libraries(${PROJ_NAME} utilities)

