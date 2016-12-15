////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     SGDIncrementalTrainerArguments.h (common)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

// utilities
#include "CommandLineParser.h"

// trainers
#include "SGDIncrementalTrainer.h"

namespace ell
{
namespace common
{
    typedef trainers::SGDIncrementalTrainerParameters SGDIncrementalTrainerArguments; 

    /// <summary> Parsed version of stochastic gradient descent parameters. </summary>
    struct ParsedSGDIncrementalTrainerArguments : public SGDIncrementalTrainerArguments, public utilities::ParsedArgSet
    {
        /// <summary> Adds the arguments to the command line parser. </summary>
        ///
        /// <param name="parser"> [in,out] The command line parser. </param>
        virtual void AddArgs(utilities::CommandLineParser& parser) override;
    };
}
}
