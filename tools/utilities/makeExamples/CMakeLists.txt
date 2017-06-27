#
# cmake file for makeExamples project
#

# define project
set (tool_name makeExamples)

set (src src/main.cpp
         src/ModelGenerateArguments.cpp
         src/GenerateModels.cpp)

set (include include/GenerateModels.h
             include/ModelGenerateArguments.h)

source_group("src" FILES ${src})
source_group("include" FILES ${include})

# create executable in build\bin
set (GLOBAL_BIN_DIR ${CMAKE_BINARY_DIR}/bin)
set (EXECUTABLE_OUTPUT_PATH ${GLOBAL_BIN_DIR}) 
add_executable(${tool_name} ${src} ${include})
target_include_directories(${tool_name} PRIVATE include)
target_link_libraries(${tool_name} utilities model nodes common)
copy_shared_libraries(${tool_name})

set_property(TARGET ${tool_name} PROPERTY FOLDER "tools/utilities")

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

add_custom_command(TARGET ${tool_name} 
  POST_BUILD
  COMMAND makeExamples
  WORKING_DIRECTORY ${EXAMPLE_DIR}
  BYPRODUCTS ${MODEL_FILES}
  COMMENT "Generating example models")
