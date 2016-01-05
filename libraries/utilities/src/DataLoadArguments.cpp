// DataLoadArguments.cpp

#include "DataLoadArguments.h"

namespace utilities
{
    ParsedDataLoadArguments::ParsedDataLoadArguments(CommandLineParser& parser) : ParsedArgSet(parser)
    {
        AddArgs(parser);
    }

    void ParsedDataLoadArguments::AddArgs(CommandLineParser& parser)
    {
        parser.AddOption(
            inputDataFile,
            "inputDataFile", 
            "idf",
            "Path to the input data file",
            "");

        parser.AddOption(
            inputDataFileHasWeights,
            "inputDataFileHasWeights",
            "idfhw",
            "Indicates whether the input data file format specifies a weight per example",
            false);
    }
}