////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     LinearSGDTrainerArguments.h (common)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

// utilities
#include "CommandLineParser.h"

// trainers
#include "LinearSGDTrainer.h"

namespace ell
{
namespace common
{
    typedef trainers::LinearSGDTrainerParameters LinearSGDTrainerArguments;

    /// <summary> Parsed version of stochastic gradient descent parameters. </summary>
    struct ParsedLinearSGDTrainerArguments : public LinearSGDTrainerArguments, public utilities::ParsedArgSet
    {
        /// <summary> Adds the arguments to the command line parser. </summary>
        ///
        /// <param name="parser"> [in,out] The command line parser. </param>
        virtual void AddArgs(utilities::CommandLineParser& parser) override;
    };
}
}
