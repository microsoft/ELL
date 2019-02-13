#
# cmake file
#

set(library_name model)

set(src
    src/CompilableCodeNode.cpp
    src/CompilableNode.cpp
    src/CompilableNodeUtilities.cpp
    src/CompiledMap.cpp
    src/InputNodeBase.cpp
    src/InputPort.cpp
    src/IRCompiledMap.cpp
    src/IRMapCompiler.cpp
    src/IRModelProfiler.cpp
    src/Map.cpp
    src/MapCompiler.cpp
    src/Model.cpp
    src/ModelBuilder.cpp
    src/ModelEditor.cpp
    src/ModelTransformer.cpp
    src/Node.cpp
    src/OptimizeModelTransformation.cpp
    src/OutputNodeBase.cpp
    src/OutputPort.cpp
    src/Port.cpp
    src/PortElements.cpp
    src/PortMemoryLayout.cpp
    src/RefineAndOptimizeTransformation.cpp
    src/RefineTransformation.cpp
    src/Submodel.cpp
    src/Transformation.cpp
    src/TransformContext.cpp
)

set(include
    include/CompilableCodeNode.h
    include/CompilableNode.h
    include/CompilableNodeUtilities.h
    include/CompiledMap.h
    include/InputNode.h
    include/InputNodeBase.h
    include/InputPort.h
    include/IRCompiledMap.h
    include/IRMapCompiler.h
    include/IRModelProfiler.h
    include/Map.h
    include/MapCompiler.h
    include/MapCompilerOptions.h
    include/Model.h
    include/ModelBuilder.h
    include/ModelEditor.h
    include/ModelTransformer.h
    include/Node.h
    include/NodeMap.h
    include/OptimizeModelTransformation.h
    include/OutputNode.h
    include/OutputNodeBase.h
    include/OutputPort.h
    include/Port.h
    include/PortElements.h
    include/PortMemoryLayout.h
    include/RefineAndOptimizeTransformation.h
    include/RefineTransformation.h
    include/SliceNode.h
    include/SpliceNode.h
    include/Submodel.h
    include/Transformation.h
    include/TransformContext.h
)

set(doc
    doc/CompileAsFunction.md
    doc/Metadata.md
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

set(optimizer_doc
)

source_group("src" FILES ${src})
source_group("include" FILES ${include})
source_group("doc" FILES ${doc})
source_group("optimizer\\include" FILES ${optimizer_include})
source_group("optimizer\\src" FILES ${optimizer_src})
source_group("optimizer\\doc" FILES ${optimizer_doc})

add_library(${library_name} ${src} ${include} ${doc} ${optimizer_src} ${optimizer_include} ${optimizer_doc})
target_include_directories(${library_name} PRIVATE include optimizer/include ${ELL_LIBRARIES_DIR})
target_link_libraries(${library_name} data emitters utilities value)

set_property(TARGET ${library_name} PROPERTY FOLDER "libraries")

#
# test project
#

set(test_name ${library_name}_test)

set(test_src
    test/src/main.cpp
    test/src/Map_test.cpp
    test/src/ModelBuilder_test.cpp
    test/src/ModelTransformerTest.cpp
    test/src/Model_test.cpp
    test/src/ModelTransformerTest.cpp
    test/src/PortElements_test.cpp
)

set(test_include
    test/include/Map_test.h
    test/include/ModelBuilder_test.h
    test/include/ModelTransformerTest.h
    test/include/Model_test.h
    test/include/ModelTransformerTest.h
    test/include/PortElements_test.h
)

source_group("src" FILES ${test_src})
source_group("include" FILES ${test_include})

add_executable(${test_name} ${test_src} ${test_include} ${include})
target_include_directories(${test_name} PRIVATE test/include ${ELL_LIBRARIES_DIR})
target_link_libraries(${test_name} common model nodes testing utilities model_testing)
copy_shared_libraries(${test_name})

set_property(TARGET ${test_name} PROPERTY FOLDER "tests")

add_test(NAME ${test_name} COMMAND ${test_name})
set_test_library_path(${test_name})

#
# compiler-specific tests
#
set(compiler_test_name ${library_name}_compiler_test)

set(compiler_test_src
    test/src/CompilableCodeNode_test.cpp
    test/src/CompilableNodesTest.cpp
    test/src/CompilerTest.cpp
    test/src/model_compiler_test_main.cpp
    test/src/ModelHeaderOutputTest.cpp
    test/src/ModelMaker.cpp
    test/src/PerformanceCountersTest.cpp
)

set(compiler_test_include
    test/include/CompilableCodeNode_test.h
    test/include/CompilableNodesTest.h
    test/include/CompilerTest.h
    test/include/ModelHeaderOutputTest.h
    test/include/ModelMaker.h
    test/include/PerformanceCountersTest.h
)

source_group("src" FILES ${compiler_test_src})
source_group("include" FILES ${compiler_test_include})

add_executable(${compiler_test_name} ${compiler_test_src} ${compiler_test_include})
target_include_directories(${compiler_test_name} PRIVATE test/include ${ELL_LIBRARIES_DIR})
target_link_libraries(${compiler_test_name} common model nodes passes testing model_testing utilities)
copy_shared_libraries(${compiler_test_name})

set_target_properties(
    ${compiler_test_name}
    PROPERTIES
        FOLDER "tests"
        ENABLE_EXPORTS ON
)

add_test(NAME ${compiler_test_name} COMMAND ${compiler_test_name})
set_test_library_path(${compiler_test_name})

#
# global optimizer-specific tests
#
set(global_optimizer_test_name global_optimizer_test)

set(optimizer_test_src
    optimizer/test/src/Cost.cpp
    optimizer/test/src/CostDatabase.cpp
    optimizer/test/src/CostModelTest.cpp
    optimizer/test/src/CostTest.cpp
    optimizer/test/src/Environment.cpp
    optimizer/test/src/EnvironmentTest.cpp
    optimizer/test/src/ExampleCostModels.cpp
    optimizer/test/src/ExampleObjectives.cpp
    optimizer/test/src/ExampleOptimizers.cpp
    optimizer/test/src/ExampleTransformations.cpp
    optimizer/test/src/FuseLinearOperationsTransformation.cpp
    optimizer/test/src/GlobalOptimizer.cpp
    optimizer/test/src/GlobalOptimizerOptions.cpp
    optimizer/test/src/main.cpp
    optimizer/test/src/ObjectiveTest.cpp
    optimizer/test/src/OptimizerOptionsTest.cpp
    optimizer/test/src/OptimizerTest.cpp
    optimizer/test/src/OptimizerTestUtil.cpp
    optimizer/test/src/SequentialOptimizer.cpp
    optimizer/test/src/SetConvolutionMethodTransformation.cpp
    optimizer/test/src/TransformationTest.cpp
)


set(optimizer_test_include
    optimizer/test/include/Cost.h
    optimizer/test/include/CostModel.h
    optimizer/test/include/CostDatabase.h
    optimizer/test/include/CostModelTest.h
    optimizer/test/include/CostTest.h
    optimizer/test/include/Environment.h
    optimizer/test/include/EnvironmentTest.h
    optimizer/test/include/ExampleCostModels.h
    optimizer/test/include/ExampleObjectives.h
    optimizer/test/include/ExampleOptimizers.h
    optimizer/test/include/ExampleTransformations.h
    optimizer/test/include/FuseLinearOperationsTransformation.h
    optimizer/test/include/GlobalOptimizer.h
    optimizer/test/include/GlobalOptimizerOptions.h
    optimizer/test/include/Objective.h
    optimizer/test/include/ObjectiveTest.h
    optimizer/test/include/OptimizerOptionsTest.h
    optimizer/test/include/OptimizerTest.h
    optimizer/test/include/OptimizerTestUtil.h
    optimizer/test/include/SequentialOptimizer.h
    optimizer/test/include/SetConvolutionMethodTransformation.h
    optimizer/test/include/TransformationTest.h
)

source_group("src" FILES ${optimizer_test_src})
source_group("include" FILES ${optimizer_test_include})

add_executable(${global_optimizer_test_name} ${optimizer_test_src} ${optimizer_test_include})
target_include_directories(${global_optimizer_test_name} PRIVATE optimizer/test/include ${ELL_LIBRARIES_DIR})
target_link_libraries(${global_optimizer_test_name} emitters model nodes testing utilities)

copy_shared_libraries(${global_optimizer_test_name})

set_property(TARGET ${global_optimizer_test_name} PROPERTY FOLDER "tests")

add_test(NAME ${global_optimizer_test_name} COMMAND ${global_optimizer_test_name})
set_test_library_path(${global_optimizer_test_name})
