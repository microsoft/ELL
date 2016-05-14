////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     ParsedStochasticGradientDescentTrainerArguments.h (stochasticGradientDescent)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

// utilities
#include "CommandLineParser.h"

// trainers
#include "StochasticGradientDescentTrainer.h"

/// <summary> Parsed version of stochastic gradient descent parameters. </summary>
struct ParsedStochasticGradientDescentTrainerArguments : public trainers::IStochasticGradientDescentTrainer::Parameters, public utilities::ParsedArgSet
{
    /// <summary> Adds the arguments to the command line parser. </summary>
    ///
    /// <param name="parser"> [in,out] The command line parser. </param>
    virtual void AddArgs(utilities::CommandLineParser& parser);
};
