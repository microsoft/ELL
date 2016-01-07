// MapLoadArguments.h

#pragma once

#include "types.h"

// utilities
#include "CommandLineParser.h"
using utilities::CommandLineParser;

// stl
#include <string>
using std::string;

namespace utilities
{
    /// A struct that holds command line parameters for loading maps
    ///
    struct MapLoadArguments
    {
        string inputMapFile = "";
        uint32 inputMapIgnoreSuffix = 0;
    };

    /// A version of MapLoadArguments that adds its members to the command line parser
    ///
    struct ParsedMapLoadArguments : public MapLoadArguments, public ParsedArgSet
    {
        /// Adds the arguments to the command line parser
        ///
        virtual void AddArgs(CommandLineParser& parser);
    };
}
