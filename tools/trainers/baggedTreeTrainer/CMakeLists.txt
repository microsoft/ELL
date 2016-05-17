#
# cmake file for sortingTreeLearner project
#

CMAKE_MINIMUM_REQUIRED(VERSION 2.8.11)

# define project
set (PROJ_NAME baggedTreeTrainer)
PROJECT(${PROJ_NAME})

#enable C++11 in GCC, etc
if(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES Clang)
  add_compile_options(-std=c++1y)
endif()

set (SRC src/main.cpp)

source_group("src" FILES ${SRC})

include_directories(include)
include_directories(../../../libraries/common/include)
include_directories(../../../libraries/dataset/include)
include_directories(../../../libraries/layers/include)
include_directories(../../../libraries/linear/include)
include_directories(../../../libraries/lossFunctions/include)
include_directories(../../../libraries/predictors/include)
include_directories(../../../libraries/trainers/include)
include_directories(../../../libraries/utilities/include)

# create executable in build\bin
set (GLOBAL_BIN_DIR ${CMAKE_BINARY_DIR}/bin)
set (EXECUTABLE_OUTPUT_PATH ${GLOBAL_BIN_DIR}) 
add_executable(${PROJ_NAME} ${SRC} ${INCLUDE})
target_link_libraries(${PROJ_NAME} common dataset layers linear lossFunctions predictors trainers utilities)

# put this project in the trainers folder in the IDE 
set_property(TARGET ${PROJ_NAME} PROPERTY FOLDER "tools/trainers")

# copy executable to build/bin directory
set (GLOBAL_BIN_DIR ${CMAKE_BINARY_DIR}/bin)
