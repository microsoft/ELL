////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     BaggingIncrementalTrainerArguments.h (common)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

// utilities
#include "CommandLineParser.h"

// trainers
#include "BaggingIncrementalTrainer.h"

namespace common
{
    using BaggingIncrementalTrainerArguments = trainers::BaggingIncrementalTrainerParameters;

    /// <summary> Parsed version of bagging parameters. </summary>
    struct ParsedBaggingIncrementalTrainerArguments : public BaggingIncrementalTrainerArguments, public utilities::ParsedArgSet
    {
        /// <summary> Adds the arguments to the command line parser. </summary>
        ///
        /// <param name="parser"> [in,out] The command line parser. </param>
        virtual void AddArgs(utilities::CommandLineParser& parser);
    };
}