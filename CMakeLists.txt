#
# cmake file for Embedded Machine Learning Library sub projects
#

CMAKE_MINIMUM_REQUIRED(VERSION 2.8.11)

# Check for trying to 32-bit in Windows
if(${CMAKE_GENERATOR} STREQUAL "Visual Studio 14 2015")
message(FATAL_ERROR "Error: can't build this project in 32-bit mode. Try again using 64-bit mode: cmake -G \"Visual Studio 14 2015 Win64\"")
endif()

PROJECT(EMLL)

# Define custom configuration types for VS
if(CMAKE_CONFIGURATION_TYPES)
#  list(APPEND CMAKE_CONFIGURATION_TYPES Documentation)
#  list(REMOVE_DUPLICATES CMAKE_CONFIGURATION_TYPES)

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
#      cmake -DDOXYGEN_ENABLED=OFF ..  )
option(SWIG_ENABLED "Enable swig interfaces" ON)
option(DOXYGEN_ENABLED "Enable Doxygen interfaces" OFF)

# Include modules in the CMake directory.
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/CMake")

# Turn on ability to create folders to organize projects
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

if(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES Clang)
  add_compile_options(-std=c++1y)
elseif(MSVC)
  add_definitions(-DUNICODE)
  add_compile_options(/MP)
endif()

enable_testing()

# Include cmake projects for libraries and executables
add_subdirectory(libraries)
add_subdirectory(tools)
add_subdirectory(scripts)
add_subdirectory(private)

# Add SWIG-generated interfaces

if(SWIG_ENABLED)
  find_package(SWIG 3.0)
  if(SWIG_FOUND)
    add_subdirectory(interfaces)

    # Add examples
    add_subdirectory(examples)
  
  else()
    message(WARNING "Couldn't find SWIG, skipping interface generation.")
  endif()
else()
  message(WARNING "Note: swig interfaces disabled.")
endif()

# Add project for solution-level documentation
set (DOC INSTALL.md StyleGuide.md)
add_custom_target(documentation ALL DEPENDS ${DOC} SOURCES ${DOC})

# Generate doxygen documentation
if(DOXYGEN_ENABLED)
  message(STATUS "Doxygen documentation generation enabled")
  find_package(Doxygen)
  if(DOXYGEN_FOUND)
    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile @ONLY)

    set (DOXYFILE_LOCATION "${CMAKE_CURRENT_BINARY_DIR}/Doxyfile")
    add_custom_target(doxygen ALL
      COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYFILE_LOCATION}
      WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
      COMMENT "Building Doxygen documentation"
      SOURCES Doxyfile)
  else()
    message(WARNING "Doxygen processor not found")
  endif()
else()
  message(STATUS "Doxygen documentation generation disabled")
endif()
