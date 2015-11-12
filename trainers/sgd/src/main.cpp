// main.cpp

// utilities
#include "files.h"
#include "CommandLineParser.h" 
#include "random_engines.h"
#include "shared_arguments.h" 
#include "BinaryClassificationEvaluator.h"
using utilities::CommandLineParser;
using utilities::ParsedSharedArguments;
using utilities::BinaryClassificationEvaluator;
using utilities::GetRandomEngine;
using utilities::OpenIfstream;

// mappings
#include "mappings.h"
using namespace mappings;

_USE_DEFAULT_DESERIALIZER_  // use the default deserializer for mappings

// dataset
#include "SequentialLineIterator.h"
#include "ParsingIterator.h"
#include "SvmlightParser.h"
#include "loader.h"
using namespace dataset;

// linear
#include "SharedLinearBinaryPredictor.h"
using predictors::SharedLinearBinaryPredictor;

// convex_optimization
#include "AsgdOptimizer.h"
using namespace convex_optimization;

// loss_functions
#include "HingeLoss.h"
#include "LogLoss.h"
using namespace loss_functions;

// command line arguments
#include "args.h"

#include <fstream>
#include <string>
#include <stdexcept>
#include <vector>
#include <memory>

#include <random>
using std::default_random_engine;
using std::seed_seq;
using std::random_device;

#include <iostream>
using std::cerr;
using std::cout;
using std::endl;

int main(int argc, char* argv[])
{
    try
    {
        // parse the command line
        CommandLineParser cmd_parser(argc, argv);
        ParsedSharedArguments shared_arguments(cmd_parser);
        ParsedSgdArguments trainer_args(cmd_parser);
        cmd_parser.ParseArgs();

        // open data file
        ifstream data_fs = OpenIfstream(shared_arguments.data_file);

        // create line iterator - read line by line sequentially
        SequentialLineIterator li(data_fs);

        // create svmlight parser
        SvmlightParser p(shared_arguments.data_file_has_weights);

        // create random number generator
        auto rng = GetRandomEngine(shared_arguments.data_random_seed_string);

        // Load a dataset, permute, and get a data iterator
        auto data = Loader::Load(li, p);

        // create loss function
        LogLoss loss(1);

        // create sgd trainer
        uint dim = data.NumColumns();
        AsgdOptimizer optimizer(dim);

        // create evaluator
        BinaryClassificationEvaluator evaluator;

        // perform epochs
        for(int epoch = 0; epoch < trainer_args.num_epochs; ++epoch)
        {
            // randomly permute the data
            data.RandPerm(rng);

            // iterate over the entire permuted dataset
            auto training_iter = data.GetIterator();
            optimizer.Update(training_iter, loss, trainer_args.l2_regularization);

            // Evaluate
            auto eval_iter = data.GetIterator();
            evaluator.Evaluate(eval_iter, optimizer.GetPredictor(), loss);
        }

        // print loss and errors
        cout << "training error\n" << evaluator << endl;
    }
    catch (runtime_error e)
    {
        cerr << "runtime error: " << e.what() << endl;
    }

    return 0;
}




