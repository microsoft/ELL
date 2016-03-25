////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Rockmill
//  File:     SGDArguments.h (sgd)
//  Authors:  Ofer Dekel
//
//  [copyright]
//
////////////////////////////////////////////////////////////////////////////////////////////////////

// utilities
#include "CommandLineParser.h"

// stl
#include <string>

/// A struct that holds the command line parameters for the trainer
///
struct SgdArguments
{
    uint64_t numEpochs = 0;
    uint64_t epochSize = 0;
    double lossScale = 0;
    double l2Regularization = 0;
    std::string dataRandomPermutationSeedString = "";
};

/// A subclass of cmd_arguments that knows how to Add its members to the command line parser
///
struct ParsedSgdArguments : SgdArguments, utilities::ParsedArgSet
{
    virtual void AddArgs(utilities::CommandLineParser& parser)
    {
        parser.AddOption(numEpochs, "numEpochs", "e", "Number of epochs over the data", 3);
        parser.AddOption(epochSize, "epochSize", "es", "Number of random examples in each epoch, 0 uses the entire dataset", 0);
        parser.AddOption(lossScale, "lossScale", "ls", "The regularization parameter lambda", 1.0);
        parser.AddOption(l2Regularization, "l2Regularization", "l2", "The regularization parameter that controls the amount of L2 regularization", 0.01);
        parser.AddOption(dataRandomPermutationSeedString, "dataRandomPermutationSeedString", "seed", "String that seeds the random generator used to permute the dataset", "RandomString");
    }
};
