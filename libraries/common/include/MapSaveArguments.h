// MapSaveArguments.h

#pragma once

#include "types.h"

// utilities
#include "CommandLineParser.h"

// stl
#include <string>

namespace common
{
    /// A struct that holds command line parameters for loading maps
    ///
    struct MapSaveArguments
    {
        std::string outputMapFile = "";
    };

    /// A version of MapSaveArguments that adds its members to the command line parser
    ///
    struct ParsedMapSaveArguments : public MapSaveArguments, public utilities::ParsedArgSet
    {
        /// Adds the arguments to the command line parser
        ///
        virtual void AddArgs(utilities::CommandLineParser& parser);
    };
}
