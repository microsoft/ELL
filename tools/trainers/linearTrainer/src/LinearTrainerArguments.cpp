////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     LinearTrainerArguments.cpp (linearTrainer)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "LinearTrainerArguments.h"

namespace ell
{
    void ParsedLinearTrainerArguments::AddArgs(utilities::CommandLineParser& parser)
    {
        parser.AddOption(
            algorithm,
            "algorithm",
            "a",
            "Choice of linear training algorithm",
            { { "SGD", Algorithm::SGD }, { "SparseDataSGD", Algorithm::SparseDataSGD }, { "SparseDataCenteredSGD", Algorithm::SparseDataCenteredSGD },{ "SDCA", Algorithm::SDCA } },
            "SDCA");

        parser.AddOption(normalize,
            "normalize",
            "n",
            "Perform sparsity-preserving normalization",
            false);

        parser.AddOption(regularization,
            "regularization",
            "r",
            "The L2 regularization parameter",
            1.0);

        parser.AddOption(desiredPrecision,
            "desiredPrecision",
            "de",
            "The desired duality gap at which to stop optimizing",
            1.0e-8);

        parser.AddOption(maxEpochs,
            "maxEpochs",
            "me",
            "The maximal number of optimizaiton epochs to run",
            20);

        parser.AddOption(permute,
            "permute",
            "p",
            "Whether or not to randomly permute the training data before each epoch",
            true);

        parser.AddOption(randomSeedString,
            "randomSeedString",
            "seed",
            "The random seed string",
            "ABCDEFG");
    }
}
