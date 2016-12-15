#
# cmake file
#

CMAKE_MINIMUM_REQUIRED(VERSION 2.8.11)

set (PROJ_NAME ell-python-interface-tests)
project(${PROJ_NAME})
set (SRC test.py 
         testing.py 
         lossfunctions_test.py 
         model_test.py 
         common_test.py
         trainers_test.py
         predictors_test.py
         nodes_test.py
         linear_test.py
         evaluators_test.py
         dataset_test.py
         sgd_trainer_test.py
         treelayout_test.py
         )
add_custom_target(${PROJ_NAME} ALL DEPENDS ${SRC} SOURCES ${SRC})

# copy the contents of the test directory to build/interfaces/python

foreach(PYTHON_FILE ${SRC})
        configure_file(${PYTHON_FILE} ${PYTHON_FILE} COPYONLY)
endforeach()

# add the python test to the cmake suite of tests

add_test(NAME python_interface_test
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        COMMAND python test.py)
