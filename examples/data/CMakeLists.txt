#
# Example data files
#

set (DATA_FILES testData.txt tinyTestData.txt protonnTestData.txt)

add_custom_target(dataFiles ALL DEPENDS ${DATA_FILES} SOURCES ${DATA_FILES})
add_dependencies(dataFiles makeExamples)
set_property(TARGET dataFiles PROPERTY FOLDER "examples")

# copy example data
foreach(DATA_FILE ${DATA_FILES})
    add_custom_command(TARGET dataFiles
                    POST_BUILD
                    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
                    COMMAND ${CMAKE_COMMAND} -E copy ${DATA_FILE} ${CMAKE_CURRENT_BINARY_DIR})
endforeach()
