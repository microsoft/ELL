#
# cmake file
#

set (library_name nodes)

set (include include/AccumulatorNode.h
             include/ActivationLayerNode.h
             include/BatchNormalizationLayerNode.h
             include/BiasLayerNode.h
             include/BinaryConvolutionalLayerNode.h
             include/BinaryOperationNode.h
             include/BinaryPredicateNode.h
             include/BroadcastFunctionNode.h
             include/ConstantNode.h
             include/ConvolutionalLayerNode.h
             include/DelayNode.h
             include/DemultiplexerNode.h
             include/DotProductNode.h
             include/DTWDistanceNode.h
             include/ExtremalValueNode.h
             include/ForestPredictorNode.h
             include/FullyConnectedLayerNode.h
             include/IRNode.h
             include/LinearPredictorNode.h
             include/L2NormNode.h
             include/MovingAverageNode.h
             include/MatrixMatrixMultiplyNode.h
             include/MatrixVectorMultiplyNode.h
             include/MatrixVectorProductNode.h
             include/MovingVarianceNode.h
             include/MultiplexerNode.h
             include/NeuralNetworkLayerNode.h
             include/NeuralNetworkPredictorNode.h
             include/PoolingLayerNode.h
             include/PortMemoryLayout.h
             include/ProtoNNPredictorNode.h
             include/ReorderDataNode.h
             include/ReshapeImageNode.h
             include/ScalingLayerNode.h
             include/SingleElementThresholdNode.h
             include/SinkNode.h
             include/SoftmaxLayerNode.h
             include/SourceNode.h
             include/SumNode.h
             include/TypeCastNode.h
             include/UnaryOperationNode.h
             include/ValueSelectorNode.h)

set (src src/ActivationLayerNode.cpp
         src/BatchNormalizationLayerNode.cpp
         src/BiasLayerNode.cpp
         src/BinaryConvolutionalLayerNode.cpp
         src/ConstantNode.cpp
         src/ConvolutionalLayerNode.cpp
         src/FullyConnectedLayerNode.cpp
         src/IRNode.cpp
         src/LinearPredictorNode.cpp
         src/MatrixMatrixMultiplyNode.cpp
         src/MatrixVectorMultiplyNode.cpp
         src/PortMemoryLayout.cpp
         src/ProtoNNPredictorNode.cpp
         src/NeuralNetworkPredictorNode.cpp
         src/PoolingLayerNode.cpp
         src/ReorderDataNode.cpp
         src/ScalingLayerNode.cpp
         src/SingleElementThresholdNode.cpp
         src/SoftmaxLayerNode.cpp)

set (tcc tcc/AccumulatorNode.tcc
         tcc/BinaryOperationNode.tcc
         tcc/BinaryPredicateNode.tcc
         tcc/BroadcastFunctionNode.tcc
         tcc/ConstantNode.tcc
         tcc/DelayNode.tcc
         tcc/DemultiplexerNode.tcc
         tcc/DotProductNode.tcc
         tcc/DTWDistanceNode.tcc
         tcc/ExtremalValueNode.tcc
         tcc/ForestPredictorNode.tcc
         tcc/L2NormNode.tcc
         tcc/MatrixVectorProductNode.tcc
         tcc/MovingAverageNode.tcc
         tcc/MovingVarianceNode.tcc
         tcc/MultiplexerNode.tcc
         tcc/NeuralNetworkLayerNode.tcc
         tcc/NeuralNetworkPredictorNode.tcc         
         tcc/ReorderDataNode.tcc
         tcc/ReshapeImageNode.tcc
         tcc/SinkNode.tcc
         tcc/SourceNode.tcc
         tcc/SumNode.tcc
         tcc/TypeCastNode.tcc
         tcc/UnaryOperationNode.tcc
         tcc/ValueSelectorNode.tcc
         tcc/SourceNode.tcc)

source_group("include" FILES ${include})
source_group("src" FILES ${src})
source_group("tcc" FILES ${tcc})

add_library(${library_name} ${src} ${include} ${tcc})
target_include_directories(${library_name} PUBLIC include)
if(CMAKE_COMPILER_IS_GNUCXX)
  target_compile_options(${library_name} PRIVATE -fPIC)
endif()
target_link_libraries(${library_name} model predictors utilities)
if(BLAS_FOUND)
  target_compile_definitions(${library_name} PUBLIC USE_BLAS=1)
endif()

set_property(TARGET ${library_name} PROPERTY FOLDER "libraries")

#
# test project
#

set (test_name ${library_name}_test)

set (test_src test/src/main.cpp 
              test/src/NeuralNetworkLayerNodesTest.cpp
              test/src/Nodes_test.cpp)

set (test_include test/include/NeuralNetworkLayerNodesTest.h
                  test/include/Nodes_test.h)

source_group("src" FILES ${test_src})
source_group("include" FILES ${test_include})

add_executable(${test_name} ${test_src} ${test_include} ${include})
target_include_directories(${test_name} PRIVATE test/include)
target_link_libraries(${test_name} nodes testing common model predictors utilities)
copy_shared_libraries(${test_name})

set_property(TARGET ${test_name} PROPERTY FOLDER "tests")

add_test(NAME ${test_name} COMMAND ${test_name})
