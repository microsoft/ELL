////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     ParsedSortingTreeTrainerArguments.h (sortingTreeTrainer)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

// utilities
#include "CommandLineParser.h"

// trainers
#include "SortingTreeTrainer.h"

/// <summary> Parsed version of sorting tree trainer parameters. </summary>
struct ParsedSortingTreeTrainerArguments : public trainers::ISortingTreeTrainer::Parameters, public utilities::ParsedArgSet
{
    /// <summary> Adds the arguments to the command line parser. </summary>
    ///
    /// <param name="parser"> [in,out] The command line parser. </param>
    virtual void AddArgs(utilities::CommandLineParser& parser);
};
