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
            coordinateList,
            "coordinateList",
            "cl",
            "Coordinate list definition string (see README)",
            "e");
    }
}
