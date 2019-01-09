////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     AppendNodesTests.cpp (nodes_test)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "AppendNodesTests.h"

#include <nodes/include/BinaryOperationNode.h>
#include <nodes/include/ConstantNode.h>
#include <nodes/include/SumNode.h>
#include <nodes/include/UnaryOperationNode.h>

#include <testing/include/testing.h>

using namespace ell::model;
using namespace ell::nodes;
using namespace ell::testing;

void TestAppendNodes()
{
    TestAppenBinaryOperation();
    TestAppendConstant();
    TestAppendSum();
    TestAppenUnaryOperation();
}

void TestAppenBinaryOperation()
{
    Model model;
    auto inputNode1 = model.AddNode<InputNode<double>>(8);
    auto inputNode2 = model.AddNode<InputNode<double>>(8);
    [[maybe_unused]] const auto& exp = AppendBinaryOperation(model, inputNode1->output, inputNode2->output, BinaryOperationType::add);
    ProcessTest("TestAppendBinaryOperation", model.Size() == 3);
}

void TestAppendConstant()
{
    Model model;
    [[maybe_unused]] const auto& c = AppendConstant<int>(model, 10);
    ProcessTest("TestAppendConstant", model.Size() == 1);
}

void TestAppendSum()
{
    Model model;
    auto inputNode = model.AddNode<InputNode<double>>(8);
    [[maybe_unused]] const auto& s = AppendSum(model, inputNode->output);
    ProcessTest("TestAppendSum", model.Size() == 2);
}

void TestAppenUnaryOperation()
{
    Model model;
    auto inputNode = model.AddNode<InputNode<double>>(8);
    [[maybe_unused]] const auto& exp = AppendUnaryOperation(model, inputNode->output, UnaryOperationType::exp);
    ProcessTest("TestAppendUnaryOperation", model.Size() == 2);
}
