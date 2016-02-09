////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  [projectName]
//  File:     main.cpp (apply)
//  Authors:  Ofer Dekel
//
//  [copyright]
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "CommandLineParser.h" 

// common
#include "DataLoaders.h"
#include "MapLoadArguments.h" 
#include "DataLoadArguments.h" 
#include "DataSaveArguments.h" 

// utilities
#include "files.h"

// stl
#include <iostream>

#include <stdexcept>
using std::runtime_error;

int main(int argc, char* argv[])
{
    try
    {
        // create a command line parser
        utilities::CommandLineParser commandLineParser(argc, argv);

        // add arguments to the command line parser
        common::ParsedMapLoadArguments mapLoadArguments;
        common::ParsedDataLoadArguments dataLoadArguments;
        common::ParsedDataSaveArguments dataSaveArguments;

        commandLineParser.AddOptionSet(mapLoadArguments);
        commandLineParser.AddOptionSet(dataLoadArguments);
        commandLineParser.AddOptionSet(dataSaveArguments); 
        
        // parse command line
        commandLineParser.Parse();

        // create mapped data iterator based on the command line params
        auto dataIterator = GetDataIterator(dataLoadArguments, mapLoadArguments);

        // if output file specified, replace stdout with it 
        std::ofstream outputDataStream;
        if (dataSaveArguments.outputDataFile != "")
        {
            outputDataStream = utilities::OpenOfstream(dataSaveArguments.outputDataFile);
            std::cout.rdbuf(outputDataStream.rdbuf()); // replaces the streambuf in cout with the one in outputDataStream
        }

        // Load row by row
        while(dataIterator->IsValid())
        {
            // get next example
            auto supervisedExample = dataIterator->Get();

            // print the example
            supervisedExample.Print(std::cout);

            // move on 
            dataIterator->Next();
        }
    }
    catch(utilities::CommandLineParserErrorException exception)
    {
        std::cerr << "Command line parse error:" << std::endl;
        for(const auto& error : exception.GetParseErrors())
        {
            std::cerr << error.GetMessage() << std::endl;
        }
        return 0;
    }
    catch (utilities::CommandLineParserPrintHelpException)
    {}
    catch(runtime_error exception)
    {
        std::cerr << "Runtime error:" << std::endl;
        std::cerr << exception.what() << std::endl;
        return 1;
    }

    return 0;
}
