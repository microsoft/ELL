////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     main.cpp (compile)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

// utilities
#include "CommandLineParser.h"
#include "Exception.h"
#include "OutputStreamImpostor.h"

// model
#include "InputNode.h"
#include "Model.h"
#include "ModelTransformer.h"

// common
#include "LoadModel.h"

// stl
#include <fstream>
#include <iostream>
#include <stdexcept>

using namespace emll;

int main(int argc, char* argv[])
{
    try
    {
        std::string filename = "";

        // create a command line parser
        utilities::CommandLineParser commandLineParser(argc, argv);
        commandLineParser.AddOption(filename, "filename", "f", "Name of model file", "");

        // parse command line
        commandLineParser.Parse();

        auto model = common::LoadModel(filename);
        model::TransformContext context;
        model::ModelTransformer transformer;
        auto newModel = transformer.RefineModel(model, context);

        auto inputNodes = newModel.GetNodesByType<model::InputNode<double>>();
        // auto newInputNode = transformer.GetCorrespondingInputNode(inputNode);
        // auto newOutputPort = transformer.GetCorrespondingOutputPort(meanNode->output);
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
