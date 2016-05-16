////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     SGDIncrementalTrainerArguments.h (common)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

// utilities
#include "CommandLineParser.h"

// trainers
#include "SGDIncrementalTrainer.h"

namespace common
{
    using SGDIncrementalTrainerArguments = trainers::SGDIncrementalTrainerParameters;

    /// <summary> Parsed version of stochastic gradient descent parameters. </summary>
    struct ParsedSGDIncrementalTrainerArguments : public SGDIncrementalTrainerArguments, public utilities::ParsedArgSet
    {
        /// <summary> Adds the arguments to the command line parser. </summary>
        ///
        /// <param name="parser"> [in,out] The command line parser. </param>
        virtual void AddArgs(utilities::CommandLineParser& parser);
    };
}