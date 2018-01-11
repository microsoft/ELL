#
# Example model files
#

set(MODEL_FILES
  arg_max.model
  broadcast_times_two.model
  identity.model
  is_equal.model
  model_1.model
  model_2.model
  model_3.model
  multi_out.model
  refined_tree_0.model
  refined_tree_1.model
  refined_tree_2.model
  refined_tree_3.model
  times_two.model
  tree_0.model
  tree_1.model
  tree_2.model
  tree_3.model
)

add_custom_target(modelFiles ALL DEPENDS ${MODEL_FILES} SOURCES ${MODEL_FILES})
add_dependencies(modelFiles makeExamples)
set_property(TARGET modelFiles PROPERTY FOLDER "examples")

# copy example models
foreach(MODEL_FILE ${MODEL_FILES})
    add_custom_command(TARGET modelFiles
                      POST_BUILD
                      WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
                      COMMAND ${CMAKE_COMMAND} -E copy ${MODEL_FILE} ${CMAKE_CURRENT_BINARY_DIR})
endforeach()
