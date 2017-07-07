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
#include "OutputStreamImpostor.h"

// model
#include "Model.h"

// stl
#include <iostream>

using namespace ell;

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

        // open model file
        model::Model model;
        try
        {
            model = common::LoadModel(modelLoadArguments.inputModelFile);
        }
        catch (const utilities::Exception&)
        {
            auto map = common::LoadMap(modelLoadArguments.inputModelFile);
            model = map.GetModel();
        }

        // print model
        utilities::OutputStreamImpostor out = printArguments.outputStream;
        if (printArguments.outputFormat == "dgml")
        {
            PrintGraph(model, out);
        }
        else
        {
            PrintModel(model, out);
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
