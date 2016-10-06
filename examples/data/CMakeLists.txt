#
# SWIG example projects
#

set (MODEL_FILES model_1.json 
                 model_2.json
                 model_3.json
                 tree_0.json
                 tree_1.json
                 tree_2.json
                 tree_3.json
                 refined_tree_0.json
                 refined_tree_1.json
                 refined_tree_2.json
                 refined_tree_3.json
                 identity.json
                 timesTwo.json)
set (DATA_FILES ${MODEL_FILES} testData.txt tinyTestData.txt)
add_custom_target(datafiles ALL DEPENDS ${DATA_FILES} SOURCES ${DATA_FILES})

# copy example data
foreach(DATA_FILE ${DATA_FILES})
    add_custom_command(TARGET datafiles 
                    POST_BUILD
                    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} 
                    COMMAND ${CMAKE_COMMAND} -E copy ${DATA_FILE} ${CMAKE_CURRENT_BINARY_DIR})
endforeach()
