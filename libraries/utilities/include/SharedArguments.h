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
        string layersFile = "";
        string outputFile = "";
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
            parser.AddOption(layersFile, "layersFile", "lf", "Path to the layers file", "");
            parser.AddOption(outputFile, "outputFile", "of", "Path to the output file", "");
        }
    };
}