#
# python example projects
#
set (PYTHON_EXAMPLES sgd.py run.bat run.sh)
add_custom_target(python_examples ALL DEPENDS ${PYTHON_EXAMPLES} SOURCES ${PYTHON_EXAMPLES})

# copy python example code
foreach(PY_FILE ${PYTHON_EXAMPLES})
    add_custom_command(TARGET python_examples POST_BUILD
                    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} 
                    COMMAND ${CMAKE_COMMAND}
                    -E copy ${PY_FILE} ${CMAKE_CURRENT_BINARY_DIR})
endforeach()
