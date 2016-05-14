////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     ParsedStochasticGradientDescentLearnerArguments.h (stochasticGradientDescentTrainer)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

// utilities
#include "CommandLineParser.h"

// trainers
#include "StochasticGradientDescentLearner.h"

/// <summary> Parsed version of stochastic gradient descent parameters. </summary>
struct ParsedStochasticGradientDescentLearnerArguments : public trainers::StochasticGradientDescentLearnerParameters, public utilities::ParsedArgSet
{
    /// <summary> Adds the arguments to the command line parser. </summary>
    ///
    /// <param name="parser"> [in,out] The command line parser. </param>
    virtual void AddArgs(utilities::CommandLineParser& parser);
};
