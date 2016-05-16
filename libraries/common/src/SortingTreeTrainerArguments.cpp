////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     SortingTreeTrainerArguments.cpp (common)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "SortingTreeTrainerArguments.h"

namespace common
{
    void ParsedSortingTreeTrainerArguments::AddArgs(utilities::CommandLineParser & parser) {
        parser.AddOption(minSplitGain,
                         "minSplitGain",
                         "msg",
                         "The minimal gain required to split a leaf node",
                         0.0);
    }
}