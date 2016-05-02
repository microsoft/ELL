////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     main.cpp (compile)
//  Authors:  Ofer Dekel
//
//  [copyright]
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "CompilableMap.h"
#include "CompileArguments.h"

// utilities
#include "CommandLineParser.h" 
#include "OutputStreamImpostor.h"

// layers
#include "Map.h"
#include "CoordinateListTools.h"

// common
#include "MapLoadArguments.h"
#include "LoadModel.h"

// stl
#include<iostream>
#include <fstream>
#include <stdexcept>

int main(int argc, char* argv[])
{
    try
    {
        // create a command line parser
        utilities::CommandLineParser commandLineParser(argc, argv);

        // add arguments to the command line parser
        common::ParsedMapLoadArguments mapLoadArguments;
        ParsedCompileArguments compileArguments;

        commandLineParser.AddOptionSet(mapLoadArguments);
        commandLineParser.AddOptionSet(compileArguments); 

        // parse command line
        commandLineParser.Parse();

        // if output file specified, replace stdout with it 
        auto outStream = utilities::GetOutputStreamImpostor(compileArguments.outputCodeFile);

        // load the model, coordinates, and map
        auto model = common::LoadModel(mapLoadArguments);
        auto mapOutputCoordinates = layers::BuildCoordinateList(model, 0, mapLoadArguments.coordinateListString);

        // load the map
        layers::Map map(model, mapOutputCoordinates);

        // convert map to compilable map
        CompilableMap compilableMap(map);

        // output the code
        compilableMap.ToCode(outStream);
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
    catch (std::runtime_error exception)
    {
        std::cerr << "runtime error: " << exception.what() << std::endl;
        return 1;
    }

    // the end
    return 0;
}
