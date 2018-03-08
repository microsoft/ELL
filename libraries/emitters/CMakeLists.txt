#
# cmake file
#

set (library_name emitters)

include (LLVMSetup)
if(NOT LLVM_FOUND)
  message("LLVM unavailable. Compilers disabled.")
  return()
endif()

set (src
    src/EmitterTypes.cpp
    src/IRAssemblyWriter.cpp
    src/IRAsyncTask.cpp
    src/IRBlockRegion.cpp
    src/IRDiagnosticHandler.cpp
    src/IREmitter.cpp
    src/IRExecutionEngine.cpp
    src/IRFunctionEmitter.cpp
    src/IRHeaderWriter.cpp
    src/IRIfEmitter.cpp
    src/IRLocalValue.cpp
    src/IRLoader.cpp
    src/IRLocalValue.cpp
    src/IRLocalValueOperations.cpp
    src/IRLoopEmitter.cpp
    src/IRMetadata.cpp
    src/IRModuleEmitter.cpp
    src/IROptimizer.cpp
    src/IRPosixRuntime.cpp
    src/IRRuntime.cpp
    src/IRSwigInterfaceWriter.cpp
    src/IRTask.cpp
    src/IRThreadPool.cpp
    src/IRThreadUtilities.cpp
    src/LLVMUtilities.cpp
    src/ModuleEmitter.cpp
    src/TargetDevice.cpp
    src/Variable.cpp
)

set (include
    include/CompilableIRFunction.h
    include/CompilerOptions.h
    include/EmitterException.h
    include/EmitterTypes.h
    include/IRAssemblyWriter.h
    include/IRAsyncTask.h
    include/IRBlockRegion.h
    include/IRDiagnosticHandler.h
    include/IREmitter.h
    include/IRExecutionEngine.h
    include/IRFunctionEmitter.h
    include/IRHeaderWriter.h
    include/IRIfEmitter.h
    include/IRLoader.h
    include/IRLocalValue.h
    include/IRLocalValueOperations.h
    include/IRLoopEmitter.h
    include/IRModuleEmitter.h
    include/IRMetadata.h
    include/IROptimizer.h
    include/IRPosixRuntime.h
    include/IRRuntime.h
    include/IRSwigInterfaceWriter.h
    include/IRTask.h
    include/IRThreadPool.h
    include/IRThreadUtilities.h
    include/LLVMInclude.h
    include/LLVMUtilities.h
    include/ModuleEmitter.h
    include/ScalarVariable.h
    include/SymbolTable.h
    include/TargetDevice.h
    include/Variable.h
    include/VectorVariable.h
)

set (tcc
    tcc/CompilableIRFunction.tcc
    tcc/EmitterTypes.tcc
    tcc/IRFunctionEmitter.tcc
    tcc/IRLocalValueOperations.tcc
    tcc/IRModuleEmitter.tcc
    tcc/IRRuntime.tcc
    tcc/ScalarVariable.tcc
    tcc/SymbolTable.tcc
    tcc/VectorVariable.tcc
    tcc/Variable.tcc
)

set (templates
    templates/CppPredictWrapper.in
    templates/SwigModule.in
    templates/SwigPredictFunction.in
    templates/SwigPredictorClass.in
    templates/SwigPredictorPython.in
    templates/SwigRawPredictPython.in
    templates/SwigShapeWrappers.in
)

source_group("src" FILES ${src})
source_group("include" FILES ${include})
source_group("tcc" FILES ${tcc})
source_group("templates" FILES ${templates})

add_library(${library_name} ${src} ${include} ${tcc} ${templates})
target_include_directories(${library_name} PUBLIC include templates)
target_include_directories(${library_name} SYSTEM PUBLIC ${LLVM_INCLUDE_DIRS})
target_link_libraries(${library_name} utilities ${LLVM_LIBS})
target_compile_definitions(${library_name} PUBLIC ${LLVM_DEFINITIONS})
target_compile_options(${library_name} PUBLIC ${LLVM_COMPILE_OPTIONS})

set_property(TARGET ${library_name} PROPERTY FOLDER "libraries")

#
# test project
#

set (test_name ${library_name}_test)

if(MSVC)
  set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /ignore:4099")
endif()

set (test_src
  test/src/main.cpp
  test/src/AsyncEmitterTest.cpp
  test/src/IREmitterTest.cpp
  test/src/IRFunctionTest.cpp
  test/src/PosixEmitterTest.cpp
)

set (test_include
  test/include/AsyncEmitterTest.h
  test/include/IREmitterTest.h
  test/include/IRFunctionTest.h
  test/include/PosixEmitterTest.h
)

source_group("src" FILES ${test_src})
source_group("include" FILES ${test_include})

add_executable(${test_name} ${test_src} ${test_include} ${include})
target_include_directories(${test_name} PRIVATE test/include)
target_link_libraries(${test_name} testing utilities emitters)
copy_shared_libraries(${test_name})

set_property(TARGET ${test_name} PROPERTY FOLDER "tests")
add_test(NAME ${test_name} COMMAND ${test_name})
set_test_library_path(${test_name})

