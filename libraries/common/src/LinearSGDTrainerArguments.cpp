////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     LinearSGDTrainerArguments.cpp (common)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "LinearSGDTrainerArguments.h"

namespace ell
{
namespace common
{
    void ParsedLinearSGDTrainerArguments::AddArgs(utilities::CommandLineParser& parser)
    {
        parser.AddOption(regularization,
                         "regularization",
                         "r",
                         "The L2 regularization parameter for the stochastic gradient descent algorithm",
                         1.0);
    }
}
}
