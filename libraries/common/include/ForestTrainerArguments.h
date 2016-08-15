////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     ForestTrainerArguments.h (common)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

// utilities
#include "CommandLineParser.h"

// trainers
#include "HistogramForestTrainer.h"

namespace common
{
    using ForestTrainerArguments = trainers::ForestTrainerParameters;

    /// <summary> Parsed version of sorting tree trainer parameters. </summary>
    struct ParsedForestTrainerArguments : public ForestTrainerArguments, public utilities::ParsedArgSet
    {
        /// <summary> Adds the arguments to the command line parser. </summary>
        ///
        /// <param name="parser"> [in,out] The command line parser. </param>
        virtual void AddArgs(utilities::CommandLineParser& parser);
    };

    using HistogramForestTrainerArguments = trainers::HistogramForestTrainerParameters;

    /// <summary> Parsed version of sorting tree trainer parameters. </summary>
    struct ParsedHistogramForestTrainerArguments : public HistogramForestTrainerArguments, public utilities::ParsedArgSet
    {
        /// <summary> Adds the arguments to the command line parser. </summary>
        ///
        /// <param name="parser"> [in,out] The command line parser. </param>
        virtual void AddArgs(utilities::CommandLineParser& parser);
    };

}