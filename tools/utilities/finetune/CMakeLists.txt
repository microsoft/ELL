#
# cmake file for finetune project
#

include(${CMAKE_ROOT}/Modules/ExternalProject.cmake)

set(common_src 
    src/DataUtils.cpp
    src/ModelUtils.cpp
    src/Report.cpp
    src/FineTuneModel.cpp
    src/FineTuneArguments.cpp
    src/OptimizationUtils.cpp
    src/TransformData.cpp
)

set(common_include
    include/DataUtils.h
    include/ModelUtils.h
    include/Report.h
    include/FineTuneModel.h
    include/FineTuneArguments.h
    include/OptimizationUtils.h
    include/TransformData.h
)

# finetune tool
set(tool_name finetune)

set(main_src
    src/main.cpp
)

set(src "${common_src}")
set(include "${common_include}")

set(docs README.md)

source_group("src" FILES ${src})
source_group("include" FILES ${include})

# create executable in build\bin
set (GLOBAL_BIN_DIR ${CMAKE_BINARY_DIR}/bin)
set (EXECUTABLE_OUTPUT_PATH ${GLOBAL_BIN_DIR}) 
add_executable(${tool_name} ${docs} ${src} ${main_src} ${include})
target_include_directories(${tool_name} PRIVATE include ${ELL_LIBRARIES_DIR})
target_link_libraries(${tool_name} common data dsp evaluators functions model nodes optimization predictors utilities)
copy_shared_libraries(${tool_name})

# put this project in the utilities folder in the IDE 
set_property(TARGET ${tool_name} PROPERTY FOLDER "tools/trainers")

#
# test project
#

set(tool_name finetune)
set(test_name ${tool_name}_test)

set(test_src
    test/src/main.cpp
    test/src/LoadTestData.cpp
    test/src/LoadTestModels.cpp
    test/src/TestDataUtils.cpp
    test/src/TestModelUtils.cpp
    test/src/TestOptimizationUtils.cpp
    test/src/TestTransformData.cpp
)

set(test_include
    test/include/LoadTestData.h
    test/include/LoadTestModels.h
    test/include/TestDataUtils.h
    test/include/TestModelUtils.h
    test/include/TestOptimizationUtils.h
    test/include/TestTransformData.h
)   

set(test_tcc
)

source_group("src" FILES ${test_src})
source_group("include" FILES ${test_include})

add_executable(${test_name} ${test_src} ${test_include} ${common_src} ${common_include})
target_include_directories(${test_name} PRIVATE include test/include ${ELL_LIBRARIES_DIR})
target_link_libraries(${test_name} common data dsp functions model nodes optimization predictors trainers testing utilities model_testing)
add_dependencies(${test_name} exampleModels)
copy_shared_libraries(${test_name})

set_property(TARGET ${test_name} PROPERTY FOLDER "tests")

add_test(NAME ${test_name} COMMAND ${test_name})
set_test_library_path(${test_name})
