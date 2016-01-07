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
}
