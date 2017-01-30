#
# cmake file for Embedded Learning Library sub projects
#

CMAKE_MINIMUM_REQUIRED(VERSION 2.8.11)

PROJECT(ELL)

# Define custom configuration types for VS
if(CMAKE_CONFIGURATION_TYPES)
#  list(APPEND CMAKE_CONFIGURATION_TYPES Documentation)
  # Copy important configuration info from Release config to new Documentation config  
  set(CMAKE_CXX_FLAGS_DOCUMENTATION ${CMAKE_CXX_FLAGS_RELEASE})
  set(CMAKE_C_FLAGS_DOCUMENTATION ${CMAKE_C_FLAGS_RELEASE})
  set(CMAKE_EXE_LINKER_FLAGS_DOCUMENTATION ${CMAKE_EXE_LINKER_FLAGS_RELEASE})
  set(CMAKE_EXE_LINKER_FLAGS_DOCUMENTATION ${CMAKE_EXE_LINKER_FLAGS_RELEASE})
  set(CMAKE_SHARED_LINKER_FLAGS_DOCUMENTATION ${CMAKE_SHARED_LINKER_FLAGS_RELEASE})
  set(CMAKE_MODULE_LINKER_FLAGS_DOCUMENTATION ${CMAKE_MODULE_LINKER_FLAGS_RELEASE})
endif()

# Options
# To set an option:
# cmake -DMyOption=ON|OFF buildDirectory
# (so if we're running cmake from a 'build' directory inside the main directory, do this:
#      cmake -DBUILD_DOCS=OFF ..  )
option(BUILD_INTERFACES "Build SWIG interfaces" OFF)
option(BUILD_DOC "Build Doxygen documentation" ON)

set(EXTERNAL_DIR "${CMAKE_SOURCE_DIR}/external" CACHE DOCUMENTATION "Directory to install external dependencies" )

# Include modules in the CMake directory.
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/CMake")

# Turn on ability to create folders to organize projects
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

if(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)
  add_compile_options(-std=c++14 -Wall) 
elseif(CMAKE_CXX_COMPILER_ID MATCHES Clang)
  add_compile_options(-std=c++14 -Wall)
elseif(MSVC)
  add_definitions(-DUNICODE)
  add_compile_options(/MP)
endif()

# Set up global variables to help find NuGet projects
set(PACKAGE_ROOT ${EXTERNAL_DIR})
include(OpenBLASSetup)
include(LLVMSetup)
include(CopyDLLs)

enable_testing()

# Include cmake projects for libraries and executables
add_subdirectory(libraries)
add_subdirectory(tools)
add_subdirectory(scripts)

if(EXISTS ${CMAKE_SOURCE_DIR}/private)
  add_subdirectory(private)
endif()

# Add examples
add_subdirectory(examples)

# Add SWIG-generated interfaces
if(BUILD_INTERFACES)
  find_package(SWIG 3.0.9)
  if(SWIG_FOUND)
    add_subdirectory(interfaces)  
  else()
    message(WARNING "SWIG not found, skipping interface generation.")
  endif()
endif()

# Add project for solution-level documentation
set (DOC INSTALL.md StyleGuide.md)
add_custom_target(documentation ALL DEPENDS ${DOC} SOURCES ${DOC})

# Generate doxygen documentation
if(BUILD_DOC)
  find_package(Doxygen 1.8 QUIET)

  # search external NuGet package directory also
  if(NOT DOXYGEN_FOUND)
    set(DOXYGEN_PACKAGE_NAME Doxygen)
    set(DOXYGEN_PACKAGE_VERSION 1.8.13)
    set(DOXYGEN_PACKAGE_DIR ${PACKAGE_ROOT}/${DOXYGEN_PACKAGE_NAME}.${DOXYGEN_PACKAGE_VERSION})
    find_program(DOXYGEN_EXECUTABLE doxygen
      HINTS "${DOXYGEN_PACKAGE_DIR}/tools")
      if(DOXYGEN_EXECUTABLE)
        set(DOXYGEN_FOUND TRUE)
      endif()
  endif()

  if(DOXYGEN_FOUND)
    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile @ONLY)
    set (DOXYFILE_LOCATION "${CMAKE_CURRENT_BINARY_DIR}/Doxyfile")
    add_custom_target(doc
      COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYFILE_LOCATION}
      WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
      COMMENT "Building Doxygen documentation" VERBATIM
      SOURCES Doxyfile)
  else()
    message(WARNING "Doxygen processor not found")
  endif()
endif()
