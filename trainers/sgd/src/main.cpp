// main.cpp

#include "CommandLineArguments.h"

// utilities
#include "files.h"
using utilities::OpenOfstream;

#include "CommandLineParser.h" 
using utilities::CommandLineParser;
using utilities::CommandLineParserErrorException;
using utilities::CommandLineParserPrintHelpException;

#include "randomEngines.h"
using utilities::GetRandomEngine;

#include "MapLoadArguments.h" 
using utilities::ParsedMapLoadArguments;

#include "MapSaveArguments.h" 
using utilities::ParsedMapSaveArguments;

#include "DataLoadArguments.h" 
using utilities::ParsedDataLoadArguments;

#include "BinaryClassificationEvaluator.h"
using utilities::BinaryClassificationEvaluator;

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
using common::GetRowDatasetMapCoordinates;

// optimization
#include "AsgdOptimizer.h"
using optimization::AsgdOptimizer;

// lossFunctions
#include "HingeLoss.h"
#include "LogLoss.h"
using namespace lossFunctions;

// stl
#include <iostream>
using std::cerr;
using std::cout;
using std::endl;

int main(int argc, char* argv[])
{
    try
    {
        // create a command line parser
        CommandLineParser commandLineParser(argc, argv);

        // add arguments to the command line parser
        ParsedMapLoadArguments mapLoadArguments;
        ParsedDataLoadArguments dataLoadArguments;
        ParsedMapSaveArguments mapSaveArguments;
        ParsedSgdArguments sgdArguments;

        commandLineParser.AddOptionSet(mapLoadArguments);
        commandLineParser.AddOptionSet(dataLoadArguments); // TODO - currently, hasWeights is ignored
        commandLineParser.AddOptionSet(mapSaveArguments);
        commandLineParser.AddOptionSet(sgdArguments);
        
        // parse command line
        try
        {
            commandLineParser.ParseArgs();
        }
        catch (const CommandLineParserPrintHelpException& ex)
        {
            // print help if requested
            cout << ex.GetHelpText() << endl;
        }

        // create and load a dataset, a map, and a coordinate list
        RowDataset dataset;
        Map map;
        CoordinateList inputCoordinates;
        GetRowDatasetMapCoordinates(dataLoadArguments, mapLoadArguments, dataset, map, inputCoordinates);

        // create loss function
        LogLoss loss;

        // create sgd trainer
        AsgdOptimizer optimizer(dataset.NumColumns());

        // create evaluator
        BinaryClassificationEvaluator evaluator;

        // create random number generator
        auto rng = GetRandomEngine(sgdArguments.dataRandomPermutationSeedString);

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
        cout << "training error\n" << evaluator << endl;

        // update the map with the newly learned layers
        auto predictor = optimizer.GetPredictor();
        predictor.AddTo(map, inputCoordinates);

        // save map to output file
        if (mapSaveArguments.outputMapFile != "")
        {
            auto outputMapFStream = OpenOfstream(mapSaveArguments.outputMapFile);
            map.Serialize(outputMapFStream);
        }
    }
    catch (const CommandLineParserErrorException& exception)
    {
        cerr << "Command line parse error:" << endl;
        for (const auto& error : exception.GetParseErrors())
        {
            cerr << error.GetMessage() << endl;
        }
        return 0;
    }
    catch (const CommandLineParserPrintHelpException& exception)
    {
        cerr << exception.GetHelpText() << endl;
    }
    catch (runtime_error exception)
    {
        cerr << "Runtime error:" << endl;
        cerr << exception.what() << endl;
        return 1;
    }

    return 0;
}
