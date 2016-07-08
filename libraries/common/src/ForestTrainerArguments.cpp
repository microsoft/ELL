////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     ForestTrainerArguments.cpp (common)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "ForestTrainerArguments.h"

namespace common
{
    void ParsedForestTrainerArguments::AddArgs(utilities::CommandLineParser& parser) 
    {
        parser.AddOption(minSplitGain,
                         "minSplitGain",
                         "msg",
                         "The minimal gain required to split a leaf node",
                         0.0);
    }
}