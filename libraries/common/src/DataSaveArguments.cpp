////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Learning Library (ELL)
//  File:     DataSaveArguments.cpp (common)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "DataSaveArguments.h"

// stl
#include <vector>

namespace ell
{
namespace common
{
    void ParsedDataSaveArguments::AddArgs(utilities::CommandLineParser& parser)
    {
        parser.AddOption(
            outputDataFilename,
            "outputDataFilename",
            "odf",
            "Path to the output data file",
            "");
    }

    utilities::CommandLineParseResult ParsedDataSaveArguments::PostProcess(const utilities::CommandLineParser& parser)
    {
        if (outputDataFilename == "null")
        {
            outputDataStream = utilities::OutputStreamImpostor(utilities::OutputStreamImpostor::StreamType::null);
        }
        else if (outputDataFilename == "")
        {
            outputDataStream = utilities::OutputStreamImpostor(utilities::OutputStreamImpostor::StreamType::cout);
        }
        else // treat argument as filename
        {
            outputDataStream = utilities::OutputStreamImpostor(outputDataFilename);
        }

        std::vector<std::string> parseErrorMessages;
        return parseErrorMessages;
    }
}
}
