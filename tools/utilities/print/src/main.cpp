////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     main.cpp (print)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "PrintArguments.h"

// common
#include "ModelLoadArguments.h"
#include "LoadModel.h"

// utilities
#include "Files.h"
#include "OutputStreamImpostor.h"
#include "CommandLineParser.h"
#include "Exception.h"

// model
#include "Model.h"

// stl
#include <iostream>
#include <fstream>
#include <stdexcept>
#include <memory>

void PrintNode(const model::Node& node, utilities::OutputStreamImpostor& out)
{
    bool isFirstInputPort = true;
    std::cout << "node_" << node.GetId() << " = " << node.GetRuntimeTypeName() << "(";
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

void PrintModel(const model::Model& model, utilities::OutputStreamImpostor& out)
{
    model.Visit([&out](const model::Node& node) { PrintNode(node, out); });
}

int main(int argc, char* argv[])
{
    try
    {
        // create a command line parser
        utilities::CommandLineParser commandLineParser(argc, argv);

        // add arguments to the command line parser
        common::ParsedModelLoadArguments modelLoadArguments;
        ParsedPrintArguments printArguments;
        commandLineParser.AddOptionSet(modelLoadArguments);
        commandLineParser.AddOptionSet(printArguments);
        commandLineParser.Parse();

        // if output file specified, use it, otherwise use std::cout
        utilities::OutputStreamImpostor outStream(printArguments.outputFilename);

        // open model file
        auto model = common::LoadModel(modelLoadArguments.inputModelFile);
        
        // print model
        PrintModel(model, outStream);
    }
    catch (const utilities::CommandLineParserPrintHelpException& exception)
    {
        std::cout << exception.GetHelpText() << std::endl;
        return 0;
    }
    catch (const utilities::CommandLineParserErrorException& exception)
    {
        std::cerr << "Command line parse error:" << std::endl;
        for (const auto& error : exception.GetParseErrors())
        {
            std::cerr << error.GetMessage() << std::endl;
        }
        return 1;
    }
    catch (utilities::LogicException exception)
    {
        std::cerr << "runtime error: " << exception.GetMessage() << std::endl;
        return 1;
    }

    // the end
    return 0;
}
