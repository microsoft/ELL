// DataSaveArguments.h

#pragma once

#include "types.h"

// utilities
#include "CommandLineParser.h"
using utilities::CommandLineParser;
using utilities::ParsedArgSet;
using utilities::ParseResult;

// stl
#include <string>
using std::string;

namespace common
{
    /// A struct that holds command line parameters for loading maps
    ///
    struct DataSaveArguments
    {
        string outputDataFile = "";
    };

    /// A version of DataSaveArguments that adds its members to the command line parser
    ///
    struct ParsedDataSaveArguments : public DataSaveArguments, public ParsedArgSet
    {
        /// Adds the arguments to the command line parser
        ///
        virtual void AddArgs(CommandLineParser& parser);

        /// Check arguments
        ///
        virtual ParseResult PostProcess(const CommandLineParser& parser);
    };
}
