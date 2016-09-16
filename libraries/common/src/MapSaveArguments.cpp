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
            "Path to the output map file (empty for standard out, 'null' for no output)",
            "");
    }

    utilities::CommandLineParseResult ParsedMapSaveArguments::PostProcess(const utilities::CommandLineParser& parser)
    {
        if (outputMapFile == "null")
        {
            outputMapStream = utilities::OutputStreamImpostor(utilities::OutputStreamImpostor::StreamType::null);
            hasOutputStream = false;
        }
        else if (outputMapFile == "")
        {
            outputMapStream = utilities::OutputStreamImpostor(utilities::OutputStreamImpostor::StreamType::cout);
            hasOutputStream = true;
        }
        else // treat argument as filename
        {
            outputMapStream = utilities::OutputStreamImpostor(outputMapFile);
            hasOutputStream = true;
        }

        std::vector<std::string> parseErrorMessages;
        return parseErrorMessages;
    }
}
}
