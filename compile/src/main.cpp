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

        // if output file specified, replace stdout with it 
        std::ofstream outputDataStream;
        if (compileArguments.outputCodeFile != "")
        {
            outputDataStream = utilities::OpenOfstream(compileArguments.outputCodeFile);
            std::cout.rdbuf(outputDataStream.rdbuf()); // replaces the streambuf in cout with the one in outputDataStream
        }

        // open file
        auto map = layers::Map::Load<CompilableMap>(mapLoadArguments.inputMapFile);

        // construct coordinate list
        auto coordinateList = layers::GetCoordinateList(map, mapLoadArguments.coordinateList);

        // output code
        map.ToCode(coordinateList, std::cout);
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
