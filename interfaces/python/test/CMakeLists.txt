#
# cmake file
#

CMAKE_MINIMUM_REQUIRED(VERSION 2.8.11)

set (PROJ_NAME emll-python-interface-tests)
project(${PROJ_NAME})
set (SRC test.py testing.py lossfunctions_test.py model_test.py)
add_custom_target(${PROJ_NAME} ALL DEPENDS ${SRC} SOURCES ${SRC})

# copy the contents of the test directory to build/interfaces/python

foreach(PYTHON_FILE ${SRC})
add_custom_command(TARGET ${PROJ_NAME} POST_BUILD
                   WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
                   COMMAND ${CMAKE_COMMAND}
                   -E copy ${PYTHON_FILE} ${CMAKE_CURRENT_BINARY_DIR})
endforeach()

# add the python test to the cmake suite of tests

add_test(NAME python_interface_test
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        COMMAND python test.py)
