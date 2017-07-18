#
# cmake file
#

set (library_name model)

set (src 
    src/CompilableNode.cpp
    src/CompilableNodeUtilities.cpp
    src/CompiledMap.cpp
    src/DynamicMap.cpp
    src/InputNode.cpp
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
)

set (include
    include/CompilableNodeUtilities.h
    include/CompiledMap.h
    include/DynamicMap.h
    include/CompilableNode.h
    include/InputNode.h
    include/InputPort.h
    include/IRCompiledMap.h
    include/IRMapCompiler.h
    include/IRSteppableMapCompiler.h
    include/MapCompiler.h
    include/Model.h
    include/ModelBuilder.h
    include/IRModelProfiler.h
    include/ModelTransformer.h
    include/Node.h
    include/NodeMap.h
    include/OutputNode.h
    include/OutputPort.h
    include/Port.h
    include/SteppableMap.h
    include/PortElements.h
)

set (tcc 
    tcc/DynamicMap.tcc
    tcc/InputNode.tcc
    tcc/InputPort.tcc
    tcc/IRCompiledMap.tcc
    tcc/IRMapCompiler.tcc
    tcc/IRSteppableMapCompiler.tcc
    tcc/MapCompiler.tcc
    tcc/Model.tcc
    tcc/ModelBuilder.tcc
    tcc/ModelTransformer.tcc
    tcc/NodeMap.tcc
    tcc/OutputNode.tcc
    tcc/OutputPort.tcc
    tcc/Port.tcc
    tcc/PortElements.tcc
    tcc/SteppableMap.tcc
)

set (doc doc/CompileAsFunction.md
         doc/README.md)

source_group("src" FILES ${src})
source_group("include" FILES ${include})
source_group("tcc" FILES ${tcc})
source_group("doc" FILES ${doc})

add_library(${library_name} ${src} ${include} ${tcc} ${doc})
target_include_directories(${library_name} PUBLIC include)
if(CMAKE_COMPILER_IS_GNUCXX)
  target_compile_options(${library_name} PRIVATE -fPIC)
endif()

target_link_libraries(${library_name} utilities data emitters)

set_property(TARGET ${library_name} PROPERTY FOLDER "libraries")

#
# test project
#

set (test_name ${library_name}_test)

set (test_src
    test/src/main.cpp
    test/src/DynamicMap_test.cpp
    test/src/ModelTestUtilities.cpp
    test/src/ModelBuilder_test.cpp
    test/src/Model_test.cpp
    test/src/ModelTestUtilities.cpp
    test/src/PortElements_test.cpp
)

set (test_include 
    test/include/DynamicMap_test.h
    test/include/ModelBuilder_test.h
    test/include/ModelTestUtilities.h
    test/include/Model_test.h
    test/include/PortElements_test.h
    test/include/ModelTestUtilities.h
)

set (test_tcc
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

#
# compiler-specific tests
#
set (compiler_test_name ${library_name}_compiler_test)

set (compiler_test_src
    test/src/model_compiler_test_main.cpp
    test/src/CompilableNodesTest.cpp
    test/src/CompilerTest.cpp
    test/src/ModelMaker.cpp
    test/src/ModelTestUtilities.cpp
    test/src/PerformanceCountersTest.cpp
)

set (compiler_test_include
    test/include/CompilableNodesTest.h
    test/include/CompilerTest.h
    test/include/ModelMaker.h
    test/include/ModelTestUtilities.h
    test/include/PerformanceCountersTest.h
)

set (compiler_test_tcc
    test/tcc/ModelMaker.tcc
    test/tcc/ModelTestUtilities.tcc
)

source_group("src" FILES ${compiler_test_src})
source_group("include" FILES ${compiler_test_include})
source_group("tcc" FILES ${compiler_test_tcc})

add_executable(${compiler_test_name} ${compiler_test_src} ${compiler_test_include})
target_include_directories(${compiler_test_name} PRIVATE test/include)
target_link_libraries(${compiler_test_name} common model nodes testing utilities)
copy_shared_libraries(${test_name})

set_property(TARGET ${compiler_test_name} PROPERTY FOLDER "tests")

add_test(NAME ${compiler_test_name} COMMAND ${compiler_test_name})

