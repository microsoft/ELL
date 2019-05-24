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

    NoFailOnUnimplemented(TestFindNearestCommonAncestor);
    FailOnException(TestIsCompleteAncestor);
    FailOnException(TestGetCompleteAncestors);
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

// Finding things

// This is the model graph for the next several tests
//     |
//    (A)
//     |
//    (B)
//   /   \
// (C)   (D)
//  |     |
//  |    (E)
//   \   /
//    (F)
//     |
//    (G)
//
void TestFindNearestCommonAncestor()
{
    throw TestNotImplementedException("TestFindNearestCommonAncestor");

    Model model;
    auto A = model.AddNode<InputNode<float>>(1);
    auto B = model.AddNode<nodes::UnaryOperationNode<float>>(A->output, nodes::UnaryOperationType::square);
    auto C = model.AddNode<nodes::UnaryOperationNode<float>>(B->output, nodes::UnaryOperationType::square);
    auto D = model.AddNode<nodes::UnaryOperationNode<float>>(B->output, nodes::UnaryOperationType::square);
    auto E = model.AddNode<nodes::UnaryOperationNode<float>>(D->output, nodes::UnaryOperationType::square);
    auto F = model.AddNode<nodes::BinaryOperationNode<float>>(C->output, E->output, nodes::BinaryOperationType::add);
    auto G = model.AddNode<OutputNode<float>>(F->output);

    ProcessTest("The nearest common ancestor to (C) and (E) is (B)", FindNearestCommonAncestor(C->output, E->output) == &B->output);
    ProcessTest("The nearest common ancestor to (E) and (F) is (B)", FindNearestCommonAncestor(E->output, F->output) == &B->output);
    ProcessTest("The nearest common ancestor to (F) and (G) is (F)", FindNearestCommonAncestor(F->output, G->output) == &F->output);
    ProcessTest("The nearest common ancestor to (D) and (E) is (D)", FindNearestCommonAncestor(D->output, E->output) == &D->output);
}

void TestIsCompleteAncestor()
{
    Model model;
    auto A = model.AddNode<InputNode<float>>(1);
    auto B = model.AddNode<nodes::UnaryOperationNode<float>>(A->output, nodes::UnaryOperationType::square);
    auto C = model.AddNode<nodes::UnaryOperationNode<float>>(B->output, nodes::UnaryOperationType::square);
    auto D = model.AddNode<nodes::UnaryOperationNode<float>>(B->output, nodes::UnaryOperationType::square);
    auto E = model.AddNode<nodes::UnaryOperationNode<float>>(D->output, nodes::UnaryOperationType::square);
    auto F = model.AddNode<nodes::BinaryOperationNode<float>>(C->output, E->output, nodes::BinaryOperationType::add);
    auto G = model.AddNode<OutputNode<float>>(F->output);

    ProcessTest("A is a complete ancestor of A", IsCompleteAncestor(A->output, A->output));
    ProcessTest("A is a complete ancestor of B", IsCompleteAncestor(B->output, A->output));
    ProcessTest("A is a complete ancestor of C", IsCompleteAncestor(C->output, A->output));
    ProcessTest("A is a complete ancestor of D", IsCompleteAncestor(D->output, A->output));
    ProcessTest("A is a complete ancestor of E", IsCompleteAncestor(E->output, A->output));
    ProcessTest("A is a complete ancestor of F", IsCompleteAncestor(F->output, A->output));
    ProcessTest("A is a complete ancestor of G", IsCompleteAncestor(G->output, A->output));

    ProcessTest("B is not a complete ancestor of A", !IsCompleteAncestor(A->output, B->output));
    ProcessTest("B is a complete ancestor of B", IsCompleteAncestor(B->output, B->output));
    ProcessTest("B is a complete ancestor of C", IsCompleteAncestor(C->output, B->output));
    ProcessTest("B is a complete ancestor of D", IsCompleteAncestor(D->output, B->output));
    ProcessTest("B is a complete ancestor of E", IsCompleteAncestor(E->output, B->output));
    ProcessTest("B is a complete ancestor of F", IsCompleteAncestor(F->output, B->output));
    ProcessTest("B is a complete ancestor of G", IsCompleteAncestor(G->output, B->output));

    ProcessTest("C is not a complete ancestor of A", !IsCompleteAncestor(A->output, C->output));
    ProcessTest("C is not a complete ancestor of B", !IsCompleteAncestor(B->output, C->output));
    ProcessTest("C is a complete ancestor of C", IsCompleteAncestor(C->output, C->output));
    ProcessTest("C is not a complete ancestor of D", !IsCompleteAncestor(D->output, C->output));
    ProcessTest("C is not a complete ancestor of E", !IsCompleteAncestor(E->output, C->output));
    ProcessTest("C is not a complete ancestor of F", !IsCompleteAncestor(F->output, C->output));
    ProcessTest("C is not a complete ancestor of G", !IsCompleteAncestor(G->output, C->output));

    ProcessTest("D is not a complete ancestor of A", !IsCompleteAncestor(A->output, D->output));
    ProcessTest("D is not a complete ancestor of B", !IsCompleteAncestor(B->output, D->output));
    ProcessTest("D is not a complete ancestor of C", !IsCompleteAncestor(C->output, D->output));
    ProcessTest("D is a complete ancestor of D", IsCompleteAncestor(D->output, D->output));
    ProcessTest("D is a complete ancestor of E", IsCompleteAncestor(E->output, D->output));
    ProcessTest("D is not a complete ancestor of F", !IsCompleteAncestor(F->output, D->output));
    ProcessTest("D is not a complete ancestor of G", !IsCompleteAncestor(G->output, D->output));

    ProcessTest("E is not a complete ancestor of A", !IsCompleteAncestor(A->output, E->output));
    ProcessTest("E is not a complete ancestor of B", !IsCompleteAncestor(B->output, E->output));
    ProcessTest("E is not a complete ancestor of C", !IsCompleteAncestor(C->output, E->output));
    ProcessTest("E is not a complete ancestor of D", !IsCompleteAncestor(D->output, E->output));
    ProcessTest("E is a complete ancestor of E", IsCompleteAncestor(E->output, E->output));
    ProcessTest("E is not a complete ancestor of F", !IsCompleteAncestor(F->output, E->output));
    ProcessTest("E is not a complete ancestor of G", !IsCompleteAncestor(G->output, E->output));

    ProcessTest("F is not a complete ancestor of A", !IsCompleteAncestor(A->output, F->output));
    ProcessTest("F is not a complete ancestor of B", !IsCompleteAncestor(B->output, F->output));
    ProcessTest("F is not a complete ancestor of C", !IsCompleteAncestor(C->output, F->output));
    ProcessTest("F is not a complete ancestor of D", !IsCompleteAncestor(D->output, F->output));
    ProcessTest("F is not a complete ancestor of E", !IsCompleteAncestor(E->output, F->output));
    ProcessTest("F is a complete ancestor of F", IsCompleteAncestor(F->output, F->output));
    ProcessTest("F is a complete ancestor of G", IsCompleteAncestor(G->output, F->output));

    ProcessTest("G is not a complete ancestor of A", !IsCompleteAncestor(A->output, G->output));
    ProcessTest("G is not a complete ancestor of B", !IsCompleteAncestor(B->output, G->output));
    ProcessTest("G is not a complete ancestor of C", !IsCompleteAncestor(C->output, G->output));
    ProcessTest("G is not a complete ancestor of D", !IsCompleteAncestor(D->output, G->output));
    ProcessTest("G is not a complete ancestor of E", !IsCompleteAncestor(E->output, G->output));
    ProcessTest("G is not a complete ancestor of F", !IsCompleteAncestor(F->output, G->output));
    ProcessTest("G is a complete ancestor of G", IsCompleteAncestor(G->output, G->output));
}

void TestGetCompleteAncestors()
{
    Model model;
    auto A = model.AddNode<InputNode<float>>(1);
    auto B = model.AddNode<nodes::UnaryOperationNode<float>>(A->output, nodes::UnaryOperationType::square);
    auto C = model.AddNode<nodes::UnaryOperationNode<float>>(B->output, nodes::UnaryOperationType::square);
    auto D = model.AddNode<nodes::UnaryOperationNode<float>>(B->output, nodes::UnaryOperationType::square);
    auto E = model.AddNode<nodes::UnaryOperationNode<float>>(D->output, nodes::UnaryOperationType::square);
    auto F = model.AddNode<nodes::BinaryOperationNode<float>>(C->output, E->output, nodes::BinaryOperationType::add);
    auto G = model.AddNode<OutputNode<float>>(F->output);

    using Set = std::unordered_set<const OutputPortBase*>;
    ProcessTest("Checking A's complete ancestors", GetCompleteAncestors(A->output) == Set{ &A->output });
    ProcessTest("Checking B's complete ancestors", GetCompleteAncestors(B->output) == Set{ &A->output, &B->output });
    ProcessTest("Checking C's complete ancestors", GetCompleteAncestors(C->output) == Set{ &A->output, &B->output, &C->output });
    ProcessTest("Checking D's complete ancestors", GetCompleteAncestors(D->output) == Set{ &A->output, &B->output, &D->output });
    ProcessTest("Checking E's complete ancestors", GetCompleteAncestors(E->output) == Set{ &A->output, &B->output, &D->output, &E->output });
    ProcessTest("Checking F's complete ancestors", GetCompleteAncestors(F->output) == Set{ &A->output, &B->output, &F->output });
    ProcessTest("Checking G's complete ancestors", GetCompleteAncestors(G->output) == Set{ &A->output, &B->output, &F->output, &G->output });

    // std::unordered_set<const OutputPortBase*> GetCompleteAncestors(const OutputPortBase* p);
}
