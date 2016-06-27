////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     main.cpp (apply)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "CommandLineParser.h" 

// common
#include "DataLoaders.h"
#include "MapLoadArguments.h" 
#include "DataLoadArguments.h" 
#include "DataSaveArguments.h" 

// utilities
#include "Files.h"
#include "OutputStreamImpostor.h"
#include "Exception.h"

// stl
#include <iostream>

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

        // if output file specified, replace stdout with it 
        auto outStream = utilities::GetOutputStreamImpostor(dataSaveArguments.outputDataFile);

        // create mapped data iterator based on the command line params
        auto dataIterator = GetDataIterator(dataLoadArguments, mapLoadArguments);

        // Load row by row
        while(dataIterator->IsValid())
        {
            // get next example
            auto supervisedExample = dataIterator->Get();

            // print the example
            supervisedExample.Print(outStream);

            // move on 
            dataIterator->Next();
        }
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
    catch (utilities::LogicException exception)
    {
        std::cerr << "runtime error: " << exception.GetMessage() << std::endl;
        return 1;
    }

    return 0;
}
