////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
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
            { { "SGD", Algorithm::SGD },{ "SDSGD", Algorithm::SDSGD } },
            "SGD");

        parser.AddOption(regularization,
            "regularization",
            "r",
            "The L2 regularization parameter",
            1.0);
    }
    }
