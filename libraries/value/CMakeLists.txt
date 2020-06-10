#
# cmake file
#

set(library_name value)

set(src
    src/Array.cpp
    src/CachingProvider.cpp
    src/CachingStrategies.cpp
    src/ComputeContext.cpp
    src/CppEmitterContext.cpp
    src/Emittable.cpp
    src/EmitterContext.cpp
    src/FunctionDeclaration.cpp
    src/LLVMContext.cpp
    src/LoopNests.cpp
    src/Matrix.cpp
    src/MatrixOperations.cpp
    src/Print.cpp
    src/Reference.cpp
    src/Scalar.cpp
    src/ScalarOperations.cpp
    src/Tensor.cpp
    src/TensorOperations.cpp
    src/Value.cpp
    src/ValueOperations.cpp
    src/Vector.cpp
    src/VectorOperations.cpp

    src/loopnests/CodeGenerator.cpp
    src/loopnests/CodePositionConstraints.cpp
    src/loopnests/ForAll.cpp
    src/loopnests/Index.cpp
    src/loopnests/IndexRange.cpp
    src/loopnests/IterationDomain.cpp
    src/loopnests/Kernel.cpp
    src/loopnests/KernelPredicate.cpp
    src/loopnests/LoopNest.cpp
    src/loopnests/LoopNestPrinter.cpp
    src/loopnests/LoopNestVisitor.cpp
    src/loopnests/Range.cpp
    src/loopnests/SplitIndexRange.cpp
    src/loopnests/SplitIterationDomain.cpp
)

set(include
    include/Array.h
    include/CachingProvider.h
    include/CachingStrategies.h
    include/ComputeContext.h
    include/CppEmitterContext.h
    include/Emittable.h
    include/EmitterContext.h
    include/FunctionDeclaration.h
    include/LLVMContext.h
    include/LoopNests.h
    include/Matrix.h
    include/MatrixOperations.h
    include/Print.h
    include/Reference.h
    include/Scalar.h
    include/Tensor.h
    include/TensorOperations.h
    include/Value.h
    include/ValueOperations.h
    include/ValueType.h
    include/Vector.h
    include/VectorOperations.h

    include/loopnests/CodeGenerator.h
    include/loopnests/CodePositionConstraints.h
    include/loopnests/ForAll.h
    include/loopnests/Index.h
    include/loopnests/IndexRange.h
    include/loopnests/IterationDomain.h
    include/loopnests/Kernel.h
    include/loopnests/KernelPredicate.h
    include/loopnests/LoopIndexInfo.h
    include/loopnests/LoopNest.h
    include/loopnests/LoopNestPrinter.h
    include/loopnests/LoopNestVisitor.h
    include/loopnests/Range.h
    include/loopnests/SplitIndexRange.h
    include/loopnests/SplitIterationDomain.h
)

set(doc
    doc/README.md)

source_group("src" FILES ${src})
source_group("include" FILES ${include})
source_group("doc" FILES ${doc})

add_library(${library_name} ${src} ${include} ${doc})
target_include_directories(${library_name} PRIVATE include ${ELL_LIBRARIES_DIR})
target_include_directories(${library_name} SYSTEM PUBLIC ${LLVM_INCLUDE_DIRS})
target_link_libraries(${library_name} PUBLIC ${LLVM_LIBS} emitters utilities)
target_compile_options(${library_name} PUBLIC ${LLVM_COMPILE_OPTIONS})
set_property(TARGET ${library_name} PROPERTY FOLDER "libraries")

#
# test project
#

set(test_name ${library_name}_test)

if(MSVC)
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /ignore:4099")
endif()

set(test_src
    test/src/main.cpp
    test/src/CachingStrategy_test.cpp
    test/src/Functions_test.cpp
    test/src/LoopNest_convolution_test.cpp
    test/src/LoopNest_kernels.cpp
    test/src/LoopNest_test.cpp
    test/src/LoopNestAPI_test.cpp
    test/src/Matrix_test.cpp
    test/src/Scalar_test.cpp
    test/src/Tensor_test.cpp
    test/src/TestUtil.cpp
    test/src/Value_test.cpp
    test/src/Vector_test.cpp
)

set(test_include
    test/include/CachingStrategy_test.h
    test/include/Functions_test.h
    test/include/LoopNest_convolution_test.h
    test/include/LoopNest_kernels.h
    test/include/LoopNest_test.h
    test/include/LoopNestAPI_test.h
    test/include/Matrix_test.h
    test/include/Scalar_test.h
    test/include/Tensor_test.h
    test/include/TestUtil.h
    test/include/Value_test.h
    test/include/Vector_test.h
)

source_group("src" FILES ${test_src})
source_group("include" FILES ${test_include})

add_executable(${test_name} ${test_src} ${test_include})
target_include_directories(${test_name} PRIVATE test/include ${ELL_LIBRARIES_DIR})
target_link_libraries(${test_name} testing ${library_name} math emitters)
copy_shared_libraries(${test_name})

set_property(TARGET ${test_name} PROPERTY FOLDER "tests")
add_test(NAME ${test_name} COMMAND ${test_name})
set_test_library_path(${test_name})
