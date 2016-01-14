#
# cmake file for linear_test project
#

CMAKE_MINIMUM_REQUIRED(VERSION 2.8.11)

set (PROJ_NAME linear_test)

project(${PROJ_NAME})

if(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)
  add_compile_options(-std=c++1y)
endif()

set (SRC linear_test.cpp)

include_directories(../../libraries/dataset/include)
include_directories(../../libraries/dataset/tcc)
include_directories(../../libraries/linear/include)
include_directories(../../libraries/types/include)

add_executable(${PROJ_NAME} ${SRC})

set_property(TARGET ${PROJ_NAME} PROPERTY FOLDER "tests")

target_link_libraries(${PROJ_NAME} dataset linear)
