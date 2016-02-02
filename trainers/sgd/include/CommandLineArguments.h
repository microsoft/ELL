// CommandLineArguments.h

#include "types.h"

// utilities
#include "CommandLineParser.h"
using utilities::ParsedArgSet;
using utilities::CommandLineParser;

// stl
#include <string>
using std::string;

/// A struct that holds the command line parameters for the trainer
///
struct SgdArguments
{
    uint64 numEpochs = 0;
    double lossScale = 0;
    double l2Regularization = 0;
    string dataRandomPermutationSeedString = "";
};

/// A subclass of cmd_arguments that knows how to Add its members to the command line parser
///
struct ParsedSgdArguments : SgdArguments, ParsedArgSet
{
    virtual void AddArgs(CommandLineParser& parser)
    {
        parser.AddOption(numEpochs, "numEpochs", "e", "Number of epochs over the data", 3);
        parser.AddOption(lossScale, "lossScale", "ls", "The regularization parameter lambda", 1.0);
        parser.AddOption(l2Regularization, "l2Regularization", "l2", "The regularization parameter that controls the amount of L2 regularization", 0.01);
        parser.AddOption(dataRandomPermutationSeedString, "dataRandomPermutationSeedString", "seed", "String that seeds the random generator used to permute the dataset", "RandomString");
    }
};
