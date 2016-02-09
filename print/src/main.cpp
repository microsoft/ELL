// main.cpp

#include "PrintableLayer.h"
#include "PrintableMap.h"
#include "PrintArguments.h"

// utilities
#include "JsonSerializer.h"
#include "files.h"
#include "CommandLineParser.h"

// layers
#include "Map.h"

// stl
#include<iostream>
#include <fstream>
#include <stdexcept>
#include <memory>

int main(int argc, char* argv[])
{
    try
    {
        // parse the command line
        utilities::CommandLineParser commandLineParser(argc, argv);
        ParsedPrintArguments printArguments;
        commandLineParser.AddOptionSet(printArguments);
        commandLineParser.Parse();

        // open map file
        auto map = layers::Map::Load<PrintableMap>(printArguments.inputMapFile);

        // open svg file
        auto outputSvgFStream = utilities::OpenOfstream(printArguments.outputSvgFile);
        
        // print to svg file
        map.Print(outputSvgFStream, printArguments);
    }
    catch (const utilities::CommandLineParserPrintHelpException& ex)
    {
        std::cout << ex.GetHelpText() << std::endl;
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
    catch (std::runtime_error e)
    {
        std::cerr << "runtime error: " << e.what() << std::endl;
        return 1;
    }

    // the end
    return 0;
}
