////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Rockmill
//  File:     MapSaveArguments.cpp (common)
//  Authors:  Ofer Dekel
//
//  [copyright]
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "MapSaveArguments.h"

namespace common
{
    void ParsedMapSaveArguments::AddArgs(utilities::CommandLineParser& parser)
    {
        parser.AddOption(
            outputStackFile,
            "outputStackFile",
            "osf",
            "Path to the output stack file",
            "");
    }
}
