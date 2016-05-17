////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     BaggingIncrementalTrainerArguments.cpp (common)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "BaggingIncrementalTrainerArguments.h"

namespace common
{
    void ParsedBaggingIncrementalTrainerArguments::AddArgs(utilities::CommandLineParser & parser) 
    {
        parser.AddOption(numIterations,
                         "numIterations",
                         "ni",
                         "The number of bagging iterations",
                         1);

        parser.AddOption(bagSize,
                         "bagSize",
                         "bs",
                         "The number of training examples to randomly place in each bag",
                         1);

        parser.AddOption(dataPermutationRandomSeed,
                         "dataPermutationRandomSeed",
                         "dprs",
                         "Seed for the random generator that controls data permutation between epochs.",
                         "123456");

    }
}