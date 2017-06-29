#
# cmake file for Embedded Learning Library subprojects
#

cmake_minimum_required(VERSION 3.3 FATAL_ERROR)

project(ELL)

# Define custom configuration types for Visual Studio
if(CMAKE_CONFIGURATION_TYPES)
  # Copy important configuration info from Release config to new Documentation config  
  list(APPEND CMAKE_CONFIGURATION_TYPES Documentation)
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
option(BUILD_DOC "Build Doxygen documentation" ON)

set(EXTERNAL_DIR "${CMAKE_SOURCE_DIR}/external" CACHE DOCUMENTATION "Directory to install external dependencies" )

# Include modules in the CMake directory.
list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/CMake")

# Turn on ability to create folders to organize projects
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# Set C++ version
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Set global compilation flags
if(NOT WIN32 AND NOT CYGWIN)
  add_compile_options("-fvisibility-inlines-hidden")
endif()

# Set Visual Studio-specific options
if(MSVC)
  add_definitions(-DUNICODE)
  add_compile_options(/MP)
  add_compile_options(/bigobj)
endif()

# Set options so we don't get visibility warnings with LLVM
if(NOT WIN32 AND NOT CYGWIN)
  add_compile_options("-fvisibility-inlines-hidden")
endif()

# Turn on ctest tests
enable_testing()

# Set up global variables to help find NuGet projects
set(PACKAGE_ROOT ${EXTERNAL_DIR})
include(OpenBLASSetup)
include(LLVMSetup)
include(CopySharedLibraries)
include(AddPrecompiledHeader)

# Print helpful message if LLVM not found on Windows
if(NOT LLVM_FOUND AND WIN32)
  message(WARNING "LLVM not found. Run the following command from the main project directory:\n    nuget.exe restore external/packages.config -PackagesDirectory external")
endif()

# Include cmake projects for libraries and executables
add_subdirectory(libraries)
add_subdirectory(tools)

if(EXISTS ${CMAKE_SOURCE_DIR}/private AND EXISTS ${CMAKE_SOURCE_DIR}/private/CMakeLists.txt)
  add_subdirectory(private)
endif()

# Add SWIG-generated interfaces
find_package(SWIG 3.0.12)
if(NOT SWIG_FOUND)
  message(WARNING "SWIG not found, interfaces and projects that depend on them will not build properly. \
                  They are not part of the default make targets, so can be skipped. \
                  If you want to build interfaces (such as the Python bindings), follow the instructions in INSTALL-XXXXX.md file to install SWIG.\
                  Then delete the CMakeCache.txt file from your build directory run the cmake command again.")
endif()
add_subdirectory(interfaces)

# Add examples (has dependencies on SWIG-generated interfaces)
add_subdirectory(examples)

# Add tutorials (has dependencies on SWIG-generated interfaces)
add_subdirectory(tutorials)

# Add project for solution-level documentation
set (DOC INSTALL-Mac.md
         INSTALL-Ubuntu.md
         INSTALL-Windows.md
         README.md
         StyleGuide.md)
add_custom_target(documentation ALL DEPENDS ${DOC} SOURCES ${DOC})
set_property(TARGET documentation PROPERTY FOLDER "documentation")

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
    set_property(TARGET doc PROPERTY FOLDER "documentation")
  else()
    message(WARNING "Doxygen processor not found")
  endif()
endif()
