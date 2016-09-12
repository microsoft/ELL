////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     main.cpp (print)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "PrintArguments.h"
#include "PrintModel.h"

// common
#include "LoadModel.h"
#include "ModelLoadArguments.h"

// utilities
#include "CommandLineParser.h"
#include "Exception.h"
#include "Files.h"
#include "OutputStreamImpostor.h"

// model
#include "Model.h"

// stl
#include <fstream>
#include <iostream>
#include <memory>
#include <stdexcept>

using namespace emll;

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
        auto model = common::LoadModel(modelLoadArguments.inputModelFile);

        // print model
        utilities::OutputStreamImpostor out = printArguments.outputStream;
        PrintModel(model, out);
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
