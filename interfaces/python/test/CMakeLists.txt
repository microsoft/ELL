#
# cmake file
#

set (test_name ell-python-interface-test)

set (src test.py 
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
add_custom_target(${test_name} ALL DEPENDS ${src} SOURCES ${src})

# copy the contents of the test directory to build/interfaces/python

foreach(PYTHON_FILE ${src})
        configure_file(${PYTHON_FILE} ${PYTHON_FILE} COPYONLY)
endforeach()

# add the python test to the cmake suite of tests

add_test(NAME ${test_name}
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        COMMAND python test.py)
