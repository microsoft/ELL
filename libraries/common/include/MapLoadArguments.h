////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  [projectName]
//  File:     MapLoadArguments.h (common)
//  Authors:  Ofer Dekel
//
//  [copyright]
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

// types
#include "types.h"

// utilities
#include "CommandLineParser.h"

// stl
#include <string>

namespace common
{
    struct CoordinateListLoadArguments
    {
        uint32 ignoreSuffix = 0;
    };

    /// A struct that holds command line parameters for loading maps
    ///
    struct MapLoadArguments
    {
        std::string inputMapFile = "";
        CoordinateListLoadArguments coordinateListLoadArguments;
    };

    /// A version of MapLoadArguments that adds its members to the command line parser
    ///
    struct ParsedMapLoadArguments : public MapLoadArguments, public utilities::ParsedArgSet
    {
        /// Adds the arguments to the command line parser
        ///
        virtual void AddArgs(utilities::CommandLineParser& parser);
    };
}
