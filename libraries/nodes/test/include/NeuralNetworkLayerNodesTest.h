/////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     NeuralNetworkLayerNodesTest.h (nodes_test)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

//
// Full network
//
void TestNeuralNetworkPredictorNode();

//
// Layer nodes
//
void TestActivationLayerNode();
void TestBatchNormalizationLayerNode();
void TestBiasLayerNode();
void TestBinaryConvolutionalLayerNode();
void TestConvolutionalLayerNode();
void TestFullyConnectedLayerNode();
void TestPoolingLayerNode();
void TestScalingLayerNode();
void TestSoftmaxLayerNode();

