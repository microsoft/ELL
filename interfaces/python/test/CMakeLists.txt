#
# cmake file
#

if(${PYTHON_ENABLED})
  set(test_name ell-python-interface-test)

  file(GLOB test_src RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *.py)

  add_custom_target(${test_name} DEPENDS ${test_src} SOURCES ${test_src})
  add_dependencies(${test_name} _ELL_python)
  set_property(TARGET ${test_name} PROPERTY FOLDER "tests")

  # copy the contents of the test directory to build/interfaces/python

  foreach(PYTHON_FILE ${test_src})
    configure_file(${PYTHON_FILE} ${PYTHON_FILE} COPYONLY)
  endforeach()

  # add the python test to the cmake suite of tests
  add_test(NAME ${test_name}
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    COMMAND ${PYTHON_EXECUTABLE} test.py)
  set_property(TARGET ${test_name} PROPERTY FOLDER "tests")

endif()  # PYTHON_ENABLED