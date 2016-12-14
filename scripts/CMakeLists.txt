#
# cmake file
#

CMAKE_MINIMUM_REQUIRED(VERSION 2.8.11)

set (PROJ_NAME scripts)
PROJECT(${PROJ_NAME})

set (SRC recipeUtil.py)

add_custom_target(${PROJ_NAME} ALL DEPENDS ${SRC} SOURCES ${SRC})

# copy contents of scripts directory to build/scripts
foreach(SCRIPT_FILE ${SRC})
    configure_file(${SCRIPT_FILE} ${SCRIPT_FILE} COPYONLY)
endforeach()                   