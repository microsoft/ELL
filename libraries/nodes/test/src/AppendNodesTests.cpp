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
    TestConstant();
    TestSum();
    TestAppenUnaryOperation();
}

void TestAppenBinaryOperation()
{
    Model model;
    auto inputNode1 = model.AddNode<InputNode<double>>(8);
    auto inputNode2 = model.AddNode<InputNode<double>>(8);
    [[maybe_unused]] const auto& exp = BinaryOperation(inputNode1->output, inputNode2->output, BinaryOperationType::add);
    ProcessTest("TestBinaryOperation", model.Size() == 3);
}

void TestConstant()
{
    Model model;
    [[maybe_unused]] const auto& c = Constant<int>(model, 10);
    ProcessTest("TestConstant", model.Size() == 1);
}

void TestSum()
{
    Model model;
    auto inputNode = model.AddNode<InputNode<double>>(8);
    [[maybe_unused]] const auto& s = Sum(inputNode->output);
    ProcessTest("TestSum", model.Size() == 2);
}

void TestAppenUnaryOperation()
{
    Model model;
    auto inputNode = model.AddNode<InputNode<double>>(8);
    [[maybe_unused]] const auto& exp = Exp(inputNode->output);
    ProcessTest("TestUnaryOperation", model.Size() == 2);
}
