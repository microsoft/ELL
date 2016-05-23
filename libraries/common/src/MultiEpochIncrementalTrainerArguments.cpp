////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     MultiEpochIncrementalTrainerArguments.cpp (common)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "MultiEpochIncrementalTrainerArguments.h"

namespace common
{
    void ParsedMultiEpochIncrementalTrainerArguments::AddArgs(utilities::CommandLineParser& parser)
    {
        parser.AddOption(epochSize,
                         "epochSize",
                         "es",
                         "The maximal number of examples to use in each epoch; zero to use all of the examples.",
                         0);

        parser.AddOption(numEpochs,
                         "numEpochs",
                         "ne",
                         "The number of epochs.",
                         1);

        parser.AddOption(dataPermutationRandomSeed,
                         "dataPermutationRandomSeed",
                         "dprs",
                         "Seed for the random generator that controls data permutation between epochs.",
                         "123456");
    }
}