#
# SWIG python interfaces
#
find_package(PythonLibs)
if(${PYTHONLIBS_FOUND})

include (CommonInterfaces)

# Fix link problems when building in Debug mode:
add_definitions(-DSWIG_PYTHON_INTERPRETER_NO_DEBUG)

find_package(PythonLibs REQUIRED)
include_directories(${PYTHON_INCLUDE_PATH})

generate_interface(python ${CMAKE_CURRENT_SOURCE_DIR} ${PYTHON_LIBRARIES} "")

# Note: need to use /usr/bin/python on my OS X install -- TODO: autodetect where python is
# tests
#add_test(NAME python_interface_test
#         WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR} 
#         COMMAND python sgd.py ../../../examples/data/testData.txt)

add_dependencies(_EMLL_python EMLL_common)

else()
  message(WARNING "Couldn't find Python libraries, python interfaces skipped.")
endif() # PYTHONLIBS_FOUND