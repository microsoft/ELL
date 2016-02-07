// DataSaveArguments.h

#pragma once

// types
#include "types.h"

// utilities
#include "CommandLineParser.h"

// stl
#include <string>

namespace common
{
    /// A struct that holds command line parameters for loading maps
    ///
    struct DataSaveArguments
    {
        std::string outputDataFile = "";
    };

    /// A version of DataSaveArguments that adds its members to the command line parser
    ///
    struct ParsedDataSaveArguments : public DataSaveArguments, public utilities::ParsedArgSet
    {
        /// Adds the arguments to the command line parser
        ///
        virtual void AddArgs(utilities::CommandLineParser& parser);

        /// Check arguments
        ///
        virtual utilities::ParseResult PostProcess(const utilities::CommandLineParser& parser);
    };
}
