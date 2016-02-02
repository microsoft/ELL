// DataLoadArguments.h

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
    struct DataLoadArguments
    {
        std::string inputDataFile = "";
        bool inputDataFileHasWeights = false;
    };

    /// A version of DataLoadArguments that adds its members to the command line parser
    ///
    struct ParsedDataLoadArguments : public DataLoadArguments, public utilities::ParsedArgSet
    {
        /// Adds the arguments to the command line parser
        ///
        virtual void AddArgs(utilities::CommandLineParser& parser);

        /// Check arguments
        ///
        virtual utilities::ParseResult PostProcess(const utilities::CommandLineParser& parser);
    };
}
