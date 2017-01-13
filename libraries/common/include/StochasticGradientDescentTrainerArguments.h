////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     StochasticGradientDescentTrainerArguments.h (common)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

// utilities
#include "CommandLineParser.h"

// trainers
#include "StochasticGradientDescentTrainer.h"

namespace ell
{
namespace common
{
    typedef trainers::StochasticGradientDescentTrainerParameters StochasticGradientDescentTrainerArguments;

    /// <summary> Parsed version of stochastic gradient descent parameters. </summary>
    struct ParsedStochasticGradientDescentTrainerArguments : public StochasticGradientDescentTrainerArguments, public utilities::ParsedArgSet
    {
        /// <summary> Adds the arguments to the command line parser. </summary>
        ///
        /// <param name="parser"> [in,out] The command line parser. </param>
        virtual void AddArgs(utilities::CommandLineParser& parser) override;
    };
}
}
