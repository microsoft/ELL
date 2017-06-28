#
# Example files
#

set (DATA_FILES testData.txt tinyTestData.txt)

set (MODEL_FILES 
    arg_max.model
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


#
# Example data files
#
add_custom_target(dataFiles DEPENDS ${DATA_FILES} SOURCES ${DATA_FILES})
add_dependencies(dataFiles makeExamples)
set_property(TARGET dataFiles PROPERTY FOLDER "examples")

# copy example data
foreach(DATA_FILE ${DATA_FILES})
    add_custom_command(TARGET dataFiles 
                    POST_BUILD
                    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} 
                    COMMAND ${CMAKE_COMMAND} -E copy ${DATA_FILE} ${CMAKE_CURRENT_BINARY_DIR})
endforeach()

#
# Example model files
#
set_source_files_properties(${MODEL_FILES} PROPERTIES GENERATED TRUE)
add_custom_target(modelFiles ALL DEPENDS ${DATA_FILES} SOURCES ${DATA_FILES})
set_property(TARGET modelFiles PROPERTY FOLDER "examples")

# copy example models
foreach(MODEL_FILE ${MODEL_FILES})
    add_custom_command(TARGET modelFiles 
                    POST_BUILD
                    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} 
                    COMMAND ${CMAKE_COMMAND} -E copy ${MODEL_FILE} ${CMAKE_CURRENT_BINARY_DIR})
endforeach()
