#
# cmake file for Embedded Learning Library subprojects
#

set(ELL_LIBRARIES_DIR ${CMAKE_CURRENT_LIST_DIR})

add_subdirectory(common)
add_subdirectory(data)
add_subdirectory(dsp)
add_subdirectory(emittable_functions)
add_subdirectory(emitters)
add_subdirectory(evaluators)
add_subdirectory(functions)
add_subdirectory(math)
add_subdirectory(model)
add_subdirectory(nodes)
add_subdirectory(optimization)
add_subdirectory(passes)
add_subdirectory(predictors)
add_subdirectory(testing)
add_subdirectory(model_testing)
add_subdirectory(trainers)
add_subdirectory(utilities)
add_subdirectory(value)

add_custom_target(libraries)
add_dependencies(libraries
    common data dsp emitters emittable_functions evaluators functions math
    model nodes optimization passes predictors trainers utilities value)

add_custom_target(tests)
add_dependencies(tests
    common_test data_test dsp_test dsp_timing emittable_functions_test
    emitters_test evaluators_test functions_test math_test math_profile
    model_test model_compiler_test global_optimizer_test model_testing
    nodes_test dsp_nodes_test nn_nodes_test nodes_timing optimization_test
    passes_test predictors_test testing trainers_test utilities_test
    value_test)
