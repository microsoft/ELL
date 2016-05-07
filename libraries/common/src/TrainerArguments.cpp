////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     TrainerArguments.cpp (common)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "TrainerArguments.h"

namespace common
{
    void ParsedTrainerArguments::AddArgs(utilities::CommandLineParser& parser)
    {
        parser.AddOption(
            randomSeedString,
            "randomSeedString",
            "rss",
            "String used to seed the random number generator that permutes the training data",
            "1234567890");

        parser.AddOption(
            verbose,
            "verbose",
            "v",
            "Verbose output",
            false);
    }
}
