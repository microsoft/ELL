#
# cmake file
#

set(library_name optimization)

set(src
    src/Interval.cpp
    src/NormProx.cpp
)

set(include
    include/AbsoluteLoss.h
    include/BinarySearch.h
    include/Common.h
    include/ElasticNetRegularizer.h
    include/ExponentialSearch.h
    include/Expression.h
    include/GetSparseSolution.h
    include/GoldenSectionSearch.h
    include/HingeLoss.h
    include/HuberLoss.h
    include/IndexedContainer.h
    include/Interval.h
    include/L2Regularizer.h
    include/LogisticLoss.h
    include/MaskedMatrixSolution.h
    include/MatrixDataset.h
    include/MatrixSolution.h
    include/MaxRegularizer.h
    include/MultivariateLoss.h
    include/NormProx.h
    include/OptimizationExample.h
    include/SmoothedHingeLoss.h
    include/SquaredHingeLoss.h
    include/SquareLoss.h
    include/SDCAOptimizer.h
    include/SGDOptimizer.h
    include/VectorSolution.h
)

source_group("src" FILES ${src})
source_group("include" FILES ${include})

add_library(${library_name} ${src} ${include})
target_include_directories(${library_name} PRIVATE include ${ELL_LIBRARIES_DIR})
target_link_libraries(${library_name} math)

# MSVC emits warnings incorrectly when mixing inheritance, templates,
# and member function definitions outside of class definitions
if(MSVC)
    target_compile_options(${library_name} PRIVATE /wd4505)
endif()

set_property(TARGET ${library_name} PROPERTY FOLDER "libraries")

#
# test project
#

set(test_name ${library_name}_test)

set(test_src
    test/src/main.cpp
    test/src/RandomDataset.cpp
    test/src/Regularizer_test.cpp
    test/src/Search_test.cpp
    test/src/Other_test.cpp
)

set(test_include
    test/include/LossFunction_test.h
    test/include/Optimizer_test.h
    test/include/Other_test.h
    test/include/RandomDataset.h
    test/include/Regularizer_test.h
    test/include/Search_test.h
    test/include/Solution_test.h
)

source_group("src" FILES ${test_src})
source_group("include" FILES ${test_include})

add_executable(${test_name} ${test_src} ${test_include})
target_include_directories(${test_name} PRIVATE test/include ${ELL_LIBRARIES_DIR})
target_link_libraries(${test_name} optimization testing)
copy_shared_libraries(${test_name})

set_property(TARGET ${test_name} PROPERTY FOLDER "tests")

add_test(NAME ${test_name} COMMAND ${test_name})
set_test_library_path(${test_name})
