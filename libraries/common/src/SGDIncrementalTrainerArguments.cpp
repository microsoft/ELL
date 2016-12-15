////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     SGDIncrementalTrainerArguments.cpp (common)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "SGDIncrementalTrainerArguments.h"

namespace ell
{
namespace common
{
    void ParsedSGDIncrementalTrainerArguments::AddArgs(utilities::CommandLineParser& parser)
    {
        parser.AddOption(regularization,
                         "regularization",
                         "r",
                         "The L2 regularization parameter for the stochastic gradient descent algorithm",
                         1.0);
    }
}
}
