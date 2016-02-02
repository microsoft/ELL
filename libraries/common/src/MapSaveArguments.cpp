// MapSaveArguments.cpp

#include "MapSaveArguments.h"

namespace common
{
    void ParsedMapSaveArguments::AddArgs(utilities::CommandLineParser& parser)
    {
        parser.AddOption(
            outputMapFile,
            "outputMapFile",
            "omf",
            "Path to the output map file",
            "");
    }
}
