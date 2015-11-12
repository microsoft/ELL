// shared_arguments.h

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
    struct shared_arguments
    {
        string data_file = "";
        bool data_file_has_weights = false;
        string data_random_seed_string = "";

        string map_file = "";
        int map_keep_layers = 0;
    };

    /// A subclass of app_arguments that knows how to Add its members to the command line parser
    ///
    struct ParsedSharedArguments : public shared_arguments, public ParsedArgSet
    {

        ParsedSharedArguments(CommandLineParser& parser)
        {
            AddArgs(parser);
        }

        virtual void AddArgs(CommandLineParser& parser)
        {
            parser.AddOption(data_file, "data_file", "df", "Path to the data file", "");
            parser.AddOption(data_file_has_weights, "data_file_has_weights", "hw", "Indicates whether the data file format specifies a weight per example", false);
            parser.AddOption(data_random_seed_string, "data_random_seed_string", "dr", "String used to initialize the random generator that permutes the dataset", "");

            parser.AddOption(map_file, "map_file", "mf", "Path to the map file", "");
            parser.AddOption(map_keep_layers, "map_keep_layers", "ml", "Number of map layers to use (0=all, positive=layers to keep from top, negative=layers to delete from bottom)", 0);
        }
    };
}