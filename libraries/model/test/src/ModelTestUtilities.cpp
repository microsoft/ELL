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

namespace ell
{
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

void NodePrinter(const model::Node& node)
{
    bool isFirstInputPort = true;
    std::cout << "node_" << node.GetId() << " (" << std::hex << (&node) << std::dec << ") = " << node.GetRuntimeTypeName() << "(";
    for (const auto& inputPort : node.GetInputPorts())
    {
        std::cout << (isFirstInputPort ? "" : ", ");
        isFirstInputPort = false;

        auto elements = inputPort->GetInputElements();
        if (elements.NumRanges() > 1)
        {
            std::cout << "{";
        }

        bool isFirstRange = true;
        for (const auto& range : elements.GetRanges())
        {
            std::cout << (isFirstRange ? "" : ", ");
            isFirstRange = false;

            auto port = range.ReferencedPort();
            std::cout << "node_" << port->GetNode()->GetId() << "." << port->GetName();
            if (!range.IsFullPortRange())
            {
                auto start = range.GetStartIndex();
                auto size = range.Size();
                std::cout << "[" << start << ":" << (start + size) << "]";
            }
        }

        if (elements.NumRanges() > 1)
        {
            std::cout << "}";
        }
    }
    std::cout << ")" << std::endl;
};

void PrintModel(const model::Model& model)
{
    model.Visit(NodePrinter);
}

void PrintModel(const model::Model& model, const model::Node* output)
{
    model.Visit(output, NodePrinter);
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

void PrintIR(emitters::IRModuleEmitter& module)
{
    if (IsVerbose())
    {
        module.DebugDump();
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
}
