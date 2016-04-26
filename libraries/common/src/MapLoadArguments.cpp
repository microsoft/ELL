////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Rockmill
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
            inputModelFile,
            "inputModelFile",
            "imf",
            "Path to the input model file",
            "");

        parser.AddOption(
            coordinateListString,
            "coordinateList",
            "cl",
            "Coordinate list definition string (see README)",
            "e");
    }
}
