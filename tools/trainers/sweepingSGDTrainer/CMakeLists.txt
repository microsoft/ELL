#
# cmake file for sgd project
#

CMAKE_MINIMUM_REQUIRED(VERSION 2.8.11)

# define project
set (PROJ_NAME sweepingSGDTrainer)
PROJECT(${PROJ_NAME})

#enable C++11 in GCC, etc
if(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES Clang)
  add_compile_options(-std=c++1y)
endif()

set (SRC src/main.cpp)

source_group("src" FILES ${SRC})

# create executable in build\bin
set (GLOBAL_BIN_DIR ${CMAKE_BINARY_DIR}/bin)
set (EXECUTABLE_OUTPUT_PATH ${GLOBAL_BIN_DIR}) 
add_executable(${PROJ_NAME} ${SRC})
target_include_directories(${PROJ_NAME} PRIVATE include)
target_link_libraries(${PROJ_NAME} common dataset layers linear lossFunctions predictors trainers evaluators utilities)

# put this project in the trainers folder in the IDE 
set_property(TARGET ${PROJ_NAME} PROPERTY FOLDER "tools/trainers")

# copy executable to build/bin directory
set (GLOBAL_BIN_DIR ${CMAKE_BINARY_DIR}/bin)

# tests
add_test(NAME sgdTrainerTest
         WORKING_DIRECTORY ${GLOBAL_BIN_DIR}
         COMMAND ${PROJ_NAME} -idf ${CMAKE_SOURCE_DIR}/examples/data/testData.txt -dd auto -omf null -v -lf log)
