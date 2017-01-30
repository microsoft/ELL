#
# cmake file for makeExamples project
#

CMAKE_MINIMUM_REQUIRED(VERSION 2.8.11)

# define project
set (PROJ_NAME makeExamples)
PROJECT(${PROJ_NAME})

#enable C++11 in GCC, etc
if(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES Clang)
  add_compile_options(-std=c++1y)
endif()

set (SRC src/main.cpp
         src/GenerateModels.cpp)

set (INCLUDE include/GenerateModels.h)

source_group("src" FILES ${SRC})
source_group("include" FILES ${INCLUDE})

# create executable in build\bin
set (GLOBAL_BIN_DIR ${CMAKE_BINARY_DIR}/bin)
set (EXECUTABLE_OUTPUT_PATH ${GLOBAL_BIN_DIR}) 
add_executable(${PROJ_NAME} ${SRC} ${INCLUDE})
target_include_directories(${PROJ_NAME} PRIVATE include)
target_link_libraries(${PROJ_NAME} utilities model nodes common)
copy_shared_libraries(${PROJ_NAME} $<TARGET_FILE_DIR:${PROJ_NAME}>)

set_property(TARGET ${PROJ_NAME} PROPERTY FOLDER "tools/utilities")

#
# Add build command to generate the examples
#
set (EXAMPLE_DIR ${CMAKE_SOURCE_DIR}/examples/data)

set (MODEL_FILES 
  ${EXAMPLE_DIR}/identity.model
  ${EXAMPLE_DIR}/is_equal.model
  ${EXAMPLE_DIR}/model_1.model 
  ${EXAMPLE_DIR}/model_2.model
  ${EXAMPLE_DIR}/model_3.model
  ${EXAMPLE_DIR}/multi_out.model
  ${EXAMPLE_DIR}/refined_tree_0.model
  ${EXAMPLE_DIR}/refined_tree_1.model
  ${EXAMPLE_DIR}/refined_tree_2.model
  ${EXAMPLE_DIR}/refined_tree_3.model
  ${EXAMPLE_DIR}/times_two.model
  ${EXAMPLE_DIR}/tree_0.model
  ${EXAMPLE_DIR}/tree_1.model
  ${EXAMPLE_DIR}/tree_2.model
  ${EXAMPLE_DIR}/tree_3.model
)

add_custom_command(TARGET ${PROJ_NAME} 
  POST_BUILD
  COMMAND makeExamples -mf model
  WORKING_DIRECTORY ${EXAMPLE_DIR}
  BYPRODUCTS ${MODEL_FILES}
  COMMENT "Generating example models")
