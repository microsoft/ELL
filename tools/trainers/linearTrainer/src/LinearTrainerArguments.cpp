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
            { { "SGD", Algorithm::SGD }, { "SDSGD", Algorithm::SDSGD }, { "SDCSGD", Algorithm::SDCSGD } },
            "SGD");

        parser.AddOption(regularization,
            "regularization",
            "r",
            "The L2 regularization parameter",
            1.0);

        parser.AddOption(normalize,
            "normalize",
            "n",
            "Perform sparsity-preserving normalization",
            false);
    }
}
