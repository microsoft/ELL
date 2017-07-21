////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     NeuralNetworkPredictorTests.h (predictors_test)
//  Authors:  Byron Changuion
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

template <typename ElementType>
void ActivationTest();

template <typename ElementType>
void LayerBaseTest();

template <typename ElementType>
void ActivationLayerTest();

template <typename ElementType>
void BatchNormalizationLayerTest();

template <typename ElementType>
void BiasLayerTest();

template <typename ElementType>
void InputLayerTest();

template <typename ElementType>
void ScalingLayerTest();

template <typename ElementType>
void FullyConnectedLayerTest();

template <typename ElementType>
void PoolingLayerTest();

template <typename ElementType>
void ConvolutionalLayerTest();

template <typename ElementType>
void BinaryConvolutionalLayerGemmTest();

template <typename ElementType>
void BinaryConvolutionalLayerBitwiseTest();

template <typename ElementType>
void SoftmaxLayerTest();

template <typename ElementType>
void NeuralNetworkPredictorTest();

//
// Archive tests
//
template <typename ElementType>
void ConvolutionalArchiveTest();

template <typename ElementType>
void BinaryConvolutionalArchiveTest();

#include "../tcc/NeuralNetworkPredictorTests.tcc"