#
# cmake file
#

set(library_name model)

set(src 
    src/CompilableNode.cpp
    src/CompilableNodeUtilities.cpp
    src/CompiledMap.cpp
    src/Map.cpp
    src/InputNodeBase.cpp
    src/InputPort.cpp
    src/IRCompiledMap.cpp
    src/IRMapCompiler.cpp
    src/MapCompiler.cpp
    src/Model.cpp
    src/ModelBuilder.cpp
    src/IRModelProfiler.cpp
    src/ModelTransformer.cpp
    src/Node.cpp
    src/OutputNode.cpp
    src/OutputPort.cpp
    src/Port.cpp
    src/PortElements.cpp
    src/PortMemoryLayout.cpp
)

set(include
    include/CompilableNodeUtilities.h
    include/CompilableNode.h
    include/CompiledMap.h
    include/InputNodeBase.h
    include/InputNode.h
    include/InputPort.h
    include/IRCompiledMap.h
    include/IRMapCompiler.h
    include/IRModelProfiler.h
    include/Map.h
    include/MapCompiler.h
    include/MapCompilerOptions.h
    include/Model.h
    include/ModelBuilder.h
    include/ModelTransformer.h
    include/Node.h
    include/NodeMap.h
    include/OutputNodeBase.h
    include/OutputNode.h
    include/OutputPort.h
    include/Port.h
    include/PortElements.h
    include/PortMemoryLayout.h
)

set(tcc 
    tcc/InputNode.tcc
    tcc/InputPort.tcc
    tcc/IRCompiledMap.tcc
    tcc/IRMapCompiler.tcc
    tcc/Map.tcc
    tcc/MapCompiler.tcc
    tcc/Model.tcc
    tcc/ModelBuilder.tcc
    tcc/ModelTransformer.tcc
    tcc/NodeMap.tcc
    tcc/OutputNode.tcc
    tcc/OutputPort.tcc
    tcc/Port.tcc
    tcc/PortElements.tcc
)

set(doc 
    doc/CompileAsFunction.md
    doc/README.md
)

set(optimizer_src
    optimizer/src/ModelOptimizer.cpp
    optimizer/src/OptimizationPass.cpp
    optimizer/src/OptimizationPassRegistry.cpp
)

set(optimizer_include
    optimizer/include/ModelOptimizer.h
    optimizer/include/ModelOptimizerOptions.h
    optimizer/include/OptimizationPass.h
    optimizer/include/OptimizationPassRegistry.h
)

set(optimizer_tcc
)

set(optimizer_doc
)

source_group("src" FILES ${src})
source_group("include" FILES ${include})
source_group("tcc" FILES ${tcc})
source_group("doc" FILES ${doc})
source_group("optimizer\\include" FILES ${optimizer_include})
source_group("optimizer\\src" FILES ${optimizer_src})
source_group("optimizer\\tcc" FILES ${optimizer_tcc})
source_group("optimizer\\doc" FILES ${optimizer_doc})

add_library(${library_name} ${src} ${include} ${tcc} ${doc} ${optimizer_src} ${optimizer_include} ${optimizer_tcc} ${optimizer_doc})
target_include_directories(${library_name} PUBLIC include optimizer/include)
target_link_libraries(${library_name} utilities data emitters)

set_property(TARGET ${library_name} PROPERTY FOLDER "libraries")

#
# test project
#

set(test_name ${library_name}_test)

set(test_src
    test/src/main.cpp
    test/src/Map_test.cpp
    test/src/ModelTestUtilities.cpp
    test/src/ModelBuilder_test.cpp
    test/src/Model_test.cpp
    test/src/ModelTestUtilities.cpp
    test/src/PortElements_test.cpp
)

set(test_include 
    test/include/Map_test.h
    test/include/ModelBuilder_test.h
    test/include/ModelTestUtilities.h
    test/include/Model_test.h
    test/include/PortElements_test.h
    test/include/ModelTestUtilities.h
)

set(test_tcc
    test/tcc/ModelTestUtilities.tcc
)

source_group("src" FILES ${test_src})
source_group("include" FILES ${test_include})

add_executable(${test_name} ${test_src} ${test_include} ${include})
target_include_directories(${test_name} PRIVATE test/include)
target_link_libraries(${test_name} common model nodes testing utilities)
copy_shared_libraries(${test_name})

set_property(TARGET ${test_name} PROPERTY FOLDER "tests")

add_test(NAME ${test_name} COMMAND ${test_name})
set_test_library_path(${test_name})

#
# compiler-specific tests
#
set(compiler_test_name ${library_name}_compiler_test)

set(compiler_test_src
    test/src/model_compiler_test_main.cpp
    test/src/CompilableNodesTest.cpp
    test/src/CompilerTest.cpp
    test/src/ModelHeaderOutputTest.cpp
    test/src/ModelMaker.cpp
    test/src/ModelTestUtilities.cpp
    test/src/PerformanceCountersTest.cpp
)

set(compiler_test_include
    test/include/CompilableNodesTest.h
    test/include/CompilerTest.h
    test/include/ModelHeaderOutputTest.h
    test/include/ModelMaker.h
    test/include/ModelTestUtilities.h
    test/include/PerformanceCountersTest.h
)

set(compiler_test_tcc
    test/tcc/CompilableNodesTest.tcc
    test/tcc/CompilerTest.tcc
    test/tcc/ModelMaker.tcc
    test/tcc/ModelTestUtilities.tcc
)

source_group("src" FILES ${compiler_test_src})
source_group("include" FILES ${compiler_test_include})
source_group("tcc" FILES ${compiler_test_tcc})

add_executable(${compiler_test_name} ${compiler_test_src} ${compiler_test_include} ${compiler_test_tcc})
target_include_directories(${compiler_test_name} PRIVATE test/include)
target_link_libraries(${compiler_test_name} common model nodes testing utilities)
copy_shared_libraries(${compiler_test_name})

set_property(TARGET ${compiler_test_name} PROPERTY FOLDER "tests")

add_test(NAME ${compiler_test_name} COMMAND ${compiler_test_name})
set_test_library_path(${compiler_test_name})
