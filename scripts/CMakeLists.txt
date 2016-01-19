#
# cmake file
#

CMAKE_MINIMUM_REQUIRED(VERSION 2.8.11)

set (PROJ_NAME scripts)
PROJECT(${PROJ_NAME})

set (SRC test.py)

add_custom_target(${PROJ_NAME} ALL SOURCES ${SRC})

#copy contents of scripts directory to build/scripts
add_custom_command(TARGET ${PROJ_NAME} POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy_directory
                   ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR})