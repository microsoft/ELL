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
using layers::Coordinate;

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

        // open data file
        ifstream dataFStream = OpenIfstream(sharedArguments.dataFile);

        // create line iterator - read line by line sequentially
        SequentialLineIterator lineIterator(dataFStream);

        // create parser
        SparseEntryParser sparseEntryParser;

        // load dataset
        RowDataset data;
        if (true) // no map file
        {
            data = DatasetLoader::Load(lineIterator, sparseEntryParser);
        }
        else
        {
            Map map; // load Map
            MappedParser<SparseEntryParser> mappedParser(sparseEntryParser, map);
            data = DatasetLoader::Load(lineIterator, mappedParser);
        }

        // create loss function
        LogLoss loss(1);

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
        vector<Coordinate> inputCoordinates;
        Coordinate::FillBack(inputCoordinates, 0, dim);

        // create Map
        auto predictor = optimizer.GetPredictor();
        auto map = make_shared<Map>(dim);
        predictor.AddTo(*map, inputCoordinates);

        // save map to output file
        if (sharedArguments.outputFile != "")
        {
            ofstream outputFStream = OpenOfstream(sharedArguments.outputFile);
            map->Serialize(outputFStream);
        }
    }
    catch (runtime_error e)
    {
        cerr << "runtime error: " << e.what() << endl;
    }

    return 0;
}




