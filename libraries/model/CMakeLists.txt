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
    src/IRCompiledMap.cpp
    src/IRMapCompiler.cpp
    src/MapCompiler.cpp
    src/Model.cpp
    src/ModelTransformer.cpp
    src/Node.cpp
    src/OutputNode.cpp
    src/OutputPort.cpp
    src/PortElements.cpp
    src/Port.cpp
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
    include/MapCompiler.h
    include/Model.h
    include/ModelTransformer.h
    include/Node.h
    include/NodeMap.h
    include/OutputNode.h
    include/OutputPort.h
    include/PortElements.h
    include/Port.h
)

set (tcc 
    tcc/DynamicMap.tcc
    tcc/InputNode.tcc
    tcc/InputPort.tcc
    tcc/IRCompiledMap.tcc
    tcc/Model.tcc
    tcc/ModelTransformer.tcc
    tcc/NodeMap.tcc
    tcc/OutputNode.tcc
    tcc/OutputPort.tcc
    tcc/PortElements.tcc
)

source_group("src" FILES ${src})
source_group("include" FILES ${include})
source_group("tcc" FILES ${tcc})

add_library(${library_name} ${src} ${include} ${tcc})
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
    test/src/Model_test.cpp
    test/src/PortElements_test.cpp
)

set (test_include 
    test/include/PortElements_test.h
    test/include/DynamicMap_test.h
    test/include/Model_test.h
)

source_group("src" FILES ${test_src})
source_group("include" FILES ${test_include})

add_executable(${test_name} ${test_src} ${test_include} ${include})
target_include_directories(${test_name} PRIVATE test/include)
target_link_libraries(${test_name} common model nodes testing utilities)
copy_shared_libraries(${test_name} $<TARGET_FILE_DIR:${test_name}>)

set_property(TARGET ${test_name} PROPERTY FOLDER "tests")

add_test(NAME ${test_name} COMMAND ${test_name})

#
# compiler-specific tests
#
set (compiler_test_name ${library_name}_compiler_test)

set (compiler_test_src
    test/src/compiler_test_main.cpp
    test/src/CompilableNodesTest.cpp
    test/src/CompilerTest.cpp
    test/src/ModelBuilder.cpp
)

set (compiler_test_include
    test/include/CompilableNodesTest.h
    test/include/CompilerTest.h
    test/include/CompilerTestUtilities.h
    test/include/ModelBuilder.h
)

set (compiler_test_tcc
    test/tcc/ModelBuilder.tcc
)

source_group("src" FILES ${compiler_test_src})
source_group("include" FILES ${compiler_test_include})
source_group("tcc" FILES ${compiler_test_tcc})

add_executable(${compiler_test_name} ${compiler_test_src} ${compiler_test_include})
target_include_directories(${compiler_test_name} PRIVATE test/include)
target_link_libraries(${compiler_test_name} common model nodes testing utilities)

set_property(TARGET ${compiler_test_name} PROPERTY FOLDER "tests")

add_test(NAME ${compiler_test_name} COMMAND ${compiler_test_name})

