// MapLoadArguments.cpp

#include "MapLoadArguments.h"

namespace utilities
{
    ParsedMapLoadArguments::ParsedMapLoadArguments(CommandLineParser& parser) : ParsedArgSet(parser)
    {
        AddArgs(parser);
    }

    void ParsedMapLoadArguments::AddArgs(CommandLineParser& parser)
    {
        parser.AddOption(
            inputMapFile,
            "inputMapFile",
            "imf",
            "Path to the input map file",
            "");
        
        parser.AddOption(
            inputMapIgnoreSuffix,
            "inputMapIgnoreSuffix",
            "imis",
            "The number of layers from the bottom of the input map to ignore",
            0);
    }
}