#
# cmake file
#

CMAKE_MINIMUM_REQUIRED(VERSION 2.8.11)

set (PROJ_NAME scripts)
PROJECT(${PROJ_NAME})

set (SRC recipeUtil.py train_sgd.py)

add_custom_target(${PROJ_NAME} ALL DEPENDS ${SRC} SOURCES ${SRC})

# tests
add_test(NAME sgd_train
         WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR} 
         COMMAND python train_sgd.py ../../data/a9a.txt a9a_sgd.stack a9a_sgd.html a9a_sgd_code.txt)

# copy contents of scripts directory to build/scripts
add_custom_command(TARGET ${PROJ_NAME} POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy_directory
                   ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR})