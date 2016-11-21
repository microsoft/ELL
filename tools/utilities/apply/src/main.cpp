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

        commandLineParser.AddOptionSet(dataLoadArguments);
        commandLineParser.AddOptionSet(dataSaveArguments);
        commandLineParser.AddOptionSet(mapLoadArguments);

        // parse command line
        commandLineParser.Parse();

        // load map
        model::DynamicMap map = common::LoadMap(mapLoadArguments);

        // load dataset
        auto dataset = common::GetDataset(dataLoadArguments);

        // get output stream
        auto outputStream = dataSaveArguments.outputDataStream;

        auto datasetIterator = dataset.GetExampleReferenceIterator();
        while (datasetIterator.IsValid())
        {
            const auto& example = datasetIterator.Get();
            auto mappedDataVector = map.Compute<data::FloatDataVector>(example.GetDataVector());
            auto mappedExample = data::DenseSupervisedExample{ std::move(mappedDataVector), example.GetMetadata() };

            mappedExample.Print(outputStream);
            outputStream << '\n';
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
