#
# cmake file
#

set(library_name passes)

set(src
    src/FuseLinearOperationsTransformation.cpp
    src/OptimizeReorderDataNodesTransformation.cpp
    src/SetConvolutionMethodTransformation.cpp
    src/StandardTransformations.cpp
)

set(include
    include/FuseLinearOperationsTransformation.h
    include/OptimizeReorderDataNodesTransformation.h
    include/SetConvolutionMethodTransformation.h
    include/StandardTransformations.h
)

set(doc
)

source_group("src" FILES ${src})
source_group("include" FILES ${include})
source_group("doc" FILES ${doc})

add_library(${library_name} ${src} ${include} ${doc})
target_include_directories(${library_name} PRIVATE include ${ELL_LIBRARIES_DIR})
target_link_libraries(${library_name} model nodes predictors)

set_property(TARGET ${library_name} PROPERTY FOLDER "libraries")

#
# test project
#

set(test_name ${library_name}_test)

set(test_src
    test/src/main.cpp
    test/src/ModelOptimizerTest.cpp
    test/src/TransformationTest.cpp
)

set(test_include
    test/include/ModelOptimizerTest.h
    test/include/TransformationTest.h
)

source_group("src" FILES ${test_src})
source_group("include" FILES ${test_include})

add_executable(${test_name} ${test_src} ${test_include} ${include})
target_include_directories(${test_name} PRIVATE test/include ${ELL_LIBRARIES_DIR})
target_link_libraries(${test_name} model nodes passes predictors testing utilities)
copy_shared_libraries(${test_name})

set_property(TARGET ${test_name} PROPERTY FOLDER "tests")

add_test(NAME ${test_name} COMMAND ${test_name})
set_test_library_path(${test_name})
