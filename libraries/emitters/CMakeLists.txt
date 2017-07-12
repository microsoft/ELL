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
    src/IRBlockRegion.cpp
    src/IRDiagnosticHandler.cpp
    src/IREmitter.cpp
    src/IRExecutionEngine.cpp
    src/IRFunctionEmitter.cpp
    src/IRHeaderWriter.cpp
    src/IRIfEmitter.cpp
    src/IRLoader.cpp
    src/IRLoopEmitter.cpp
    src/IRMetadata.cpp
    src/IRModuleEmitter.cpp
    src/IROptimizer.cpp
    src/IRRuntime.cpp
    src/IRSwigInterfaceWriter.cpp
    src/ModuleEmitter.cpp
    src/TargetDevice.cpp
    src/Variable.cpp
)

set (include 
    include/ClockInterface.h
    include/CompilableIRFunction.h
    include/EmitterException.h
    include/EmitterTypes.h
    include/IRAssemblyWriter.h
    include/IRBlockRegion.h
    include/IRDiagnosticHandler.h
    include/IREmitter.h
    include/IRExecutionEngine.h
    include/IRFunctionEmitter.h
    include/IRHeaderWriter.h
    include/IRIfEmitter.h
    include/IRLoader.h
    include/IRLoopEmitter.h
    include/IRModuleEmitter.h
    include/IRMetadata.h
    include/IROptimizer.h
    include/IRRuntime.h
    include/IRSwigInterfaceWriter.h
    include/LLVMInclude.h
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
    tcc/IRLoopEmitter.tcc
    tcc/IRModuleEmitter.tcc
    tcc/IRRuntime.tcc
    tcc/ScalarVariable.tcc
    tcc/SymbolTable.tcc
    tcc/VectorVariable.tcc
    tcc/Variable.tcc
)

source_group("src" FILES ${src})
source_group("include" FILES ${include})
source_group("tcc" FILES ${tcc})

add_library(${library_name} ${src} ${include} ${tcc})
target_include_directories(${library_name} PUBLIC include ${LLVM_INCLUDE_DIRS})
if(CMAKE_COMPILER_IS_GNUCXX)
  target_compile_options(${library_name} PRIVATE -fPIC)
endif()

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
  test/src/IREmitterTest.cpp
  test/src/IRFunctionTest.cpp
)

set (test_include 
  test/include/IREmitterTest.h
  test/include/IRFunctionTest.h
)

source_group("src" FILES ${test_src})
source_group("include" FILES ${test_include})

add_executable(${test_name} ${test_src} ${test_include} ${include})
target_include_directories(${test_name} PRIVATE test/include)
target_link_libraries(${test_name} nodes model testing utilities predictors emitters)
copy_shared_libraries(${test_name})

set_property(TARGET ${test_name} PROPERTY FOLDER "tests")
add_test(NAME ${test_name} COMMAND ${test_name})

