////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     ProtoNNTrainerArguments.h (common)
//  Authors:  Suresh Iyengar
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

// utilities
#include "CommandLineParser.h"

// trainers
#include "ProtoNNTrainer.h"

namespace ell
{
namespace common
{
    typedef trainers::ProtoNNTrainerParameters ProtoNNTrainerArguments;

    /// <summary> Parsed version of stochastic gradient descent parameters. </summary>
    struct ParsedProtoNNTrainerArguments : public ProtoNNTrainerArguments, public utilities::ParsedArgSet
    {
        /// <summary> Adds the arguments to the command line parser. </summary>
        ///
        /// <param name="parser"> [in,out] The command line parser. </param>
        void AddArgs(utilities::CommandLineParser& parser) override;
    };
}
}
