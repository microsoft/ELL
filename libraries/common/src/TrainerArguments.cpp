////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     TrainerArguments.cpp (common)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "TrainerArguments.h"

namespace ell
{
namespace common
{
    void ParsedTrainerArguments::AddArgs(utilities::CommandLineParser& parser)
    {
        parser.AddOption(
            numEpochs,
            "numEpochs",
            "ne",
            "The number of training epochs to perform",
            1);

        parser.AddOption(
            verbose,
            "verbose",
            "v",
            "Verbose output",
            false);

        parser.AddOption(
            lossFunctionArguments.lossFunction,
            "lossFunction",
            "lf",
            "Choice of loss function",
            { { "squared", LossFunctionArguments::LossFunction::squared }, { "hinge", LossFunctionArguments::LossFunction::hinge }, { "log", LossFunctionArguments::LossFunction::log }, {"smoothHinge", LossFunctionArguments::LossFunction::smoothHinge} },
            "squared");
    }
}
}
