// MapSaveArguments.h

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
    struct MapSaveArguments
    {
        string outputMapFile = "";
    };

    /// A version of MapSaveArguments that adds its members to the command line parser
    ///
    struct ParsedMapSaveArguments : public MapSaveArguments, public ParsedArgSet
    {
        /// Adds the arguments to the command line parser
        ///
        virtual void AddArgs(CommandLineParser& parser);
    };
}
