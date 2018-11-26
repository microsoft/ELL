#
# cmake file for Embedded Learning Library subprojects
#

set(ELL_LIBRARIES_DIR ${CMAKE_CURRENT_LIST_DIR})

add_subdirectory(common)
add_subdirectory(data)
add_subdirectory(dsp)
add_subdirectory(emitters)
add_subdirectory(evaluators)
add_subdirectory(functions)
add_subdirectory(math)
add_subdirectory(model)
add_subdirectory(nodes)
add_subdirectory(passes)
add_subdirectory(predictors)
add_subdirectory(testing)
add_subdirectory(model_testing)
add_subdirectory(trainers)
add_subdirectory(utilities)
add_subdirectory(value)
