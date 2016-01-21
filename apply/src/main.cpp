// main.cpp

#include "CommandLineParser.h" 
using utilities::CommandLineParser;
using utilities::CommandLineParserErrorException;
using utilities::CommandLineParserPrintHelpException;

// layers
#include "Map.h"
using layers::Map;

#include "Coordinate.h"
using layers::CoordinateList;

// dataset
#include "SupervisedExample.h"
using dataset::RowDataset;

// common
#include "Loaders.h"
using common::GetDataIterator;

#include "MapLoadArguments.h" 
using common::ParsedMapLoadArguments;

#include "DataLoadArguments.h" 
using common::ParsedDataLoadArguments;

#include "DataSaveArguments.h" 
using common::ParsedDataSaveArguments;

// utilities
#include "files.h"
using utilities::OpenOfstream;

// stl
#include <iostream>
using std::cerr;
using std::cout;
using std::endl;

#include <stdexcept>
using std::runtime_error;

int main(int argc, char* argv[])
{
    try
    {
        // create a command line parser
        CommandLineParser commandLineParser(argc, argv);

        // add arguments to the command line parser
        ParsedMapLoadArguments mapLoadArguments;
        ParsedDataLoadArguments dataLoadArguments;
        ParsedDataSaveArguments dataSaveArguments;

        commandLineParser.AddOptionSet(mapLoadArguments);
        commandLineParser.AddOptionSet(dataLoadArguments);
        commandLineParser.AddOptionSet(dataSaveArguments); 
        
        // parse command line
        commandLineParser.ParseArgs();

        // create mapped data iterator based on the command line params
        auto dataIterator = GetDataIterator(dataLoadArguments, mapLoadArguments);

        // if output file specified, replace stdout with it 
        ofstream outputDataStream;
        if (dataSaveArguments.outputDataFile != "")
        {
            outputDataStream = OpenOfstream(dataSaveArguments.outputDataFile);
            cout.rdbuf(outputDataStream.rdbuf()); // replaces the streambuf in cout with the one in outputDataStream
        }

        // Load row by row
        while(dataIterator->IsValid())
        {
            // get next example
            auto supervisedExample = dataIterator->Get();

            // print the example
            supervisedExample.Print(cout);

            // move on 
            dataIterator->Next();
        }
    }
    catch(CommandLineParserErrorException exception)
    {
        cerr << "Command line parse error:" << endl;
        for(const auto& error : exception.GetParseErrors())
        {
            cerr << error.GetMessage() << endl;
        }
        return 0;
    }
    catch (CommandLineParserPrintHelpException)
    {
    }
    catch(runtime_error exception)
    {
        cerr << "Runtime error:" << endl;
        cerr << exception.what() << endl;
        return 1;
    }

    return 0;
}
