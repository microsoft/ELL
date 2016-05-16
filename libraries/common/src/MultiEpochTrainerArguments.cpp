////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     MultiEpochTrainerArguments.cpp (common)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "MultiEpochTrainerArguments.h"

namespace common
{
    void ParsedMultiEpochTrainerArguments::AddArgs(utilities::CommandLineParser& parser)
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
    }
}