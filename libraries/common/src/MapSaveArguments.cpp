////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     MapSaveArguments.cpp (common)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "MapSaveArguments.h"

namespace common
{
    void ParsedMapSaveArguments::AddArgs(utilities::CommandLineParser& parser)
    {
        parser.AddOption(
            outputModelFile,
            "outputModelFile",
            "omf",
            "Path to the output model file",
            "");
    }
}
