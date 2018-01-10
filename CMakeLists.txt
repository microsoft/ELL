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

# Try to create a compilation database, which is useful to have when working
# with clang tooling
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Options
# To set an option:
# cmake -DMyOption=ON|OFF buildDirectory
# (so if we're running cmake from a 'build' directory inside the main directory, do this:
#      cmake -DBUILD_DOCS=OFF ..  )
option(BUILD_DOC "Build Doxygen documentation" ON)
option(USE_COMPILER_CACHE "Use a compiler cache (ccache) to speed up build times" $ENV{CMAKE_COMPILER_CACHE})
option(STRICT_MODE "Build with 'warnings as errors'" OFF)
option(PROFILING "Enable performance profiling tests" OFF)
option(DISABLE_PYTHON "Explicitly disable building python modules" OFF)

set(EXTERNAL_DIR "${CMAKE_SOURCE_DIR}/external" CACHE DOCUMENTATION "Directory to install external dependencies" )

if(DISABLE_PYTHON)
  message(STATUS "Python has been explicitly disabled")
else()
  find_package(PythonInterp 3.6 QUIET)

  message(STATUS "PYTHONINTERP_FOUND=${PYTHONINTERP_FOUND}")
  message(STATUS "PYTHON_EXECUTABLE=${PYTHON_EXECUTABLE}")
  message(STATUS "PYTHON_VERSION_STRING=${PYTHON_VERSION_STRING}")
  message(STATUS "PYTHON_VERSION_MAJOR=${PYTHON_VERSION_MAJOR}")

  find_package(PythonLibs 3.6 QUIET)

  message(STATUS "PYTHONLIBS_FOUND=${PYTHONLIBS_FOUND}")
  message(STATUS "PYTHON_LIBRARIES=${PYTHON_LIBRARIES}")
  message(STATUS "PYTHON_INCLUDE_PATH=${PYTHON_INCLUDE_PATH}")
  message(STATUS "PYTHON_INCLUDE_DIRS=${PYTHON_INCLUDE_DIRS}")
  message(STATUS "PYTHON_DEBUG_LIBRARIES=${PYTHON_DEBUG_LIBRARIES}")
  message(STATUS "PYTHONLIBS_VERSION_STRING=${PYTHONLIBS_VERSION_STRING}")

  if(${PYTHONLIBS_FOUND})
    set(PYTHON_ENABLED ON)
  else()
    set(PYTHON_ENABLED OFF)
    message(STATUS "### Python 3.6 or later not found, so python features will be disabled")
  endif()
endif()

# Include modules in the CMake directory.
list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/CMake")

# Turn on ability to create folders to organize projects
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# Set C++ version
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Apply -fPIC where applicable to the platform
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# Find the platform-specific way of working with threads
set(THREADS_PREFER_PTHREAD_FLAG ON)
set(CMAKE_THREAD_PREFER_PTHREAD ON)
find_package(Threads REQUIRED)

if(STRICT_MODE)
  message("-- Turning on strict mode with warnings as errors.")
endif()

if(MSVC)
  # Set Visual Studio-specific options
  add_definitions(-DUNICODE)
  add_compile_options(/MP)
  add_compile_options(/bigobj)
  add_compile_options(/W4)
  if(STRICT_MODE)
    add_compile_options(/WX)
  endif()
else()
  # Set Clang/GCC-specific options
  add_compile_options(-Wall)
  if(STRICT_MODE)
    add_compile_options(-Werror)
  endif()
  add_compile_options(-Wno-missing-braces)
  add_compile_options(-Wmissing-field-initializers)
  add_compile_options(-fvisibility-inlines-hidden)
  set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -gdwarf-2 -O0")
  set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -gdwarf-2  -O0")
endif()

# Turn on ctest tests
enable_testing()

# Set up global variables to help find NuGet projects
set(PACKAGE_ROOT ${EXTERNAL_DIR})
include(OpenBLASSetup)
include(LLVMSetup)
include(SWIGSetup)
include(CopySharedLibraries)
include(AddPrecompiledHeader)
include(CompilerCache)

if(USE_COMPILER_CACHE)
  use_compiler_cache()
endif()

# Print helpful message if LLVM not found on Windows
if(NOT LLVM_FOUND AND WIN32)
  message(WARNING "LLVM not found. Run the following command from the main project directory:\n    nuget.exe restore external/packages.config -PackagesDirectory external")
endif()

# Include cmake projects for libraries and executables
add_subdirectory(libraries)
add_subdirectory(tools)
include(CommonInterfaces)
add_subdirectory(interfaces)
# Add examples (has dependencies on SWIG-generated interfaces)
add_subdirectory(examples)

# Add user directories to ELL build if requested
if(EXISTS "${CMAKE_SOURCE_DIR}/user")
  # Add root user directory if it has a CMakeLists.txt file and INCLUDE_IN_ELL_BUILD.txt file
  if(EXISTS"${CMAKE_SOURCE_DIR}/user/CMakeLists.txt" AND EXISTS "${CMAKE_SOURCE_DIR}/user/INCLUDE_IN_ELL_BUILD.txt")
    message(STATUS "Adding user directory to ELL build")
    add_subdirectory(user)
  endif()

  # Now add all child directories that have CMakeLists.txt files and INCLUDE_IN_ELL_BUILD.txt file
  file(GLOB children RELATIVE "${CMAKE_SOURCE_DIR}/user" "${CMAKE_SOURCE_DIR}/user/*")
  foreach(child ${children})
    if(IS_DIRECTORY "${CMAKE_SOURCE_DIR}/user/${child}" AND EXISTS "${CMAKE_SOURCE_DIR}/user/${child}/CMakeLists.txt" AND EXISTS "${CMAKE_SOURCE_DIR}/user/${child}/INCLUDE_IN_ELL_BUILD.txt")
      message(STATUS "Adding user directory ${child} to ELL build")
      add_subdirectory("user/${child}")
    endif()
  endforeach()
endif()

# Add project for solution-level documentation
set (DOC README.md
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
