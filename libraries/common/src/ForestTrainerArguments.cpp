////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     ForestTrainerArguments.cpp (common)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "ForestTrainerArguments.h"

namespace ell
{
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
                         "10");

        parser.AddOption(numRounds,
                         "numRounds",
                         "nr",
                         "The number of boosting rounds to perform",
                         "10");

        parser.AddOption(randomSeed,
                         "randomSeed",
                         "rs",
                         "Random seed used to choose random split threshold candidates",
                         "123456");

        parser.AddOption(thresholdFinderSampleSize,
                         "thresholdFinderSampleSize",
                         "tfss",
                         "The maximum number of samples to give the threshold finder",
                         10000);

        parser.AddOption(candidatesPerInput,
                         "candidatesPerInput",
                         "cpi",
                         "The number of split candidates to create per input element",
                         8);

        parser.AddOption(sortingTrainer,
                         "sortingTrainer",
                         "st",
                         "Use the sorting trainer instead of the histogram trainer",
                         false);
    }
}
}
