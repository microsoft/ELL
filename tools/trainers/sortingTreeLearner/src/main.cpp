////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Rockmill
//  File:     main.cpp (sortingTreeLearner)
//  Authors:  Ofer Dekel
//
//  [copyright]
//
////////////////////////////////////////////////////////////////////////////////////////////////////

// utilities
#include "Files.h"
#include "OutputStreamImpostor.h"
#include "CommandLineParser.h" 
#include "RandomEngines.h"
#include "BinaryClassificationEvaluator.h"

// layers
#include "Map.h"
#include "Coordinate.h"
#include "CoordinateListTools.h"

// dataset
#include "SupervisedExample.h"

// common
#include "MapLoadArguments.h" 
#include "MapSaveArguments.h" 
#include "DataLoadArguments.h" 
#include "DataLoaders.h"
#include "LoadModel.h"

// trainers
#include "SortingTreeLearner.h"

// lossFunctions
#include "HingeLoss.h"
#include "LogLoss.h"

// stl
#include <iostream>
#include <stdexcept>

int main(int argc, char* argv[])
{
    try
    {
        // create a command line parser
        utilities::CommandLineParser commandLineParser(argc, argv);

        // add arguments to the command line parser
        common::ParsedMapLoadArguments mapLoadArguments;
        common::ParsedDataLoadArguments dataLoadArguments;
        common::ParsedMapSaveArguments mapSaveArguments;

        commandLineParser.AddOptionSet(mapLoadArguments);
        commandLineParser.AddOptionSet(dataLoadArguments);
        commandLineParser.AddOptionSet(mapSaveArguments);
        
        // parse command line
        commandLineParser.Parse();

        // if output file specified, replace stdout with it 
        auto outStream = utilities::GetOutputStreamImpostor(mapSaveArguments.outputModelFile);

        // load a model
        auto model = common::LoadModel(mapLoadArguments);

        // get output coordinate list and create the map
        auto outputCoordinateList = layers::BuildCoordinateList(model, dataLoadArguments.parsedDataDimension, mapLoadArguments.coordinateListString);
        layers::Map map(model, outputCoordinateList);

        // load dataset
        //auto rowDataset = common::GetRowDataset(dataLoadArguments, std::move(map));
        dataset::RowDataset<> rowDataset;

        // create sgd trainer
        lossFunctions::LogLoss loss;
        trainers::SortingTreeLearner<lossFunctions::LogLoss> sortingTreeLearner(loss);
        
        // create evaluator
        //utilities::BinaryClassificationEvaluator evaluator;

        // create random number generator
        //auto rng = utilities::GetRandomEngine(sgdArguments.dataRandomPermutationSeedString);

        // randomly permute the data
        //dataset->RandPerm(rng, epochSize);

        auto dataIterator = rowDataset.GetIterator();
        auto tree = sortingTreeLearner.Train(dataIterator, rowDataset.GetMaxDataVectorSize());

        tree.AddToModel(model, outputCoordinateList);
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
    catch (std::runtime_error exception)
    {
        std::cerr << "runtime error: " << exception.what() << std::endl;
        return 1;
    }

    return 0;
}
