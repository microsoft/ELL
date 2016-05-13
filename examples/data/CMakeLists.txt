#
# SWIG example projects
#

set (DATA_FILES testData.txt)
add_custom_target(datafiles ALL DEPENDS ${DATA_FILES} SOURCES ${DATA_FILES})


# copy python example code
foreach(DATA_FILE ${DATA_FILES})
    add_custom_command(TARGET datafiles POST_BUILD
                    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} 
                    COMMAND ${CMAKE_COMMAND}
                    -E copy ${DATA_FILE} ${CMAKE_CURRENT_BINARY_DIR})
endforeach()
