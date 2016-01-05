// MapSaveArguments.cpp

#include "MapSaveArguments.h"

namespace utilities
{
    ParsedMapSaveArguments::ParsedMapSaveArguments(CommandLineParser& parser) : ParsedArgSet(parser)
    {
        AddArgs(parser);
    }

    void ParsedMapSaveArguments::AddArgs(CommandLineParser& parser)
    {
        parser.AddOption(
            outputMapFile,
            "outputMapFile",
            "omf",
            "Path to the output map file",
            "");
    }
}