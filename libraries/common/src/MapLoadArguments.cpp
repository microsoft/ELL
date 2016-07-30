////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     MapLoadArguments.cpp (common)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "MapLoadArguments.h"

namespace common
{
    void ParsedMapLoadArguments::AddArgs(utilities::CommandLineParser& parser)
    {
        modelLoadArguments.AddArgs(parser);

        parser.AddOption(
            coordinateListString,
            "coordinateList",
            "cl",
            "Coordinate list definition string (see README)",
            "e");
    }

    utilities::CommandLineParseResult ParsedMapLoadArguments::PostProcess(const utilities::CommandLineParser& parser)
    {
        return modelLoadArguments.PostProcess(parser);
    }
}
