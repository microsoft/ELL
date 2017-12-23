////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     main.cpp (print)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "PrintArguments.h"
#include "PrintGraph.h"
#include "PrintModel.h"

// common
#include "LoadModel.h"
#include "MapLoadArguments.h"

// utilities
#include "CommandLineParser.h"
#include "Exception.h"
#include "NeuralNetworkPredictorNode.h"
#include "OutputStreamImpostor.h"
#include "StringUtil.h"

// model
#include "Model.h"

// stl
#include <iostream>

using namespace ell;
using namespace ell::utilities;

bool IsNeuralNetworkPredictorNode(const ell::model::Node* node)
{
    if (dynamic_cast<const ell::nodes::NeuralNetworkPredictorNode<float>*>(node) != nullptr)
    {
        return true;
    }

    return dynamic_cast<const ell::nodes::NeuralNetworkPredictorNode<double>*>(node) != nullptr;
}

int main(int argc, char* argv[])
{
    try
    {
        // create a command line parser
        utilities::CommandLineParser commandLineParser(argc, argv);

        // add arguments to the command line parser
        common::ParsedMapLoadArguments mapLoadArguments;
        ParsedPrintArguments printArguments;
        commandLineParser.AddOptionSet(mapLoadArguments);
        commandLineParser.AddOptionSet(printArguments);
        commandLineParser.Parse();

        if (argc == 1)
        {
            std::cout << commandLineParser.GetHelpString() << std::endl;
            return 1;
        }

        // if no input specified, print help and exit
        if (!mapLoadArguments.HasInputFilename())
        {
            std::cout << commandLineParser.GetHelpString() << std::endl;
            return 1;
        }

        // Load model from file
        model::Map map = LoadMap(mapLoadArguments);

        if (printArguments.refine > 0)
        {
            model::TransformContext context;
            map.Refine(context, printArguments.refine);
        }

        // print model
        utilities::OutputStreamImpostor out = printArguments.outputStream;
        auto lowerOutputFormat = ToLowercase(printArguments.outputFormat);
        if (lowerOutputFormat == "dgml" || lowerOutputFormat == "dot")
        {
            PrintGraph(map.GetModel(), lowerOutputFormat, out, printArguments.includeNodeId);
        }
        else
        {
            PrintModel(map.GetModel(), out, printArguments.includeNodeId);
        }
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
