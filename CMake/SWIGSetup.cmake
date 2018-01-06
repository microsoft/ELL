#
# SWIGSetup
#

# Centralized place to define SWIG variables 
# Sets the following variables:
#
# General information:
# SWIG_FOUND
#
# Settings for running SWIG:
# SWIG_DIR
# SWIG_EXECUTABLE
# SWIG_VERSION
#
# Using FindSWIG module:
# find_package(SWIG)
# if(SWIG_FOUND)
#     message(STATUS "SWIG executable: ${SWIG_EXECUTABLE}")
#     message(STATUS "SWIG version: ${SWIG_VERSION}")
#     message(STATUS "SWIG directory: ${SWIG_DIR}")
#

if(WIN32)
  set(SWIG_VERSION "3.0.12")
  if(EXISTS "${PACKAGE_ROOT}/swigwintools.${SWIG_VERSION}/tools/swigwin-${SWIG_VERSION}/swig.exe")
    set(SWIG_DIR "${PACKAGE_ROOT}/swigwintools.${SWIG_VERSION}/tools/swigwin-${SWIG_VERSION}")
    set(SWIG_EXECUTABLE "${SWIG_DIR}/swig.exe")
  endif()
endif()

find_package(SWIG 3.0.12)

if(NOT SWIG_FOUND)
    message(WARNING "SWIG not found, interfaces and projects that depend on them will not build properly. \
                    They are not part of the default make targets, so can be skipped. \
                    Then delete the CMakeCache.txt file from your build directory run the cmake command again.")
endif()
