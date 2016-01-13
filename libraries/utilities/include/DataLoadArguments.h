// DataLoadArguments.h

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
    struct DataLoadArguments
    {
        string inputDataFile = "";
        bool inputDataFileHasWeights = false;
    };

    /// A version of DataLoadArguments that adds its members to the command line parser
    ///
    struct ParsedDataLoadArguments : public DataLoadArguments, public ParsedArgSet
    {
        /// Adds the arguments to the command line parser
        ///
        virtual void AddArgs(CommandLineParser& parser);

        /// Check arguments
        ///
        virtual CommandLineParser::ParseResult PostProcess(const CommandLineParser& parser);
    };
}
