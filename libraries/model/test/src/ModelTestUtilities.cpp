////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     ModelTestUtilities.cpp (compile_test)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "ModelTestUtilities.h"

// model
#include "InputNode.h"
#include "InputPort.h"
#include "Model.h"
#include "ModelTransformer.h"
#include "OutputNode.h"
#include "OutputPort.h"

// nodes
#include "ExtremalValueNode.h"
#include "MovingAverageNode.h"

// stl
#include <iostream>

using namespace ell;

static bool g_isVerbose = false;

VerboseRegion::VerboseRegion(bool verbose)
    : _oldVerbose(IsVerbose())
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

void PrintMap(const model::Map& map)
{
    map.GetModel().Print(std::cout);
}

void PrintModel(const model::Model& model)
{
    model.Print(std::cout);
}

void PrintModel(const model::Model& model, const model::Node* output)
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
