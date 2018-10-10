#
# cmake file
#

set (library_name optimization)

set (src src/Empty.cpp)

set (include include/OptimizationExample.h
             include/Expression.h
             include/IndexedContainer.h
             include/L2Regularizer.h
             include/LogLoss.h
             include/MatrixExampleSet.h
             include/MatrixSolution.h
             include/MultivariateLossAdapter.h
             include/SquareLoss.h
             include/SDCAOptimizer.h
             include/SGDOptimizer.h
             include/VectorSolution.h
)

set (tcc tcc/OptimizationExample.tcc
         tcc/Expression.tcc
         tcc/IndexedContainer.tcc
         tcc/L2Regularizer.tcc
         tcc/LogLoss.tcc
         tcc/MatrixExampleSet.tcc
         tcc/MatrixSolution.tcc
         tcc/MultivariateLossAdapter.tcc
         tcc/SquareLoss.tcc
         tcc/SDCAOptimizer.tcc
         tcc/SGDOptimizer.tcc
         tcc/VectorSolution.tcc
)

source_group("src" FILES ${src})
source_group("include" FILES ${include})
source_group("tcc" FILES ${tcc})

add_library(${library_name} ${src} ${include} ${tcc} ${doc})
target_include_directories(${library_name} PUBLIC include)
target_link_libraries(${library_name} utilities math)

# MSVC emits warnings incorrectly when mixing inheritance, templates,
# and member function definitions outside of class definitions
if(MSVC)
    target_compile_options(${library_name} PRIVATE /wd4505)
endif()

set_property(TARGET ${library_name} PROPERTY FOLDER "libraries")

#
# test project
#

set (test_name ${library_name}_test)

set (test_src test/src/main.cpp)

source_group("src" FILES ${test_src})

add_executable(${test_name} ${test_src} ${include})
target_include_directories(${test_name} PRIVATE test/include)
target_link_libraries(${test_name} optimization testing)
copy_shared_libraries(${test_name})

# ${library_name}

set_property(TARGET ${test_name} PROPERTY FOLDER "tests")

add_test(NAME ${test_name} COMMAND ${test_name})
set_test_library_path(${test_name})
