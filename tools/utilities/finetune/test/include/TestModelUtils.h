////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     TestModelUtils.h (finetune_test)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

// Main driver function
void TestModelUtils();

// Individual tests
void TestIsNeuralNetworkPredictorNode();
void TestIsFullyConnectedLayerNode();
void TestIsConvolutionalLayerNode();

void TestAppendSinkNode();
void TestAppendOutputWithSink();
