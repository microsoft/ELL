#
# cmake file for Bonsai projects
#

CMAKE_MINIMUM_REQUIRED(VERSION 2.8.5)
PROJECT(Bonsai)

# Turn on ability to create folders to organize projects
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++1y")

# Include cmake projects for libraries and executables
add_subdirectory(libraries/common)
add_subdirectory(libraries/dataset)
add_subdirectory(libraries/layers)
add_subdirectory(libraries/linear)
add_subdirectory(libraries/loss_functions)
add_subdirectory(libraries/optimization)
add_subdirectory(libraries/predictors)
add_subdirectory(libraries/tree_layout)
add_subdirectory(libraries/utilities)
#add_subdirectory(apply)
add_subdirectory(compile)
add_subdirectory(print)
add_subdirectory(tests/linear_test)
add_subdirectory(tests/standalone_tree_layout)
add_subdirectory(tests/test)
add_subdirectory(trainers/sgd)

