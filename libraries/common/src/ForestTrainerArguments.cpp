////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     ForestTrainerArguments.cpp (common)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "ForestTrainerArguments.h"

namespace common
{
    void ParsedForestTrainerArguments::AddArgs(utilities::CommandLineParser& parser) 
    {
        parser.AddOption(minSplitGain,
                         "minSplitGain",
                         "msg",
                         "The minimal gain required to split a leaf node",
                         0.0);

        parser.AddOption(maxSplitsPerRound,
                         "maxSplitsPerRound",
                         "ms",
                         "The number of split operations to perform on each boosting round",
                         "1");
        parser.AddOption(numRounds,
                         "numRounds",
                         "nr",
                         "The number of boosting rounds to perform",
                         "10");
    }
}