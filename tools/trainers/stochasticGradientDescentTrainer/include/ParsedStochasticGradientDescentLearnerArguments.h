////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     ParsedSGDIncrementalTrainerArguments.h (stochasticGradientDescentTrainer)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

// utilities
#include "CommandLineParser.h"

// trainers
#include "SGDIncrementalTrainer.h"

/// <summary> Parsed version of stochastic gradient descent parameters. </summary>
struct ParsedSGDIncrementalTrainerArguments : public trainers::SGDIncrementalTrainerParameters, public utilities::ParsedArgSet
{
    /// <summary> Adds the arguments to the command line parser. </summary>
    ///
    /// <param name="parser"> [in,out] The command line parser. </param>
    virtual void AddArgs(utilities::CommandLineParser& parser);
};
