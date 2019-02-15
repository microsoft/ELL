////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     ModelTestUtilities.cpp (compile_test)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "ModelTestUtilities.h"

#include <common/include/LoadModel.h>

#include <model/include/InputNode.h>
#include <model/include/InputPort.h>
#include <model/include/Model.h>
#include <model/include/ModelTransformer.h>
#include <model/include/OutputNode.h>
#include <model/include/OutputPort.h>

#include <nodes/include/ExtremalValueNode.h>
#include <nodes/include/MovingAverageNode.h>

#include <iostream>

using namespace ell;

static bool g_isVerbose = false;

VerboseRegion::VerboseRegion(bool verbose) :
    _oldVerbose(IsVerbose())
{
    SetVerbose(verbose);
}

VerboseRegion::~VerboseRegion()
{
    SetVerbose(_oldVerbose);
}

void SetVerbose(bool verbose)
{
    g_isVerbose = verbose;
}

bool IsVerbose()
{
    return g_isVerbose;
}

void TestWithSerialization(ell::model::Map& map, std::string name, std::function<void(ell::model::Map& map, int)> body)
{
    // 3 iterations is important, because it finds bugs in reserialization of the deserialized model.
    for (int iteration = 0; iteration < 3; iteration++)
    {
        if (iteration > 0)
        {
            auto filename = utilities::FormatString("%s%d.json", name.c_str(), iteration);

            // archive the model
            common::SaveMap(map, filename);

            // unarchive the model
            map = common::LoadMap(filename);
        }

        body(map, iteration);
    }
}

void PrintMap(const model::Map& map)
{
    map.GetModel().Print(std::cout);
}

void PrintModel(const model::Model& model)
{
    model.Print(std::cout);
}

void PrintModel(const model::Model& model, const model::OutputPortBase* output)
{
    model.PrintSubset(std::cout, output);
}

model::Model GetSimpleModel()
{
    model::Model g;
    auto in = g.AddNode<model::InputNode<double>>(3);
    auto minAndArgMin = g.AddNode<nodes::ArgMinNode<double>>(in->output);
    auto maxAndArgMax = g.AddNode<nodes::ArgMaxNode<double>>(in->output);
    auto meanMin = g.AddNode<nodes::MovingAverageNode<double>>(minAndArgMin->val, 2);
    auto meanMax = g.AddNode<nodes::MovingAverageNode<double>>(maxAndArgMax->val, 2);
    g.AddNode<model::OutputNode<double>>(model::PortElements<double>({ meanMin->output, meanMax->output }));
    return g;
}

model::Model GetComplexModel()
{
    model::Model g;
    auto in = g.AddNode<model::InputNode<double>>(3);
    auto in2 = g.AddNode<model::InputNode<bool>>(3);
    auto minAndArgMin = g.AddNode<nodes::ArgMinNode<double>>(in->output);
    auto maxAndArgMax = g.AddNode<nodes::ArgMaxNode<double>>(in->output);
    auto meanMin = g.AddNode<nodes::MovingAverageNode<double>>(minAndArgMin->val, 2);
    auto meanMax = g.AddNode<nodes::MovingAverageNode<double>>(maxAndArgMax->val, 2);
    g.AddNode<model::OutputNode<double>>(model::PortElements<double>({ meanMin->output, meanMax->output }));
    g.AddNode<model::OutputNode<bool>>(model::PortElements<bool>({ in2->output }));
    return g;
}

model::Model GetTwoOutputModel()
{
    model::Model g;
    auto in = g.AddNode<model::InputNode<double>>(3);
    auto minAndArgMin = g.AddNode<nodes::ArgMinNode<double>>(in->output);
    auto maxAndArgMax = g.AddNode<nodes::ArgMaxNode<double>>(in->output);
    g.AddNode<nodes::MovingAverageNode<double>>(minAndArgMin->val, 8);
    g.AddNode<nodes::MovingAverageNode<double>>(maxAndArgMax->val, 8);
    return g;
}

model::Model GetLinearDebugNodeModel(int numDebugNodes)
{
    // in -> node1 -> node2 -> ... -> nodeN
    using DebugNode = DebugNode<double, int>;
    model::Model g;
    auto in = g.AddNode<model::InputNode<double>>(3);
    const model::OutputPort<double>* lastOutput = &(in->output);
    for (int i = 0; i < numDebugNodes; ++i)
    {
        auto node = g.AddNode<DebugNode>(*lastOutput, i + 1);
        lastOutput = &(node->output);
    }
    return g;
}

const DebugNode<double, int>* FindDebugNode(const model::Model& model, int tag)
{
    const DebugNode<double, int>* result = nullptr;
    model.Visit([&result, tag](const model::Node& node) {
        auto debugNode = dynamic_cast<const DebugNode<double, int>*>(&node);
        if (debugNode != nullptr && debugNode->GetDebugInfo() == tag)
        {
            result = debugNode;
        }
    });
    return result;
}

void PrintHeader(emitters::IRModuleEmitter& module)
{
    if (IsVerbose())
    {
        module.WriteToStream(std::cout, emitters::ModuleOutputFormat::cHeader);
    }
}

void PrintHeader(model::IRCompiledMap& compiledMap)
{
    if (IsVerbose())
    {
        compiledMap.WriteCode(std::cout, emitters::ModuleOutputFormat::cHeader);
    }
}

void PrintIR(emitters::IRModuleEmitter& module)
{
    if (IsVerbose())
    {
        module.WriteToStream(std::cout, emitters::ModuleOutputFormat::ir);
    }
}

void PrintIR(model::IRCompiledMap& compiledMap)
{
    if (IsVerbose())
    {
        compiledMap.WriteCode(std::cout, emitters::ModuleOutputFormat::ir);
    }
}

void PrintDiagnostics(emitters::IRDiagnosticHandler& handler)
{
    if (!IsVerbose()) return;

    // Print out any diagnostic messages
    auto messages = handler.GetMessages();
    if (messages.size() > 0)
    {
        std::cout << "Diagnostic messages" << std::endl;
        for (auto message : messages)
        {
            std::cout << message << std::endl;
        }
    }
    else
    {
        std::cout << "Diagnostic messages -- none" << std::endl;
    }
}

size_t GetShapeSize(const math::IntegerTriplet& shape)
{
    return shape[0] * shape[1] * shape[2];
}
