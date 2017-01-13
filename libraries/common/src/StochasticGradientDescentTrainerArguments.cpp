////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     StochasticGradientDescentTrainerArguments.cpp (common)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "StochasticGradientDescentTrainerArguments.h"

namespace ell
{
namespace common
{
    void ParsedStochasticGradientDescentTrainerArguments::AddArgs(utilities::CommandLineParser& parser)
    {
        parser.AddOption(regularization,
                         "regularization",
                         "r",
                         "The L2 regularization parameter for the stochastic gradient descent algorithm",
                         1.0);
    }
}
}
