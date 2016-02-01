// main.cpp

#include "PrintableLayer.h"
#include "PrintableMap.h"
#include "PrintArguments.h"

// utilities
#include "JsonSerializer.h"
#include "files.h"
using utilities::OpenIfstream;
using utilities::OpenOfstream;

#include "CommandLineParser.h"
using utilities::CommandLineParserErrorException;
using utilities::CommandLineParserPrintHelpException;

// layers
#include "Map.h"
using layers::Map;

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
        // parse the command line
        CommandLineParser commandLineParser(argc, argv);
        ParsedPrintArguments printArguments;
        commandLineParser.AddOptionSet(printArguments);
        commandLineParser.Parse();

        // open map file
        auto mapFStream = OpenIfstream(printArguments.mapFile); // TODO: use common Arguments and common loader
        auto map = JsonSerializer::Load<PrintableMap>(mapFStream, "Base");
        
        auto outputSvgFStream = OpenOfstream(printArguments.svgFile);
        map.Print(outputSvgFStream, printArguments);
    }
    catch (const CommandLineParserPrintHelpException& ex)
    {
        cout << ex.GetHelpText() << endl;
        return 0;
    }
    catch (const CommandLineParserErrorException& exception)
    {
        cerr << "Command line parse error:" << endl;
        for (const auto& error : exception.GetParseErrors())
        {
            cerr << error.GetMessage() << endl;
        }
        return 1;
    }
    catch (runtime_error e)
    {
        cerr << "runtime error: " << e.what() << std::endl;
        return 1;
    }

    // the end
    return 0;
}
