// DataSaveArguments.h

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
    struct DataSaveArguments
    {
        string outputDataFile = "";
        bool outputDataFileHasWeights = false;
    };

    /// A version of DataSaveArguments that adds its members to the command line parser
    ///
    struct ParsedDataSaveArguments : public DataSaveArguments, public ParsedArgSet
    {
        /// Ctor
        ///
        ParsedDataSaveArguments(CommandLineParser& parser);
        
        /// Adds the arguments to the command line parser
        ///
        virtual void AddArgs(CommandLineParser& parser);
    };
}