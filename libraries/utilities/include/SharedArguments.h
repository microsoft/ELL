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

        string mapFile = "";
        int mapKeepLayers = 0;
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
            parser.AddOption(dataRandomSeedString, "dataRandomSeedString", "dr", "String used to initialize the random generator that permutes the dataset", "");

            parser.AddOption(mapFile, "mapFile", "mf", "Path to the map file", "");
            parser.AddOption(mapKeepLayers, "mapKeepLayers", "ml", "Number of map layers to use (0=all, positive=layers to keep from top, negative=layers to delete from bottom)", 0);
        }
    };
}