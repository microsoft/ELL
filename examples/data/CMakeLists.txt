#
# example data
#

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

set_source_files_properties(${MODEL_FILES} PROPERTIES GENERATED TRUE)

set (DATA_FILES ${MODEL_FILES} testData.txt tinyTestData.txt)
add_custom_target(datafiles DEPENDS ${DATA_FILES} SOURCES ${DATA_FILES})
add_dependencies(datafiles makeExamples)

# copy example data
foreach(DATA_FILE ${DATA_FILES})
    add_custom_command(TARGET datafiles 
                    POST_BUILD
                    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} 
                    COMMAND ${CMAKE_COMMAND} -E copy ${DATA_FILE} ${CMAKE_CURRENT_BINARY_DIR})
endforeach()
