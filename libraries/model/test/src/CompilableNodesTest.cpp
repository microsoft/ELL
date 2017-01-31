////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     CompilableNodesTest.cpp (compile_test)
//  Authors:  Umesh Madan, Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "CompilableNodesTest.h"
#include "CompilerTestUtilities.h"

// model
#include "DynamicMap.h"
#include "EmitterException.h"
#include "EmitterTypes.h"
#include "IRCompiledMap.h"
#include "IREmitter.h"
#include "IRMapCompiler.h"
#include "Model.h"

// nodes
#include "AccumulatorNode.h"
#include "BinaryOperationNode.h"
#include "BinaryPredicateNode.h"
#include "ConstantNode.h"
#include "DTWDistanceNode.h"
#include "DelayNode.h"
#include "DotProductNode.h"
#include "ExtremalValueNode.h"
#include "MultiplexerNode.h"
#include "SumNode.h"
#include "TypeCastNode.h"
#include "UnaryOperationNode.h"

// testing
#include "testing.h"

// stl
#include <iostream>
#include <ostream>
#include <string>

namespace ell
{

void TestCompilableAccumulatorNode()
{
    model::Model model;
    auto inputNode = model.AddNode<model::InputNode<double>>(3);
    auto accumNode = model.AddNode<nodes::AccumulatorNode<double>>(inputNode->output);
    auto map = model::DynamicMap(model, { { "input", inputNode } }, { { "output", accumNode->output } });
    auto compiledMapA = model::IRCompiledMap(map);
    auto compiledMap = std::move(compiledMapA);
    // compiledMap.WriteCode(std::cout, "asm");

    // compare output
    std::vector<std::vector<double>> signal = { { 1, 2, 3 }, { 4, 5, 6 }, { 7, 8, 9 }, { 3, 4, 5 }, { 2, 3, 2 }, { 1, 5, 3 }, { 1, 2, 3 }, { 4, 5, 6 }, { 7, 8, 9 }, { 7, 4, 2 }, { 5, 2, 1 } };
    //    VerifyCompiledOutput(map, compiledMap, signal, "AccumulatorNode");
}

void TestCompilableConstantNode()
{
    model::Model model;
    auto inputNode = model.AddNode<model::InputNode<double>>(3);
    auto constantNode = model.AddNode<nodes::ConstantNode<double>>(std::vector<double>{ 1.0, 2.0, 3.0 });
    auto dotNode = model.AddNode<nodes::DotProductNode<double>>(inputNode->output, constantNode->output);
    auto map = model::DynamicMap(model, { { "input", inputNode } }, { { "output", dotNode->output } });
    auto compiledMap = model::IRCompiledMap(map);
    // compiledMap.WriteCode(std::cout, "asm");

    // compare output
    std::vector<std::vector<double>> signal = { { 1, 2, 3 }, { 4, 5, 6 }, { 7, 8, 9 }, { 3, 4, 5 }, { 2, 3, 2 }, { 1, 5, 3 }, { 1, 2, 3 }, { 4, 5, 6 }, { 7, 8, 9 }, { 7, 4, 2 }, { 5, 2, 1 } };
    VerifyCompiledOutput(map, compiledMap, signal, "ConstantNode");
}

void TestCompilableDotProductNode()
{
    model::Model model;
    auto inputNode = model.AddNode<model::InputNode<double>>(3);
    auto constantNode = model.AddNode<nodes::ConstantNode<double>>(std::vector<double>{ 1.0, 2.0, 3.0 });
    auto dotNode = model.AddNode<nodes::DotProductNode<double>>(inputNode->output, constantNode->output);
    auto map = model::DynamicMap(model, { { "input", inputNode } }, { { "output", dotNode->output } });
    auto compiledMap = model::IRCompiledMap(map);
    // compiledMap.WriteCode(std::cout, "asm");

    // compare output
    std::vector<std::vector<double>> signal = { { 1, 2, 3 }, { 4, 5, 6 }, { 7, 8, 9 }, { 3, 4, 5 }, { 2, 3, 2 }, { 1, 5, 3 }, { 1, 2, 3 }, { 4, 5, 6 }, { 7, 8, 9 }, { 7, 4, 2 }, { 5, 2, 1 } };
    VerifyCompiledOutput(map, compiledMap, signal, "DotProductNode");
}

void TestCompilableDelayNode()
{
    model::Model model;
    auto inputNode = model.AddNode<model::InputNode<double>>(3);
    auto delayNode = model.AddNode<nodes::DelayNode<double>>(inputNode->output, 8);
    auto map = model::DynamicMap(model, { { "input", inputNode } }, { { "output", delayNode->output } });
    auto compiledMap = model::IRCompiledMap(map);
    compiledMap.WriteCode(std::cout, emitters::ModuleOutputFormat::ir);

    // compare output
    std::vector<std::vector<double>> signal = { { 1, 2, 3 }, { 4, 5, 6 }, { 7, 8, 9 }, { 3, 4, 5 }, { 2, 3, 2 }, { 1, 5, 3 }, { 1, 2, 3 }, { 4, 5, 6 }, { 7, 8, 9 }, { 7, 4, 2 }, { 5, 2, 1 } };
    VerifyCompiledOutput(map, compiledMap, signal, "DelayNode");
}

void TestCompilableDTWDistanceNode()
{
    model::Model model;
    std::vector<std::vector<double>> prototype = { { 1, 2, 3 }, { 4, 5, 6 }, { 7, 8, 9 } };
    auto inputNode = model.AddNode<model::InputNode<double>>(3);
    auto dtwNode = model.AddNode<nodes::DTWDistanceNode<double>>(inputNode->output, prototype);
    auto map = model::DynamicMap(model, { { "input", inputNode } }, { { "output", dtwNode->output } });
    auto compiledMap = model::IRCompiledMap(map);
    // compiledMap.WriteCode(std::cout, "asm");
    compiledMap.WriteCodeHeader(std::cout);
    std::cout << std::endl;

    // compare output
    std::vector<std::vector<double>> signal = { { 1, 2, 3 }, { 4, 5, 6 }, { 7, 8, 9 }, { 3, 4, 5 }, { 2, 3, 2 }, { 1, 5, 3 }, { 1, 2, 3 }, { 4, 5, 6 }, { 7, 8, 9 }, { 7, 4, 2 }, { 5, 2, 1 } };
    VerifyCompiledOutput(map, compiledMap, signal, "DTWDistanceNode");
}

class LabeledPrototype
{
public:
    LabeledPrototype() = default;
    LabeledPrototype(const LabeledPrototype&) = default;
    LabeledPrototype(int label, const std::vector<std::vector<double>>& prototype)
        : _label(label), _prototype(prototype){};
    int Label() const { return _label; }
    size_t Dimension() const { return _prototype[0].size(); }
    std::vector<std::vector<double>> Prototype() const { return _prototype; }

private:
    int _label = 0;
    std::vector<std::vector<double>> _prototype;
};

model::DynamicMap GenerateMulticlassDTWClassifier(const std::vector<LabeledPrototype>& prototypes)
{
    auto dim = prototypes[0].Dimension();

    ell::model::Model model;
    auto inputNode = model.AddNode<ell::model::InputNode<double>>(dim);

    std::vector<double> labels = { 0.0 };
    auto threshNode = model.AddNode<ell::nodes::ConstantNode<double>>(5.0);
    ell::model::PortElements<double> dtwOutputs(threshNode->output);

    std::vector<std::pair<int, ell::model::Node*>> dtwNodes;
    for (const auto& prototype : prototypes)
    {
        auto label = prototype.Label();
        labels.push_back(static_cast<double>(label));
        auto dtwNode = model.AddNode<ell::nodes::DTWDistanceNode<double>>(inputNode->output, prototype.Prototype());
        dtwNodes.emplace_back(label, dtwNode);
        dtwOutputs.Append(dtwNode->output);
    }
    auto labelsNode = model.AddNode<ell::nodes::ConstantNode<double>>(labels);
    auto argMinNode = model.AddNode<ell::nodes::ArgMinNode<double>>(dtwOutputs); // val, argVal
    auto selectNode = model.AddNode<ell::nodes::MultiplexerNode<double, int>>(labelsNode->output, argMinNode->argVal);
    model::DynamicMap result(model, { { "input", inputNode } }, { { "output", ell::model::PortElements<double>{ selectNode->output, argMinNode->val } } });
    return result;
}

void TestCompilableMulticlassDTW()
{
    model::Model model;
    std::vector<std::vector<double>> prototype1 = { { 1, 2, 3 }, { 4, 5, 6 }, { 7, 8, 9 } };
    std::vector<std::vector<double>> prototype2 = { { 9, 8, 7 }, { 6, 5, 4 }, { 3, 2, 1 } };
    std::vector<LabeledPrototype> prototypes = { { 3, prototype1 }, { 21, prototype2 } };

    auto map = GenerateMulticlassDTWClassifier(prototypes);
    auto compiledMap = model::IRCompiledMap(map);
    compiledMap.WriteCode(std::cout, emitters::ModuleOutputFormat::ir);

    // compare output
    std::vector<std::vector<double>> signal = { { 1, 2, 3 }, { 4, 5, 6 }, { 7, 8, 9 }, { 3, 4, 5 }, { 2, 3, 2 }, { 1, 5, 3 }, { 1, 2, 3 }, { 4, 5, 6 }, { 7, 8, 9 }, { 7, 4, 2 }, { 5, 2, 1 } };
    VerifyCompiledOutput(map, compiledMap, signal, "MulticlassDTW");
}

void TestCompilableSumNode()
{
    model::Model model;
    auto inputNode = model.AddNode<model::InputNode<double>>(3);
    auto sumNode = model.AddNode<nodes::SumNode<double>>(inputNode->output);
    auto map = model::DynamicMap(model, { { "input", inputNode } }, { { "output", sumNode->output } });
    auto compiledMap = model::IRCompiledMap(map);
    // compiledMap.WriteCode(std::cout, emitters::ModuleOutputFormat::ir);

    // compare output
    std::vector<std::vector<double>> signal = { { 1, 2, 3 }, { 4, 5, 6 }, { 7, 8, 9 }, { 3, 4, 5 }, { 2, 3, 2 }, { 1, 5, 3 }, { 1, 2, 3 }, { 4, 5, 6 }, { 7, 8, 9 }, { 7, 4, 2 }, { 5, 2, 1 } };
    VerifyCompiledOutput(map, compiledMap, signal, "SumNode");
}

void TestCompilableUnaryOperationNode()
{
    model::Model model;
    auto inputNode = model.AddNode<model::InputNode<double>>(3);
    auto testNode = model.AddNode<nodes::UnaryOperationNode<double>>(inputNode->output, emitters::UnaryOperationType::sqrt);
    auto map = model::DynamicMap(model, { { "input", inputNode } }, { { "output", testNode->output } });
    auto compiledMap = model::IRCompiledMap(map);
    // compiledMap.WriteCode(std::cout, "asm");

    // compare output
    std::vector<std::vector<double>> signal = { { 1, 2, 3 }, { 4, 5, 6 }, { 7, 8, 9 }, { 3, 4, 5 }, { 2, 3, 2 }, { 1, 5, 3 }, { 1, 2, 3 }, { 4, 5, 6 }, { 7, 8, 9 }, { 7, 4, 2 }, { 5, 2, 1 } };
    VerifyCompiledOutput(map, compiledMap, signal, "UnaryOpNode");
}

void TestCompilableBinaryOperationNode()
{
    model::Model model;
    auto inputNode = model.AddNode<model::InputNode<double>>(3);
    auto constantNode = model.AddNode<nodes::ConstantNode<double>>(std::vector<double>{ 1.0, 2.0, 3.0 });
    auto testNode = model.AddNode<nodes::BinaryOperationNode<double>>(inputNode->output, constantNode->output, emitters::BinaryOperationType::add);
    auto map = model::DynamicMap(model, { { "input", inputNode } }, { { "output", testNode->output } });
    auto compiledMap = model::IRCompiledMap(map);
    // compiledMap.WriteCode(std::cout, "asm");

    // compare output
    std::vector<std::vector<double>> signal = { { 1, 2, 3 }, { 4, 5, 6 }, { 7, 8, 9 }, { 3, 4, 5 }, { 2, 3, 2 }, { 1, 5, 3 }, { 1, 2, 3 }, { 4, 5, 6 }, { 7, 8, 9 }, { 7, 4, 2 }, { 5, 2, 1 } };
    VerifyCompiledOutput(map, compiledMap, signal, "BinaryOpNode");
}

// Problem: memory corruption for BinaryPredicateNode (probably because of bool foolishness)
void TestCompilableBinaryPredicateNode()
{
    model::Model model;
    auto inputNode = model.AddNode<model::InputNode<double>>(1);
    //    auto constantNode = model.AddNode<nodes::ConstantNode<double>>(std::vector<double>{ 1.0, 2.0, 3.0 });
    auto constantNode = model.AddNode<nodes::ConstantNode<double>>(std::vector<double>{ 2 });
    auto testNode = model.AddNode<nodes::BinaryPredicateNode<double>>(inputNode->output, constantNode->output, emitters::BinaryPredicateType::equal);
    auto map = model::DynamicMap(model, { { "input", inputNode } }, { { "output", testNode->output } });
    auto compiledMap = model::IRCompiledMap(map);
    // compiledMap.WriteCode(std::cout, "asm");

    // compare output
    std::vector<std::vector<double>> signal = { { 1 }, { 4 }, { 7 }, { 2 }, { 4 }, { 1 }, { 11 }, { 24 }, { 92 }, { 1 } };

    // TODO: Fix VerifyCompiledOutput --- types don't match for booleans
    PrintCompiledOutput(map, compiledMap, signal, "BinaryPredicateNode");

    std::cout << "Done with binary predicate" << std::endl;
}

void TestCompilableMultiplexerNode()
{
    model::Model model;
    auto inputNode = model.AddNode<model::InputNode<int>>(1);
    auto constantNode = model.AddNode<nodes::ConstantNode<double>>(std::vector<double>{ 1.0, 2.0, 3.0 });
    auto testNode = model.AddNode<nodes::MultiplexerNode<double, int>>(constantNode->output, inputNode->output);
    auto map = model::DynamicMap(model, { { "input", inputNode } }, { { "output", testNode->output } });
    auto compiledMap = model::IRCompiledMap(map);
    // compiledMap.WriteCode(std::cout, "asm");

    // compare output
    std::vector<std::vector<int>> signal = { { 0 }, { 1 }, { 0 }, { 1 }, { 1 }, { 0 }, { 0 }, { 1 }, { 1 }, { 0 } };
    VerifyCompiledOutput(map, compiledMap, signal, "MultiplexerNode");
    std::cout << "Done with multiplexer" << std::endl;
}

void TestCompilableTypeCastNode()
{
    model::Model model;
    auto inputNode = model.AddNode<model::InputNode<int>>(1);
    auto testNode = model.AddNode<nodes::TypeCastNode<int, double>>(inputNode->output);
    auto map = model::DynamicMap(model, { { "input", inputNode } }, { { "output", testNode->output } });
    auto compiledMap = model::IRCompiledMap(map);
    // compiledMap.WriteCode(std::cout, "asm");

    // compare output
    std::vector<std::vector<int>> signal = { { 1 }, { 4 }, { 7 }, { 2 }, { 4 }, { 1 }, { 11 }, { 24 }, { 92 }, { 1 } };
    VerifyCompiledOutput(map, compiledMap, signal, "MultiplexerNode");
    std::cout << "Done with typecast" << std::endl;
}
}