#
# cmake file for Embedded Machine Learning Library sub projects
#

CMAKE_MINIMUM_REQUIRED(VERSION 2.8.11)
PROJECT(EMLL)

# Include modules in the CMake directory.
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/CMake")

# Turn on ability to create folders to organize projects
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

if(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES Clang)
  add_compile_options(-std=c++1y)
endif()

enable_testing()

# Include cmake projects for libraries and executables
add_subdirectory(libraries/common)
add_subdirectory(libraries/dataset)
add_subdirectory(libraries/features)
add_subdirectory(libraries/layers)
add_subdirectory(libraries/linear)
add_subdirectory(libraries/lossFunctions)
add_subdirectory(libraries/optimization)
add_subdirectory(libraries/predictors)
add_subdirectory(libraries/testing)
add_subdirectory(libraries/treeLayout)
add_subdirectory(libraries/utilities)
add_subdirectory(apply)
add_subdirectory(compile)
add_subdirectory(print)
add_subdirectory(trainers/sgd)
add_subdirectory(scripts)

# Add SWIG-generated interfaces
find_package(SWIG)
if(${SWIG_FOUND})
  add_subdirectory(interfaces)
else()
  message(WARNING "Couldn't find SWIG, skipping interface generation.")
endif()


# Add examples
add_subdirectory(examples)

# Add solution-level documentation
set (DOC INSTALL.md StyleGuide.md)
add_custom_target(documentation ALL DEPENDS ${DOC} SOURCES ${DOC})

add_dependencies(EMLL_common common dataset layers linear lossFunctions optimization predictors testing treeLayout utilities)
