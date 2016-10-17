////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     main.cpp (apply)
//  Authors:  Chuck Jacobs
//
////////////////////////////////////////////////////////////////////////////////////////////////////

// test parameters
// bin\Release\apply -idf examples/data/testData.txt --inputModelFile examples/data/model_1.json -v -in 3128 -out 3133.output

// utilities
#include "CommandLineParser.h"
#include "Exception.h"
#include "Files.h"
#include "OutputStreamImpostor.h"

// data
#include "Example.h"
#include "Dataset.h"

// common
#include "DataLoadArguments.h"
#include "DataLoaders.h"
#include "DataSaveArguments.h"
#include "LoadModel.h"
#include "MapLoadArguments.h"
#include "MapSaveArguments.h"

// model
#include "DynamicMap.h"
#include "InputNode.h"
#include "Model.h"
#include "OutputNode.h"

// stl
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <tuple>

using namespace emll;

int main(int argc, char* argv[])
{
    try
    {
        // create a command line parser
        utilities::CommandLineParser commandLineParser(argc, argv);

        // add arguments to the command line parser
        common::ParsedDataLoadArguments dataLoadArguments;
        common::ParsedDataSaveArguments dataSaveArguments;
        common::ParsedMapLoadArguments mapLoadArguments;
        common::ParsedMapSaveArguments mapSaveArguments;

        commandLineParser.AddOptionSet(dataLoadArguments);
        commandLineParser.AddOptionSet(dataSaveArguments);
        commandLineParser.AddOptionSet(mapLoadArguments);
        commandLineParser.AddOptionSet(mapSaveArguments);

        bool verbose = false;
        commandLineParser.AddOption(verbose, "verbose", "v", "Verbose mode", false);

        // parse command line
        commandLineParser.Parse();

        if (verbose)
        {
            std::cout << commandLineParser.GetCurrentValuesString() << std::endl;
        }

        // load map
        model::DynamicMap map = common::LoadMap(mapLoadArguments);

        // load dataset
        if (verbose) std::cout << "Loading data from file: " << dataLoadArguments.inputDataFilename << std::endl;
        auto dataset = common::GetDataset(dataLoadArguments);

        // get output stream
        auto outputStream = dataSaveArguments.outputDataStream;
        auto mapInputSize = map.GetInputSize("input");

        auto datasetIterator = dataset.GetExampleReferenceIterator();
        while (datasetIterator.IsValid())
        {
            const auto& example = datasetIterator.Get();
            auto featureArray = example.GetDataVector().ToArray();

            featureArray.resize(mapInputSize);
            map.SetInputValue<double>("input", featureArray);

            // TODO: create data vector via Iterator.
            auto output = map.ComputeOutput<data::FloatDataVector>("output");
            auto mappedExample = data::DenseSupervisedExample{ std::move(output), example.GetMetadata() };

            mappedExample.Print(outputStream);
            outputStream << std::string("\n");
            datasetIterator.Next();
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
    catch (const utilities::Exception& exception)
    {
        std::cerr << "exception: " << exception.GetMessage() << std::endl;
        return 1;
    }

    return 0;
}
