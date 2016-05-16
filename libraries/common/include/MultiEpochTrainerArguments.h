////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     MultiEpochTrainerArguments.h (common)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

// utilities
#include "CommandLineParser.h"

// trainers
#include "MultiEpochTrainer.h"

namespace common
{
    using MultiEpochTrainerArguments = trainers::MultiEpochTrainerParameters;

    /// <summary> Parsed version of multi-epoch trainer parameters. </summary>
    struct ParsedMultiEpochTrainerArguments : public MultiEpochTrainerArguments, public utilities::ParsedArgSet
    {
        /// <summary> Adds the arguments to the command line parser. </summary>
        ///
        /// <param name="parser"> [in,out] The command line parser. </param>
        virtual void AddArgs(utilities::CommandLineParser& parser);
    };

}