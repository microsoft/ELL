#include "..\include\DataSaveArguments.h"
// DataSaveArguments.cpp

#include "DataSaveArguments.h"

namespace utilities
{
    void ParsedDataSaveArguments::AddArgs(CommandLineParser& parser)
    {
        parser.AddOption(
            outputDataFile,
            "outputDataFile", 
            "odf",
            "Path to the output data file",
            "");

        parser.AddOption(
            outputDataFileHasWeights,
            "outputDataFileHasWeights",
            "odfhw",
            "Indicates whether the output data file format specifies a weight per example",
            false);
    }

    ParseResult ParsedDataSaveArguments::PostProcess(const CommandLineParser & parser)
    {
        vector<string> parseErrorMessages;

        if (outputDataFile == "")
        {
            parseErrorMessages.push_back("-outputDataFile (or -odf) is required");
        }

        return parseErrorMessages;
    }
}
