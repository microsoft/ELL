// main.cpp

#include "Compiler.h"
#include "CommandLineArguments.h"

// utilities
#include "JsonSerializer.h"
#include "files.h"
using utilities::OpenIfstream;
using utilities::OpenOfstream;

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
        ParsedCommandLineArguments args(commandLineParser);
        commandLineParser.ParseArgs();

        // open file
        ifstream mapFStream = OpenIfstream(args.mapFile);
        auto map = JsonSerializer::Load<Compiler>(mapFStream, "Base");
        
        ofstream svgOStream = OpenOfstream(args.codeFile);
    }

    catch (runtime_error e)
    {
        cerr << "runtime error: " << e.what() << std::endl;
    }

    // the end
    return 0;
}
