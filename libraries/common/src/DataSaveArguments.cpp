// DataSaveArguments.cpp

#include "DataSaveArguments.h"

#include <vector>
using std::vector;

namespace common
{
    void ParsedDataSaveArguments::AddArgs(CommandLineParser& parser)
    {
        parser.AddOption(
            outputDataFile,
            "outputDataFile", 
            "odf",
            "Path to the output data file",
            "");
    }

    ParseResult ParsedDataSaveArguments::PostProcess(const CommandLineParser & parser)
    {
        vector<string> parseErrorMessages;

        return parseErrorMessages;
    }
}
