////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  [projectName]
//  File:     main.cpp (compile)
//  Authors:  Ofer Dekel
//
//  [copyright]
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "CompilableMap.h"
#include "CompileArguments.h"

// utilities
#include "files.h"
#include "CommandLineParser.h" 

// layers
#include "Map.h"

// common
#include "CoordinateListTools.h"
#include "MapLoadArguments.h"

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

        // open file
        auto map = layers::Map::Load<CompilableMap>(mapLoadArguments.inputMapFile);

        // construct coordinate list
        auto coordinateList = GetCoordinateList(map, mapLoadArguments.coordinateListLoadArguments);

        // TODO - allow output to cout or file
        map.ToCode(coordinateList, std::cout);

    }
    catch(const utilities::CommandLineParserPrintHelpException& ex)
    {
        std::cout << ex.GetHelpText() << std::endl;
    }
    catch(const utilities::CommandLineParserErrorException& exception)
    {
        std::cerr << "Command line parse error:" << std::endl;
        for(const auto& error : exception.GetParseErrors())
        {
            std::cerr << error.GetMessage() << std::endl;
        }
        return 0;
    }
    catch(std::runtime_error e)
    {
        std::cerr << "runtime error: " << e.what() << std::endl;
    }

    // the end
    return 0;
}
