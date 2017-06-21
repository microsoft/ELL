#
# SWIG python interfaces
#
find_package(PythonInterp)
find_package(PythonLibs)
if(${PYTHONLIBS_FOUND})
  include (CommonInterfaces)

  message(STATUS "Using python found at: ${PYTHON_EXECUTABLE}")
  message(STATUS "Using python libraries found at: ${PYTHON_LIBRARY}")

  include_directories(${PYTHON_INCLUDE_PATH})

  generate_interface(python ${CMAKE_CURRENT_SOURCE_DIR} ${PYTHON_LIBRARIES} "" ${PYTHON_INCLUDE_PATH})
  set(CMAKE_SWIG_FLAGS ${CMAKE_SWIG_FLAGS})

  # tests
  #add_test(NAME python_interface_test
  #         WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR} 
  #         COMMAND python sgd.py ../../../examples/data/testData.txt)

  #add_test(NAME python_interface_test
  #        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
  #        COMMAND python ./../../../interfaces/python/test/test.py)

  add_dependencies(_ELL_python ELL_common)

else()
  message(WARNING "Couldn't find Python libraries, python interfaces skipped.")
endif() # PYTHONLIBS_FOUND

add_subdirectory(test)
