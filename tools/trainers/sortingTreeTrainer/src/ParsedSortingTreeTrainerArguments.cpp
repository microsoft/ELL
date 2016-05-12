////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     ParsedSortingTreeTrainerArguments.cpp (sortingTreeTrainer)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "ParsedSortingTreeTrainerArguments.h"

void ParsedSortingTreeTrainerArguments::AddArgs(utilities::CommandLineParser & parser)
{
    parser.AddOption(minSplitGain,
                     "minSplitGain",
                     "msg",
                     "The minimal gain required to split a leaf node",
                     0.0);
}
