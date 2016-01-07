// MapSaveArguments.cpp

#include "MapSaveArguments.h"

namespace utilities
{
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
