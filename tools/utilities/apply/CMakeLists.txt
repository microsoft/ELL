#
# cmake file for apply project
#

CMAKE_MINIMUM_REQUIRED(VERSION 2.8.11)

# define project
set (PROJ_NAME apply)
project(${PROJ_NAME})

#enable C++11 in GCC, etc.
if(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES Clang)
  add_compile_options(-std=c++1y)
endif()

set (SRC src/main.cpp)

include_directories(${CMAKE_SOURCE_DIR}/libraries/linear/include)
include_directories(${CMAKE_SOURCE_DIR}/libraries/layers/include)
include_directories(${CMAKE_SOURCE_DIR}/libraries/dataset/include)
include_directories(${CMAKE_SOURCE_DIR}/libraries/common/include)
include_directories(${CMAKE_SOURCE_DIR}/libraries/utilities/include)

source_group("src" FILES ${SRC})

# create executable in build\bin
set (GLOBAL_BIN_DIR ${CMAKE_BINARY_DIR}/bin)
set (EXECUTABLE_OUTPUT_PATH ${GLOBAL_BIN_DIR}) 
add_executable(${PROJ_NAME} ${SRC})

target_link_libraries(${PROJ_NAME} utilities layers linear dataset common)

set_property(TARGET ${PROJ_NAME} PROPERTY FOLDER "tools/utilities")

