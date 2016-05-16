////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     SortingTreeTrainerArguments.h (common)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

// utilities
#include "CommandLineParser.h"

// trainers
#include "SortingTreeTrainer.h"

namespace common
{
    using SortingTreeTrainerArguments = trainers::SortingTreeTrainerParameters;

    /// <summary> Parsed version of sorting tree trainer parameters. </summary>
    struct ParsedSortingTreeTrainerArguments : public SortingTreeTrainerArguments, public utilities::ParsedArgSet
    {
        /// <summary> Adds the arguments to the command line parser. </summary>
        ///
        /// <param name="parser"> [in,out] The command line parser. </param>
        virtual void AddArgs(utilities::CommandLineParser& parser);
    };
}