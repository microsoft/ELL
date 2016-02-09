////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  [projectName]
//  File:     MapLoadArguments.cpp (common)
//  Authors:  Ofer Dekel
//
//  [copyright]
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "MapLoadArguments.h"

namespace common
{
    void ParsedMapLoadArguments::AddArgs(utilities::CommandLineParser& parser)
    {
        parser.AddOption(
            inputMapFile,
            "inputMapFile",
            "imf",
            "Path to the input map file",
            "");
        
        parser.AddOption(
            coordinateListLoadArguments.ignoreSuffix,
            "ignoreSuffix",
            "clis",
            "The number of layers from the bottom of the input map to ignore",
            0);
    }
}
