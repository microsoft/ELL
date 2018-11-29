#
# cmake file
#

set(library_name value)

set(src
    src/ComputeContext.cpp
    src/Emittable.cpp
    src/EmitterContext.cpp
    src/LLVMContext.cpp
    src/ValueMatrix.cpp
    src/ValueMatrixOperations.cpp
    src/ValueScalar.cpp
    src/ValueScalarOperations.cpp
    src/ValueTensor.cpp
    src/ValueTensorOperations.cpp
    src/Value.cpp
    src/ValueOperations.cpp
    src/ValueVector.cpp
    src/ValueVectorOperations.cpp
)

set(include
    include/ComputeContext.h
    include/Emittable.h
    include/EmitterContext.h
    include/LLVMContext.h
    include/ValueMatrix.h
    include/ValueMatrixOperations.h
    include/ValueScalar.h
    include/ValueScalarOperations.h
    include/ValueTensor.h
    include/ValueTensorOperations.h
    include/Value.h
    include/ValueOperations.h
    include/ValueType.h
    include/ValueVector.h
    include/ValueVectorOperations.h
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
    test/src/Value_test.cpp
)

set(test_include
    test/include/Value_test.h
)

source_group("src" FILES ${test_src})
source_group("include" FILES ${test_include})

add_executable(${test_name} ${test_src} ${test_include})
target_include_directories(${test_name} PRIVATE test/include ${ELL_LIBRARIES_DIR})
target_link_libraries(${test_name} testing ${library_name} math)
copy_shared_libraries(${test_name})

set_property(TARGET ${test_name} PROPERTY FOLDER "tests")
add_test(NAME ${test_name} COMMAND ${test_name})
set_test_library_path(${test_name})
