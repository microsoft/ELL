#
# cmake file for test_exe project
#

CMAKE_MINIMUM_REQUIRED(VERSION 2.8.5)

set (PROJ_NAME test_exe)

project(${PROJ_NAME})

if(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)
  add_compile_options(-std=c++1y)
endif()

set (SRC test.cpp)

include_directories(../../libraries/utilities/include)

add_executable(${PROJ_NAME} ${SRC})

set_property(TARGET ${PROJ_NAME} PROPERTY FOLDER "tests")

target_link_libraries(${PROJ_NAME} utilities)

