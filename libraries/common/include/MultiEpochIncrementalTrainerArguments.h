////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     MultiEpochIncrementalTrainerArguments.h (common)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

// utilities
#include "CommandLineParser.h"

// trainers
#include "MultiEpochIncrementalTrainer.h"

namespace common
{
    using MultiEpochIncrementalTrainerArguments = trainers::MultiEpochIncrementalTrainerParameters;

    /// <summary> Parsed version of multi-epoch trainer parameters. </summary>
    struct ParsedMultiEpochIncrementalTrainerArguments : public MultiEpochIncrementalTrainerArguments, public utilities::ParsedArgSet
    {
        /// <summary> Adds the arguments to the command line parser. </summary>
        ///
        /// <param name="parser"> [in,out] The command line parser. </param>
        virtual void AddArgs(utilities::CommandLineParser& parser);
    };
}