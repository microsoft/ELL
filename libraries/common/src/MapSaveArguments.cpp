////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Project:  Embedded Machine Learning Library (EMLL)
//  File:     MapSaveArguments.cpp (common)
//  Authors:  Ofer Dekel
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "MapSaveArguments.h"

namespace emll
{
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

    utilities::CommandLineParseResult ParsedMapSaveArguments::PostProcess(const utilities::CommandLineParser& parser)
    {
        if (outputMapFile == "null")
        {
            outputMapStream = utilities::OutputStreamImpostor(utilities::OutputStreamImpostor::StreamType::null);
        }
        else if (outputMapFile == "")
        {
            outputMapStream = utilities::OutputStreamImpostor(utilities::OutputStreamImpostor::StreamType::cout);
        }
        else // treat argument as filename
        {
            outputMapStream = utilities::OutputStreamImpostor(outputMapFile);
        }

        std::vector<std::string> parseErrorMessages;
        return parseErrorMessages;
    }
}
}
