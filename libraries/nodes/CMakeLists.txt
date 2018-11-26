#
# cmake file
#

set(library_name nodes)

set(src
    src/ActivationLayerNode.cpp
    src/BatchNormalizationLayerNode.cpp
    src/BiasLayerNode.cpp
    src/BinaryConvolutionalLayerNode.cpp
    src/ClockNode.cpp
    src/CompiledActivationFunctions.cpp
    src/ConstantNode.cpp
    src/ConvolutionalLayerNode.cpp
    src/DCTNode.cpp
    src/DiagonalConvolutionNode.cpp
    src/FFTNode.cpp
    src/FilterBankNode.cpp
    src/FullyConnectedLayerNode.cpp
    src/GRUNode.cpp
    src/IIRFilterNode.cpp
    src/IRNode.cpp
    src/LSTMNode.cpp
    src/MatrixMatrixMultiplyNode.cpp
    src/MatrixVectorMultiplyNode.cpp
    src/NeuralNetworkPredictorNode.cpp
    src/PoolingLayerNode.cpp
    src/ProtoNNPredictorNode.cpp
    src/RNNNode.cpp
    src/RegionDetectionLayerNode.cpp
    src/ScalingLayerNode.cpp
    src/SimpleConvolutionNode.cpp
    src/SingleElementThresholdNode.cpp
    src/SoftmaxLayerNode.cpp
    src/UnrolledConvolutionNode.cpp
    src/VoiceActivityDetectorNode.cpp
    src/WinogradConvolutionNode.cpp
)

set(include
    include/AccumulatorNode.h
    include/ActivationLayerNode.h
    include/BatchNormalizationLayerNode.h
    include/BiasLayerNode.h
    include/BinaryConvolutionalLayerNode.h
    include/BinaryFunctionNode.h
    include/BinaryOperationNode.h
    include/BinaryPredicateNode.h
    include/BroadcastFunctionNode.h
    include/BufferNode.h
    include/ClockNode.h
    include/ConcatenationNode.h
    include/CompiledActivationFunctions.h
    include/ConstantNode.h
    include/ConvolutionalLayerNode.h
    include/DCTNode.h
    include/DebugSinkNode.h
    include/DelayNode.h
    include/DemultiplexerNode.h
    include/DiagonalConvolutionNode.h
    include/DotProductNode.h
    include/DTWDistanceNode.h
    include/ExtremalValueNode.h
    include/FFTNode.h
    include/FilterBankNode.h
    include/ForestPredictorNode.h
    include/FullyConnectedLayerNode.h
    include/GRUNode.h
    include/HammingWindowNode.h
    include/IIRFilterNode.h
    include/IRNode.h
    include/L2NormSquaredNode.h
    include/LSTMNode.h
    include/LinearPredictorNode.h
    include/MatrixMatrixMultiplyNode.h
    include/MatrixVectorMultiplyNode.h
    include/MatrixVectorProductNode.h
    include/MovingAverageNode.h
    include/MovingVarianceNode.h
    include/MultiplexerNode.h
    include/NeuralNetworkLayerNode.h
    include/NeuralNetworkPredictorNode.h
    include/PoolingLayerNode.h
    include/ProtoNNPredictorNode.h
    include/ReceptiveFieldMatrixNode.h
    include/RNNNode.h
    include/RegionDetectionLayerNode.h
    include/ReorderDataNode.h
    include/ScalingLayerNode.h
    include/SimpleConvolutionNode.h
    include/SingleElementThresholdNode.h
    include/SinkNode.h
    include/SoftmaxLayerNode.h
    include/SourceNode.h
    include/SquaredEuclideanDistanceNode.h
    include/SumNode.h
    include/TypeCastNode.h
    include/UnaryOperationNode.h
    include/UnrolledConvolutionNode.h
    include/VoiceActivityDetectorNode.h
    include/ValueSelectorNode.h
    include/WinogradConvolutionNode.h
)

set (tcc
    tcc/AccumulatorNode.tcc
    tcc/BinaryFunctionNode.tcc
    tcc/BinaryOperationNode.tcc
    tcc/BinaryPredicateNode.tcc
    tcc/BroadcastFunctionNode.tcc
    tcc/BufferNode.tcc
    tcc/ConcatenationNode.tcc
    tcc/ConstantNode.tcc
    tcc/DTWDistanceNode.tcc
    tcc/DebugSinkNode.tcc
    tcc/DelayNode.tcc
    tcc/DemultiplexerNode.tcc
    tcc/DotProductNode.tcc
    tcc/ExtremalValueNode.tcc
    tcc/ForestPredictorNode.tcc
    tcc/HammingWindowNode.tcc
    tcc/L2NormSquaredNode.tcc
    tcc/LinearPredictorNode.tcc
    tcc/MatrixVectorProductNode.tcc
    tcc/MovingAverageNode.tcc
    tcc/MovingVarianceNode.tcc
    tcc/MultiplexerNode.tcc
    tcc/NeuralNetworkLayerNode.tcc
    tcc/NeuralNetworkPredictorNode.tcc
    tcc/ReceptiveFieldMatrixNode.tcc
    tcc/ReorderDataNode.tcc
    tcc/SinkNode.tcc
    tcc/SourceNode.tcc
    tcc/SquaredEuclideanDistanceNode.tcc
    tcc/SumNode.tcc
    tcc/TypeCastNode.tcc
    tcc/UnaryOperationNode.tcc
    tcc/VoiceActivityDetectorNode.tcc
    tcc/ValueSelectorNode.tcc
)

source_group("src" FILES ${src})
source_group("include" FILES ${include})
source_group("tcc" FILES ${tcc})

add_library(${library_name} ${src} ${include} ${tcc})

target_include_directories(${library_name}
    PRIVATE
        include
        ${ELL_LIBRARIES_DIR}
)

target_link_libraries(${library_name}
    math
    model
    predictors
    dsp
    utilities
)

if(BLAS_FOUND)
    target_compile_definitions(${library_name}
        PUBLIC
            USE_BLAS=1
    )
endif()

set_property(TARGET ${library_name} PROPERTY FOLDER "libraries")

#
# test projects
#

# general nodes tests
set(test_name ${library_name}_test)

set(test_src
    test/src/main.cpp
    test/src/NodesTests.cpp
    test/src/NodesTestData.cpp
)

set(test_include
    test/include/NodesTests.h
    test/include/NodesTestData.h
)

source_group("src" FILES ${test_src})
source_group("include" FILES ${test_include})

add_executable(${test_name}
    ${test_src}
    ${test_include}
)

target_include_directories(${test_name}
    PRIVATE
        test/include
        ${ELL_LIBRARIES_DIR}
)

target_link_libraries(${test_name}
    common
    data
    model
    nodes
    testing
    utilities
)

copy_shared_libraries(${test_name})

set_property(TARGET ${test_name} PROPERTY FOLDER "tests")

add_test(NAME ${test_name} COMMAND ${test_name})
set_test_library_path(${test_name})

# dsp nodes tests
set(test_name dsp_${library_name}_test)

set(test_src
    test/src/DSPNodesTest_main.cpp
    test/src/DSPNodesTests.cpp
    test/src/DSPNodesTiming.cpp
    test/src/NodesTestData.cpp
)

set(test_include
    test/include/DSPNodesTests.h
    test/include/DSPNodesTiming.h
    test/include/NodesTestData.h
    test/include/NodesTestUtilities.h
)

source_group("src" FILES ${test_src})
source_group("include" FILES ${test_include})

add_executable(${test_name}
    ${test_src}
    ${test_include}
)

target_include_directories(${test_name}
    PRIVATE
        test/include
        ${ELL_LIBRARIES_DIR}
)

target_link_libraries(${test_name}
    common
    math
    model
    nodes
    testing
    model_testing
    utilities
)

add_dependencies(${test_name} dspDataFiles)

copy_shared_libraries(${test_name})

set_property(TARGET ${test_name} PROPERTY FOLDER "tests")

add_test(NAME ${test_name} COMMAND ${test_name})
set_test_library_path(${test_name})

# neural net nodes test
set(test_name nn_${library_name}_test)

set(test_src
    test/src/NeuralNetworkLayerNodesTest_main.cpp
    test/src/NeuralNetworkLayerNodesTests.cpp
    test/src/NodesTestData.cpp
)

set(test_include
    test/include/NeuralNetworkLayerNodesTests.h
    test/include/NodesTestData.h
)

source_group("src" FILES ${test_src})
source_group("include" FILES ${test_include})

add_executable(${test_name}
    ${test_src}
    ${test_include}
    ${include}
)

target_include_directories(${test_name}
    PRIVATE
        test/include
        ${ELL_LIBRARIES_DIR}
)

target_link_libraries(${test_name}
    common
    dsp
    math
    model
    nodes
    predictors
    testing
    utilities
)

copy_shared_libraries(${test_name})

set_property(TARGET ${test_name} PROPERTY FOLDER "tests")

add_test(NAME ${test_name} COMMAND ${test_name})
set_test_library_path(${test_name})

#
# timing project
#

set(timing_name ${library_name}_timing)

set(timing_src
    test/src/timing_main.cpp
    test/src/DSPNodesTiming.cpp
)

set(timing_include
    test/include/DSPNodesTiming.h
    test/include/NodesTestUtilities.h
)

source_group("src" FILES ${timing_src})
source_group("include" FILES ${timing_include})

add_executable(${timing_name}
    ${timing_src}
    ${timing_include}
    ${include}
)

target_include_directories(${timing_name}
    PRIVATE
        test/include
        ${ELL_LIBRARIES_DIR}
)

target_link_libraries(${timing_name}
    dsp
    math
    model
    nodes
    predictors
    testing
    utilities
)

copy_shared_libraries(${timing_name})

set_property(TARGET ${timing_name} PROPERTY FOLDER "tests")
