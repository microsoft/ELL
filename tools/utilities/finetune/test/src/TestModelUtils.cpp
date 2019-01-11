////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     TestModelUtils.cpp (finetune_test)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "TestModelUtils.h"
#include "LoadTestModels.h"
#include "ModelUtils.h"


#include <model/include/Model.h>

#include <nodes/include/BinaryOperationNode.h>
#include <nodes/include/UnaryOperationNode.h>

#include <testing/include/testing.h>

#include <functional>

using namespace ell;
using namespace ell::model;
using namespace ell::testing;

namespace
{
bool CheckNodes(Model& model, int desiredIndex, std::function<bool(const Node*)> fn)
{
    bool success = true;
    auto it = model.GetNodeIterator();
    int count = 0;
    while (it.IsValid())
    {
        bool thisNodeOK = fn(it.Get()) == (count == desiredIndex);
        success &= thisNodeOK;
        it.Next();
        ++count;
    }
    return success;
}
} // namespace

void TestModelUtils()
{
    NoFailOnUnimplemented(TestIsNeuralNetworkPredictorNode);

    FailOnException(TestIsFullyConnectedLayerNode);
    FailOnException(TestIsConvolutionalLayerNode);

    NoFailOnUnimplemented(TestAppendSinkNode);
    NoFailOnUnimplemented(TestAppendOutputWithSink);
}

// Neural-net-related
void TestIsNeuralNetworkPredictorNode()
{
    throw TestNotImplementedException("TestIsNeuralNetworkPredictorNode");
}

// test model nodes:   0: input, 1: conv, 2: fully-connected, 3: output
void TestIsFullyConnectedLayerNode()
{
    auto model = GetNodeFindingTestModel();
    ProcessTest("TestIsFullyConnectedLayerNode", CheckNodes(model, 2, [](const Node* node) { return IsFullyConnectedLayerNode(node); }));
}

void TestIsConvolutionalLayerNode()
{
    auto model = GetNodeFindingTestModel();
    ProcessTest("TestIsConvolutionalLayerNode", CheckNodes(model, 1, [](const Node* node) { return IsConvolutionalLayerNode(node); }));
}

// Appending nodes or whatnot to models
void TestAppendSinkNode()
{
    throw TestNotImplementedException("TestAppendSinkNode");
}

void TestAppendOutputWithSink()
{
    throw TestNotImplementedException("TestAppendOutputWithSink");
}
