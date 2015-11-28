// main.cpp

// utilities
#include "files.h"
using utilities::OpenIfstream;

#include "CommandLineParser.h" 
using utilities::CommandLineParser;

#include "randomEngines.h"
using utilities::GetRandomEngine;

#include "SharedArguments.h" 
using utilities::ParsedSharedArguments;

#include "BinaryClassificationEvaluator.h"
using utilities::BinaryClassificationEvaluator;

// layers
#include "layers.h"
using layers::Map;
using layers::Coordinate;

_USE_DEFAULT_DESERIALIZER_  // use the default deserializer for layers

// dataset
#include "SequentialLineIterator.h"
#include "SparseEntryParser.h"
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
        CommandLineParser cmd_instanceParser(argc, argv);
        ParsedSharedArguments shared_arguments(cmd_instanceParser);
        ParsedSgdArguments trainer_args(cmd_instanceParser);
        cmd_instanceParser.ParseArgs();

        // open data file
        ifstream data_fs = OpenIfstream(shared_arguments.dataFile);

        // create line iterator - read line by line sequentially
        SequentialLineIterator li(data_fs);

        // create parser
        SparseEntryParser p;

        // create random number generator
        auto rng = GetRandomEngine(shared_arguments.dataRandomSeedString);

        // Load a dataset, permute, and get a data iterator
        auto data = DatasetLoader::Load(li, p);

        // create loss function
        LogLoss loss(1);

        // create sgd trainer
        uint64 dim = data.NumColumns();
        AsgdOptimizer optimizer(dim);

        // create evaluator
        BinaryClassificationEvaluator evaluator;

        // perform epochs
        for(int epoch = 0; epoch < trainer_args.numEpochs; ++epoch)
        {
            // randomly permute the data
            data.RandPerm(rng);

            // iterate over the entire permuted dataset
            auto trainSetIterator = data.GetIterator();
            optimizer.Update(trainSetIterator, loss, trainer_args.l2Regularization);

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
        map->Add(predictor, inputCoordinates);
    }
    catch (runtime_error e)
    {
        cerr << "runtime error: " << e.what() << endl;
    }

    return 0;
}




