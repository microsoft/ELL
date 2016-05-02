////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     DataSaveArguments.cpp (common)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "DataSaveArguments.h"

// stl
#include <vector>

namespace common
{
    void ParsedDataSaveArguments::AddArgs(utilities::CommandLineParser& parser)
    {
        parser.AddOption(
            outputDataFile,
            "outputDataFile", 
            "odf",
            "Path to the output data file",
            "");
    }

    utilities::CommandLineParseResult ParsedDataSaveArguments::PostProcess(const utilities::CommandLineParser & parser)
    {
        std::vector<std::string> parseErrorMessages;

        return parseErrorMessages;
    }
}
