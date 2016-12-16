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

source_group("src" FILES ${SRC})

# create executable in build\bin
set (GLOBAL_BIN_DIR ${CMAKE_BINARY_DIR}/bin)
set (EXECUTABLE_OUTPUT_PATH ${GLOBAL_BIN_DIR}) 
add_executable(${PROJ_NAME} ${SRC})
target_include_directories(${PROJ_NAME} PRIVATE include)
target_link_libraries(${PROJ_NAME} common data lossFunctions model predictors trainers evaluators utilities)

# put this project in the trainers folder in the IDE 
set_property(TARGET ${PROJ_NAME} PROPERTY FOLDER "tools/trainers")

# copy executable to build/bin directory
set (GLOBAL_BIN_DIR ${CMAKE_BINARY_DIR}/bin)

# tests
set (TEST_NAME ${PROJ_NAME}_test_1)
add_test(NAME ${TEST_NAME}
         WORKING_DIRECTORY ${GLOBAL_BIN_DIR}
         COMMAND ${PROJ_NAME} --inputDataFilename ${CMAKE_SOURCE_DIR}/examples/data/testData.txt --inputModelFile ${CMAKE_SOURCE_DIR}/examples/data/model_3.model --modelInputs 1024 --modelOutputs 1031.output -dd 3 --outputModelFilename sgdTrainer_model_3.model -v -ne 20)

set (TEST_NAME ${PROJ_NAME}_test_2)
add_test(NAME ${TEST_NAME}
         WORKING_DIRECTORY ${GLOBAL_BIN_DIR}
         COMMAND ${PROJ_NAME} --inputDataFilename ${CMAKE_SOURCE_DIR}/examples/data/testData.txt -dd auto --outputModelFilename sgdTrainer_model_0.model -v -ne 20)
