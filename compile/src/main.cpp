// main.cpp

#include "CompilableMap.h"
#include "CompileArguments.h"

// utilities
#include "files.h"
using utilities::JsonSerializer;
using utilities::OpenOfstream;
using utilities::OpenIfstream;

#include "CommandLineParser.h" 
using utilities::CommandLineParser;
using utilities::CommandLineParserErrorException;
using utilities::CommandLineParserPrintHelpException;

// layers
#include "Map.h"
using layers::Map;

// common
#include "CoordinateListTools.h"
using common::GetCoordinateList;

#include "MapLoadArguments.h"
using common::ParsedMapLoadArguments;

// stl
#include<iostream>
using std::cerr;
using std::cout;
using std::endl;

#include <fstream>
using std::ifstream;

#include <stdexcept>
using std::runtime_error;

#include <memory>
using std::dynamic_pointer_cast;

int main(int argc, char* argv[])
{
    try
    {
        // create a command line parser
        CommandLineParser commandLineParser(argc, argv);

        // add arguments to the command line parser
        ParsedMapLoadArguments mapLoadArguments;
        ParsedCompileArguments compileArguments;

        commandLineParser.AddOptionSet(mapLoadArguments);
        commandLineParser.AddOptionSet(compileArguments); 

        // parse command line
        commandLineParser.Parse();

        // open file
        //auto map = GetMap(mapLoadArguments);
        // open map file
        auto mapFStream = OpenIfstream(mapLoadArguments.inputMapFile); 
        auto map = JsonSerializer::Load<CompilableMap>(mapFStream, "Base");

        auto coordinateList = GetCoordinateList(map, mapLoadArguments.coordinateListLoadArguments);

        map.ToCode(coordinateList);

    }
    catch(const CommandLineParserPrintHelpException& ex)
    {
        cout << ex.GetHelpText() << endl;
    }
    catch(const CommandLineParserErrorException& exception)
    {
        cerr << "Command line parse error:" << endl;
        for(const auto& error : exception.GetParseErrors())
        {
            cerr << error.GetMessage() << endl;
        }
        return 0;
    }
    catch(runtime_error e)
    {
        cerr << "runtime error: " << e.what() << std::endl;
    }

    // the end
    return 0;
}
