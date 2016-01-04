// main.cpp

// utilities
#include "files.h"
using utilities::OpenIfstream;
using utilities::OpenOfstream;

#include "CommandLineParser.h" 
using utilities::CommandLineParser;

#include "randomEngines.h"
using utilities::GetRandomEngine;

#include "SharedArguments.h" 
using utilities::ParsedSharedArguments;

#include "BinaryClassificationEvaluator.h"
using utilities::BinaryClassificationEvaluator;

// layers
#include "Map.h"
using layers::Map;

#include "CoordinateListFactory.h"
using layers::CoordinateListFactory;

// dataset
#include "SequentialLineIterator.h"
#include "SparseEntryParser.h"
#include "MappedParser.h"
#include "ParsingIterator.h"
#include "DatasetLoader.h"
using namespace dataset;

// linear
#include "SharedLinearBinaryPredictor.h"
using predictors::SharedLinearBinaryPredictor;

// optimization
#include "AsgdOptimizer.h"
using namespace convex_optimization;

// loss_functions
#include "HingeLoss.h"
#include "LogLoss.h"
using namespace loss_functions;

// command line arguments
#include "args.h"

#include <iostream>
using std::cerr;
using std::cout;
using std::endl;

int main(int argc, char* argv[])
{
    try
    {
        // parse the command line
        CommandLineParser commandLineParser(argc, argv);
        ParsedSharedArguments sharedArguments(commandLineParser);
        ParsedSgdArguments trainerArguments(commandLineParser);
        commandLineParser.ParseArgs();

        // create a map 
        Map map;

        // create a list of input coordinates for this map
        CoordinateList inputCoordinates;

        // create a dataset
        RowDataset data;

        // open data file
        ifstream dataFStream = OpenIfstream(sharedArguments.dataFile);

        // create line iterator - read line by line sequentially
        SequentialLineIterator lineIterator(dataFStream);

        // create parser for sparse vectors (SVMLight format)
        SparseEntryParser sparseEntryParser;

        // handle two cases - input map specified or unspecified
        if (sharedArguments.inputMapFile == "")
        {
            // load data wihtout applying any map
            data = DatasetLoader::Load(lineIterator, sparseEntryParser);

            // create default map with single input layer
            map = Map(data.NumColumns());

            // create a coordinate list of this map
            inputCoordinates = CoordinateListFactory::Sequence(0, data.NumColumns()); 
        }
        else
        {
            // open map file
            ifstream mapFStream = OpenIfstream(sharedArguments.inputMapFile);
            map = JsonSerializer::Load<Map>(mapFStream, "Base");

            // create list of output coordinates
            inputCoordinates = CoordinateListFactory::IgnoreSuffix(map, sharedArguments.inputMapIgnoreSuffix);
            
            // load data
            MappedParser<SparseEntryParser> mappedParser(sparseEntryParser, map, inputCoordinates);
            data = DatasetLoader::Load(lineIterator, mappedParser);
        }

        // create loss function
        LogLoss loss;

        // create sgd trainer
        AsgdOptimizer optimizer(data.NumColumns());

        // create evaluator
        BinaryClassificationEvaluator evaluator;

        // create random number generator
        auto rng = GetRandomEngine(sharedArguments.dataRandomSeedString);

        // perform epochs
        for(int epoch = 0; epoch < trainerArguments.numEpochs; ++epoch)
        {
            // randomly permute the data
            data.RandPerm(rng);

            // iterate over the entire permuted dataset
            auto trainSetIterator = data.GetIterator();
            optimizer.Update(trainSetIterator, loss, trainerArguments.l2Regularization);

            // Evaluate
            auto evaluationIterator = data.GetIterator();
            evaluator.Evaluate(evaluationIterator, optimizer.GetPredictor(), loss);
        }

        // print loss and errors
        cout << "training error\n" << evaluator << endl;

        // update the map with the newly learned layers
        auto predictor = optimizer.GetPredictor();
        predictor.AddTo(map, inputCoordinates);

        // save map to output file
        if (sharedArguments.outputMapFile != "")
        {
            ofstream outputFStream = OpenOfstream(sharedArguments.outputMapFile);
            map.Serialize(outputFStream);
        }
    }
    catch (runtime_error e)
    {
        cerr << "runtime error: " << e.what() << endl;
    }

    return 0;
}




