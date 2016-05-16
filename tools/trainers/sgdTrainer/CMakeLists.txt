#
# cmake file for sgd project
#

CMAKE_MINIMUM_REQUIRED(VERSION 2.8.11)

# define project
set (PROJ_NAME sgdTrainer)
PROJECT(${PROJ_NAME})

#enable C++11 in GCC, etc
if(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES Clang)
  add_compile_options(-std=c++1y)
endif()

set (SRC src/main.cpp)

set (INCLUDE include/SGDArguments.h)

source_group("src" FILES ${SRC})
source_group("include" FILES ${INCLUDE})

include_directories(include)
include_directories(${CMAKE_SOURCE_DIR}/libraries/common/include)
include_directories(${CMAKE_SOURCE_DIR}/libraries/dataset/include)
include_directories(${CMAKE_SOURCE_DIR}/libraries/layers/include)
include_directories(${CMAKE_SOURCE_DIR}/libraries/linear/include)
include_directories(${CMAKE_SOURCE_DIR}/libraries/lossFunctions/include)
include_directories(${CMAKE_SOURCE_DIR}/libraries/predictors/include)
include_directories(${CMAKE_SOURCE_DIR}/libraries/trainers/include)
include_directories(${CMAKE_SOURCE_DIR}/libraries/utilities/include)

# create executable in build\bin
set (GLOBAL_BIN_DIR ${CMAKE_BINARY_DIR}/bin)
set (EXECUTABLE_OUTPUT_PATH ${GLOBAL_BIN_DIR}) 
add_executable(${PROJ_NAME} ${SRC} ${INCLUDE})
target_link_libraries(${PROJ_NAME} common dataset layers linear lossFunctions predictors trainers utilities)

# put this project in the trainers folder in the IDE 
set_property(TARGET ${PROJ_NAME} PROPERTY FOLDER "tools/trainers")

# copy executable to build/bin directory
set (GLOBAL_BIN_DIR ${CMAKE_BINARY_DIR}/bin)
