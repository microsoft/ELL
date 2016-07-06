#
# cmake file for Embedded Machine Learning Library sub projects
#

CMAKE_MINIMUM_REQUIRED(VERSION 2.8.11)

add_subdirectory(common)
if (LLVM_ENABLED)
  add_subdirectory(compiler)
endif()
add_subdirectory(dataset)
add_subdirectory(evaluators)
add_subdirectory(features)
add_subdirectory(layers)
add_subdirectory(linear)
add_subdirectory(lossFunctions)
add_subdirectory(model)
add_subdirectory(nodes)
add_subdirectory(predictors)
add_subdirectory(testing)
add_subdirectory(trainers)
add_subdirectory(treeLayout)
add_subdirectory(utilities)
