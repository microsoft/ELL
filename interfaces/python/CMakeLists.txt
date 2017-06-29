#
# SWIG python interfaces
#
find_package(PythonInterp 3.5 QUIET)
find_package(PythonLibs 3.5 QUIET)
if(${PYTHONLIBS_FOUND})
  add_subdirectory(test)
else()
  message(WARNING "Couldn't find Python libraries 3.5 or later, _ELL_python target will not be build properly. This is not a required component, so it can be skipped.")

  set(PYTHON_LIBRARIES common)
  set(PYTHON_INCLUDE_PATH ${CMAKE_CURRENT_SOURCE_DIR})
endif() # PYTHONLIBS_FOUND

add_subdirectory(utilities)

include (CommonInterfaces)

include_directories(${PYTHON_INCLUDE_PATH})

generate_interface(python ${CMAKE_CURRENT_SOURCE_DIR} ${PYTHON_LIBRARIES} "" ${PYTHON_INCLUDE_PATH})
set(CMAKE_SWIG_FLAGS ${CMAKE_SWIG_FLAGS})
