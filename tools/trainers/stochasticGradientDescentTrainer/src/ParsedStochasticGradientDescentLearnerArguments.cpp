////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     ParsedSGDIncrementalTrainerArguments.cpp (stochasticGradientDescentTrainer)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "ParsedSGDIncrementalTrainerArguments.h"

void ParsedSGDIncrementalTrainerArguments::AddArgs(utilities::CommandLineParser& parser)
{
    parser.AddOption(regularization, 
                     "regularization", 
                     "r", 
                     "The L2 regularization parameter for the stochastic gradient descent algorithm",
                     1.0);
}
