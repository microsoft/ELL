////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     ForestTrainerArguments.h (common)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

// utilities
#include "CommandLineParser.h"

// trainers
#include "HistogramForestTrainer.h"
#include "SortingForestTrainer.h"

namespace ell
{
namespace common
{
    struct ForestTrainerArguments : public trainers::SortingForestTrainerParameters, public trainers::HistogramForestTrainerParameters
    {
        bool sortingTrainer;
    };

    /// <summary> Parsed version of sorting tree trainer parameters. </summary>
    struct ParsedForestTrainerArguments : public ForestTrainerArguments, public utilities::ParsedArgSet
    {
        /// <summary> Adds the arguments to the command line parser. </summary>
        ///
        /// <param name="parser"> [in,out] The command line parser. </param>
        void AddArgs(utilities::CommandLineParser& parser) override;
    };
}
}
