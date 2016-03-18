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
#include "OutputStreamImpostor.h"

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
        auto outStream = utilities::GetOutputStreamImpostor(compileArguments.outputCodeFile);

        // open file
        // ####
        auto layerStack = std::make_shared<CompilableMap>(layers::LayerStack::Load<CompilableMap>(mapLoadArguments.inputMapFile));
        auto map = layers::Map(layerStack);
        //        auto map = layers::Map::Load<CompilableMap>(mapLoadArguments.inputMapFile);

        // construct coordinate list
        auto coordinateList = layers::GetCoordinateList(*layerStack, mapLoadArguments.coordinateList);

        // output code
        // #### TODO: map->ToCode(outStream) ??? --- but then we need a compilableMap to be a map, not a layerstack 
        layerStack->ToCode(outStream, coordinateList);
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
