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
    src/GRULayerNode.cpp
    src/IIRFilterNode.cpp
    src/IRNode.cpp
    src/LSTMLayerNode.cpp
    src/MatrixMatrixMultiplyNode.cpp
    src/MatrixVectorMultiplyNode.cpp
    src/NeuralNetworkPredictorNode.cpp
    src/PoolingLayerNode.cpp
    src/ProtoNNPredictorNode.cpp
    src/RecurrentLayerNode.cpp
    src/RegionDetectionLayerNode.cpp
    src/ScalingLayerNode.cpp
    src/SimpleConvolutionNode.cpp
    src/SingleElementThresholdNode.cpp
    src/SoftmaxLayerNode.cpp
    src/UnrolledConvolutionNode.cpp
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
    include/CompiledActivationFunctions.h
    include/ConstantNode.h
    include/ConvolutionalLayerNode.h
    include/DCTNode.h
    include/DebugSinkNode.h
    include/DelayNode.h
    include/DemultiplexerNode.h
    include/DiagonalConvolutionNode.h
    include/DotProductNode.h
    include/ExtremalValueNode.h
    include/FFTNode.h
    include/FilterBankNode.h
    include/ForestPredictorNode.h
    include/FullyConnectedLayerNode.h
    include/GRULayerNode.h
    include/HammingWindowNode.h
    include/IIRFilterNode.h
    include/IRNode.h
    include/L2NormSquaredNode.h
    include/LSTMLayerNode.h
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
    include/RecurrentLayerNode.h
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
    include/ValueSelectorNode.h
)

set (tcc
    tcc/AccumulatorNode.tcc
    tcc/BinaryFunctionNode.tcc
    tcc/BinaryOperationNode.tcc
    tcc/BinaryPredicateNode.tcc
    tcc/BroadcastFunctionNode.tcc
    tcc/BufferNode.tcc
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
    tcc/ValueSelectorNode.tcc
)

source_group("src" FILES ${src})
source_group("include" FILES ${include})
source_group("tcc" FILES ${tcc})

add_library(${library_name} ${src} ${include} ${tcc})

target_include_directories(${library_name}
    PUBLIC
        include
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
# test project 
#

set(test_name ${library_name}_test)

set(test_src
    test/src/main.cpp
    test/src/NeuralNetworkLayerNodesTest.cpp
    test/src/Nodes_test.cpp
)

set(test_include
    test/include/NeuralNetworkLayerNodesTest.h
    test/include/Nodes_test.h
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
)

target_link_libraries(${test_name}
    common
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
