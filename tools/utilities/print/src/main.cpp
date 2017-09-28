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
#include "ModelLoadArguments.h"

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
        common::ParsedModelLoadArguments modelLoadArguments;
        ParsedPrintArguments printArguments;
        commandLineParser.AddOptionSet(modelLoadArguments);
        commandLineParser.AddOptionSet(printArguments);
        commandLineParser.Parse();

        if (argc == 1)
        {
            std::cout << commandLineParser.GetHelpString() << std::endl;
            return 1;
        }
        if (modelLoadArguments.inputModelFile == "")
        {

            std::cout << "### Error: Please specify a model to print using the -imf argument" << std::endl
                      << std::endl;
            std::cout << commandLineParser.GetHelpString() << std::endl;
            return 1;
        }

        // open model file
        model::DynamicMap map;
        try
        {
            model::Model model = common::LoadModel(modelLoadArguments.inputModelFile);
            std::vector<std::pair<std::string, model::InputNodeBase*>> inputs;
            std::vector<std::pair<std::string, model::PortElementsBase>> outputs;
            map = model::DynamicMap(model, inputs, outputs);
        }
        catch (const utilities::Exception&)
        {
            // perhaps this file is a map
            map = common::LoadMap(modelLoadArguments.inputModelFile);
        }

        if (printArguments.refine)
        {
            model::TransformContext context;
            map.Refine(context, 100);
        }

        // print model
        utilities::OutputStreamImpostor out = printArguments.outputStream;
        auto lowerOutputFormat = ToLowercase(printArguments.outputFormat);
        if (lowerOutputFormat == "dgml" || lowerOutputFormat == "dot")
        {
            PrintGraph(map.GetModel(), lowerOutputFormat, out);
        }
        else
        {
            PrintModel(map.GetModel(), out);
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
