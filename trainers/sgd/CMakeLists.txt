#
# cmake file for sgd project
#

CMAKE_MINIMUM_REQUIRED(VERSION 2.8.11)

set (PROJ_NAME sgd)

PROJECT(${PROJ_NAME})

if(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)
  add_compile_options(-std=c++1y)
endif()

set (SRC src/main.cpp)
set (INCLUDE include/CommandLineArguments.h)

source_group("src" FILES ${SRC})
source_group("include" FILES ${INCLUDE})

include_directories(include)
include_directories(../../libraries/common/include)
include_directories(../../libraries/dataset/include)
include_directories(../../libraries/layers/include)
include_directories(../../libraries/linear/include)
include_directories(../../libraries/loss_functions/include)
include_directories(../../libraries/predictors/include)
include_directories(../../libraries/optimization/include)
include_directories(../../libraries/types/include)
include_directories(../../libraries/utilities/include)


add_executable(${PROJ_NAME} ${SRC} ${INCLUDE})

set_property(TARGET ${PROJ_NAME} PROPERTY FOLDER "trainers")

target_link_libraries(${PROJ_NAME} common dataset layers linear loss_functions predictors optimization utilities)

add_test(NAME sgd_train
         WORKING_DIRECTORY ${CMAKE_SOURCE_DIR} 
         COMMAND ${PROJ_NAME} -idf data/a9a.txt)

