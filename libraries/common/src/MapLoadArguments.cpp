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
            inputStackFile,
            "inputStackFile",
            "isf",
            "Path to the input stack file",
            "");

        parser.AddOption(
            coordinateListString,
            "coordinateList",
            "cl",
            "Coordinate list definition string (see README)",
            "e");
    }
}
