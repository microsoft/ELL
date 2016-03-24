////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  [projectName]
//  File:     main.cpp (sgd)
//  Authors:  Ofer Dekel
//
//  [copyright]
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "SGDArguments.h"

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
#include "DataLoaders.h"
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
        commandLineParser.AddOptionSet(dataLoadArguments);
        commandLineParser.AddOptionSet(mapSaveArguments);
        commandLineParser.AddOptionSet(sgdArguments);
        
        // parse command line
        commandLineParser.Parse();

        // if output file specified, replace stdout with it 
        auto outStream = utilities::GetOutputStreamImpostor(mapSaveArguments.outputStackFile);

        // read map from file
        std::shared_ptr<layers::Map> map = common::GetMap(mapLoadArguments);

        // get the dataset
        auto dataset = common::GetDataset(dataLoadArguments, map);

        // create loss function
        lossFunctions::LogLoss loss;

        // create sgd trainer
        optimization::AsgdOptimizer optimizer(dataset->GetMaxExampleSize());

        // create evaluator
        utilities::BinaryClassificationEvaluator evaluator;

        // create random number generator
        auto rng = utilities::GetRandomEngine(sgdArguments.dataRandomPermutationSeedString);

        // perform epochs
        for(int epoch = 0; epoch < sgdArguments.numEpochs; ++epoch)
        {
            // randomly permute the data
            dataset->RandPerm(rng);

            // iterate over the entire permuted dataset
            auto trainSetIterator = dataset->GetIterator();
            optimizer.Update(trainSetIterator, dataset->NumExamples(), loss, sgdArguments.l2Regularization);

            // Evaluate training error
            auto evaluationIterator = dataset->GetIterator();
            evaluator.Evaluate(evaluationIterator, optimizer.GetPredictor(), loss);
        }

        // print loss and errors
        std::cerr << "training error\n" << evaluator << std::endl;

        // update the map with the newly learned layers
        auto predictor = optimizer.GetPredictor();

        predictor.AddToStack(map->GetStack(), map->GetOutputCoordinates());

        // output map
        map->GetStack().Save(outStream);
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
