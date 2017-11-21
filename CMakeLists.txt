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

set(EXTERNAL_DIR "${CMAKE_SOURCE_DIR}/external" CACHE DOCUMENTATION "Directory to install external dependencies" )

# Include modules in the CMake directory.
list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/CMake")

# Turn on ability to create folders to organize projects
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# Set C++ version
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Apply -fPIC where applicable to the platform
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

if(MSVC)
  # Set Visual Studio-specific options
  add_definitions(-DUNICODE)
  add_compile_options(/MP)
  add_compile_options(/bigobj)
  add_compile_options(/W4)
else()
  # Set Clang/GCC-specific options
  add_compile_options(-Wall)
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

include(CommonInterfaces)
add_subdirectory(interfaces)

# Add examples (has dependencies on SWIG-generated interfaces)
add_subdirectory(examples)

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
