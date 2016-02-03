// main.cpp

#include "CommandLineArguments.h"

// utilities
#include "files.h"
#include "CommandLineParser.h" 
#include "randomEngines.h"
#include "BinaryClassificationEvaluator.h"

// layers
#include "Map.h"
#include "Coordinate.h"

// dataset
#include "SupervisedExample.h"

// common
#include "Loaders.h"
#include "MapLoadArguments.h" 
#include "MapSaveArguments.h" 
#include "DataLoadArguments.h" 

// optimization
#include "AsgdOptimizer.h"

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
        ParsedSgdArguments sgdArguments;

        commandLineParser.AddOptionSet(mapLoadArguments);
        commandLineParser.AddOptionSet(dataLoadArguments); // TODO - currently, hasWeights is ignored
        commandLineParser.AddOptionSet(mapSaveArguments);
        commandLineParser.AddOptionSet(sgdArguments);
        
        // parse command line
        commandLineParser.Parse();

        // create and load a dataset, a map, and a coordinate list
        dataset::RowDataset dataset;
        layers::Map map;
        layers::CoordinateList inputCoordinates;
        GetRowDatasetMapCoordinates(dataLoadArguments, mapLoadArguments, dataset, map, inputCoordinates);

        // create loss function
        lossFunctions::LogLoss loss;

        // create sgd trainer
        optimization::AsgdOptimizer optimizer(dataset.NumColumns());

        // create evaluator
        utilities::BinaryClassificationEvaluator evaluator;

        // create random number generator
        auto rng = utilities::GetRandomEngine(sgdArguments.dataRandomPermutationSeedString);

        // perform epochs
        for(int epoch = 0; epoch < sgdArguments.numEpochs; ++epoch)
        {
            // randomly permute the data
            dataset.RandPerm(rng);

            // iterate over the entire permuted dataset
            auto trainSetIterator = dataset.GetIterator();
            optimizer.Update(trainSetIterator, loss, sgdArguments.l2Regularization);

            // Evaluate training error
            auto evaluationIterator = dataset.GetIterator();
            evaluator.Evaluate(evaluationIterator, optimizer.GetPredictor(), loss);
        }

        // print loss and errors
        std::cout << "training error\n" << evaluator << std::endl;

        // update the map with the newly learned layers
        auto predictor = optimizer.GetPredictor();
        predictor.AddTo(map, inputCoordinates);

        // save map to output file
        if (mapSaveArguments.outputMapFile != "")
        {
            auto outputMapFStream = utilities::OpenOfstream(mapSaveArguments.outputMapFile);
            map.Serialize(outputMapFStream);
        }
    }
    catch (const utilities::CommandLineParserErrorException& exception)
    {
        std::cerr << "Command line parse error:" << std::endl;
        for (const auto& error : exception.GetParseErrors())
        {
            std::cerr << error.GetMessage() << std::endl;
        }
        return 0;
    }
    catch (const utilities::CommandLineParserPrintHelpException& exception)
    {
        std::cerr << exception.GetHelpText() << std::endl;
    }
    catch (std::runtime_error exception)
    {
        std::cerr << "Runtime error:" << std::endl;
        std::cerr << exception.what() << std::endl;
        return 1;
    }

    return 0;
}
