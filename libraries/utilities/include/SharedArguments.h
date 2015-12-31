// SharedArguments.h

#pragma once

// utilities
#include "CommandLineParser.h"
using utilities::CommandLineParser;

// stl
#include <string>
using std::string;

namespace utilities
{
    /// A struct that holds the command line parameters for the application
    ///
    struct SharedArguments
    {
        string dataFile = "";
        bool dataFileHasWeights = false;
        string dataRandomSeedString = "";
        string inputMapFile = "";
        uint32 inputMapIgnoreSuffix = 0;
        string outputMapFile = "";
    };

    /// A subclass of app_arguments that knows how to Add its members to the command line parser
    ///
    struct ParsedSharedArguments : public SharedArguments, public ParsedArgSet
    {
        ParsedSharedArguments(CommandLineParser& parser)
        {
            AddArgs(parser);
        }

        virtual void AddArgs(CommandLineParser& parser)
        {
            parser.AddOption(dataFile, "dataFile", "df", "Path to the data file", "");
            parser.AddOption(dataFileHasWeights, "dataFileHasWeights", "hw", "Indicates whether the data file format specifies a weight per example", false);
            parser.AddOption(dataRandomSeedString, "dataRandomSeedString", "dr", "String used to initialize the random generator that permutes the dataset", "RandomString");
            parser.AddOption(inputMapFile, "inputMapFile", "imf", "Path to the input map file", "");
            parser.AddOption(inputMapIgnoreSuffix, "inputMapIgnoreSuffix", "imis", "The number of layers from the bottom of the input map to igrnore", 0);
            parser.AddOption(outputMapFile, "outputMapFile", "omf", "Path to the output map file", "");
        }
    };
}