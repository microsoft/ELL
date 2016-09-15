////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     main.cpp (sgdTrainer)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

// utilities
#include "CommandLineParser.h"
#include "Exception.h"
#include "Files.h"
#include "OutputStreamImpostor.h"

// dataset
#include "Example.h"
#include "RowDataset.h"

// common
#include "DataLoadArguments.h"
#include "DataLoaders.h"
#include "LoadModel.h"
#include "ModelLoadArguments.h"
#include "ModelSaveArguments.h"

// model
#include "InputNode.h"
#include "Model.h"
#include "DynamicMap.h"

// stl
#include <iostream>
#include <memory>
#include <stdexcept>
#include <tuple>

using namespace emll;

int main(int argc, char* argv[])
{
    try
    {
        // create a command line parser
        utilities::CommandLineParser commandLineParser(argc, argv);

        bool verbose = false;
        commandLineParser.AddOption(verbose, "verbose", "v", "Verbose mode", false);

        // add arguments to the command line parser
        common::ParsedDataLoadArguments dataLoadArguments;
        common::ParsedModelSaveArguments modelSaveArguments;

        commandLineParser.AddOptionSet(dataLoadArguments);
        commandLineParser.AddOptionSet(modelSaveArguments);

        // parse command line
        commandLineParser.Parse();

        if (verbose)
        {
            std::cout << "Apply" << std::endl;
            std::cout << commandLineParser.GetCurrentValuesString() << std::endl;
        }

        // load dataset
        if (verbose) std::cout << "Loading data ..." << std::endl;
        auto dataset = common::GetRowDataset(dataLoadArguments);
        size_t numColumns = dataLoadArguments.parsedDataDimension;

        // load map
        if (verbose) std::cout << "Loading map ..." << std::endl;
        auto mapFilename = "";
        auto map = common::LoadMap(mapFilename);

        dataset::GenericRowDataset outputDataset;

        // Get dataset iterator
        auto datasetIterator = dataset.GetIterator();
        while(datasetIterator.IsValid())
        {
            auto row = datasetIterator.Get();
            // TODO: run it through the map
            datasetIterator.Next();
        }

        // TOOD: save output dataset
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
