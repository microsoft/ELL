////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     main.cpp (print)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "PrintableLayer.h"
#include "PrintableModel.h"
#include "PrintArguments.h"

// common
#include "ModelLoadArguments.h"
#include "LoadModel.h"

// utilities
#include "Files.h"
#include "OutputStreamImpostor.h"
#include "CommandLineParser.h"
#include "Exception.h"

// layers
#include "Map.h"
#include "Model.h"

// stl
#include <iostream>
#include <fstream>
#include <stdexcept>
#include <memory>

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
        auto outStream = utilities::GetOutputStreamImpostor(printArguments.outputSvgFile);

        // open model file
        auto model = common::LoadModel(modelLoadArguments);

        // convert model to printable model
        PrintableModel printableModel(model);
        
        // print to svg file
        printableModel.Print(outStream, printArguments);
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
    catch (utilities::Exception exception)
    {
        std::cerr << "runtime error: " << exception.GetMessage() << std::endl;
        return 1;
    }

    // the end
    return 0;
}
