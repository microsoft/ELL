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

        // create the map 
        //Map map;

        // open data file
        ifstream dataFStream = OpenIfstream(sharedArguments.dataFile);

        // create line iterator - read line by line sequentially
        SequentialLineIterator lineIterator(dataFStream);

        // create parser
        SparseEntryParser sparseEntryParser;

        // load dataset
        RowDataset data;
        if (sharedArguments.inputMapFile == "")
        {
            data = DatasetLoader::Load(lineIterator, sparseEntryParser);
        }
        else
        {
            // open map file
            ifstream mapFStream = OpenIfstream(sharedArguments.inputMapFile);
            auto map = Map::Deserialize<Map>(mapFStream); // TODO, why does this return a shared ptr? myabe move semantics better?

            // create list of output coordinates
            auto outputCoordinates = CoordinateListFactory::IgnoreSuffix(*map, sharedArguments.inputMapIgnoreSuffix); // TODO outputCoordinates is a bad name, because it is actually the input
            
            // load data
            MappedParser<SparseEntryParser> mappedParser(sparseEntryParser, *map, outputCoordinates);
            data = DatasetLoader::Load(lineIterator, mappedParser);
        }

        // create loss function
        LogLoss loss;

        // create sgd trainer
        uint64 dim = data.NumColumns();
        AsgdOptimizer optimizer(dim);

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

        // TEMP = create coordinates of raw input
        auto inputCoordinates = CoordinateListFactory::Sequence(0, dim); // TODO create one map at the beginning of main

        // create Map
        auto predictor = optimizer.GetPredictor();
        auto map = make_shared<Map>(dim);
        predictor.AddTo(*map, inputCoordinates);

        // save map to output file
        if (sharedArguments.outputMapFile != "")
        {
            ofstream outputFStream = OpenOfstream(sharedArguments.outputMapFile);
            map->Serialize(outputFStream);
        }
    }
    catch (runtime_error e)
    {
        cerr << "runtime error: " << e.what() << endl;
    }

    return 0;
}




