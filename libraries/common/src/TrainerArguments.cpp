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

        parser.AddOption(
            lossArguments.lossFunction,
           "lossFunction",
           "lf",
           "Choice of loss function",
           {{"squared", LossArguments::LossFunction::squared}, {"hinge", LossArguments::LossFunction::hinge}, {"log", LossArguments::LossFunction::log}},
           "squared");

        parser.AddOption(
            lossArguments.lossFunctionParameter,
            "lossFunctionParameter",
            "lfp",
            "Parameter of the loss function",
            0.0);
    }
}
