////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     LinearTrainerArguments.h (linearTrainer)
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
    struct LinearTrainerArguments
    {
        enum class Algorithm
        {
            SGD,
            SDSGD
        };

        Algorithm algorithm = Algorithm::SGD;

        double regularization;
    };

    /// <summary> Parsed version of LinearTrainerArguments. </summary>
    struct ParsedLinearTrainerArguments : public LinearTrainerArguments, public utilities::ParsedArgSet
    {
        /// <summary> Adds the arguments to the command line parser. </summary>
        ///
        /// <param name="parser"> [in,out] The command line parser. </param>
        virtual void AddArgs(utilities::CommandLineParser& parser) override;
    };
}
