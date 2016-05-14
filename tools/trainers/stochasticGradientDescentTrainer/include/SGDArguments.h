////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     SGDArguments.h (stochasticGradientDescentTrainer)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

// utilities
#include "CommandLineParser.h"

// stl
#include <string>

struct SgdArguments
{
    uint64_t numEpochs = 0;
    uint64_t epochSize = 0;
};

struct ParsedSgdArguments : SgdArguments, utilities::ParsedArgSet
{
    virtual void AddArgs(utilities::CommandLineParser& parser)
    {
        parser.AddOption(numEpochs, "numEpochs", "e", "Number of epochs over the data", 3);
        parser.AddOption(epochSize, "epochSize", "es", "Number of random examples in each epoch, 0 uses the entire dataset", 0);
    }
};
